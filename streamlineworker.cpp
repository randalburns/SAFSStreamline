#include <unordered_map>
#include "ioqueue.h"
#include "streamlineworker.h"

#include "io_interface.h"

using namespace safs;

// class SAFSStreamline
//
// Takes request in the form of lists of files (maintaining an input queue)
//   and registers a callback function for the set of files
//
// Once all files are read, it invokes the callback and returns a list of buffers.

StreamlineWorker::StreamlineWorker ( int workerid, IOQueue& ioqref,  file_io_factory::shared_ptr factoryp ):
  wid(workerid), ioq(ioqref), iostatus(IODEPTH, 0), workers(IODEPTH), factory(factoryp)
{
  io = create_io(factory, thread::get_curr_thread());
  io->set_callback(callback::ptr(new SSCallback(this)));
}

std::string StreamlineWorker::ioslot_key ( int fileid, int off )
{
  std::ostringstream os;
  os << fileid << "," << off;
  return os.str();
}

// Dequeue an I/O, send to FlashGraph, set up callbacks.
void StreamlineWorker::process ( )
{
  IOQueue::ioqel* qel = NULL;

  // Loop until no more I/Os
  do {

    for (int ioslot=0; ioslot<IODEPTH; ioslot++)
    {
      // start IO on all open slots
      if (iostatus[ioslot] == 0)
      {
        qel = ioq.dequeue();

        if ( qel == NULL )
        {
          break;  //RB this breaks the for loop?
        }

        // convert into a workitem 
        StreamlineWorker::workitem* wi = new workitem();
        wi->streamline = qel->streamline;
        wi->ioslot = ioslot;
        wi->seed = qel->seed;
        wi->cuboid = qel->cuboid;

        // RBTODO buffers is wrong.  We are going to have to allocate a buffer to cut data into and
        // keep a count of I/O requests

//        for ( unsigned int i=0; i<wi->ranges.size(); i++ )
//        {
//          wi->buffers.push_back(NULL);
//        }

        // add the workitem to the workers
        workers[ioslot] = wi;

        // Update i/o status
        iostatus[ioslot] = 1;

        // create the SAFS requests
        // This is direct I/O. Memory buffer, I/O offset and I/O size
        // all need to be aligned to the I/O block size.

        // do an experiment do a 512 aligned I/O to an mutliplied offset
        size_t io_size = ROUNDUP_PAGE(500);
        data_loc_t loc(factory->get_file_id(), 4096*wi->ioslot);
        // The buffer will be free'd in the callback function.
        char *buf = (char *) valloc(io_size);
        io_request req(buf, loc, io_size, READ);
        io->access(&req, 1);

        // update the offset map
        std::string s = ioslot_key (loc.get_file_id(), loc.get_offset());
        offset2ioslot.emplace(s, ioslot);
        std::cout << "Request: thread=" << wid << " streamline=" << wi->streamline << ", Off=" << loc.get_offset() << ", ioslot=" << ioslot << std::endl;

        // Delete the qelement
        delete (qel);
      }
    } //endfor -- IOslots full
    
    // wait on I/O when an I/O is complete, restart the do loop
    io->wait4complete(1);
  } 
  while ( qel != NULL ); //end do while -- no more I/Os

  return;
}

//testing 
void StreamlineWorker::printworkers ( )
{
  StreamlineWorker::workitem* wp;

  for (unsigned int i=0; i<workers.size(); i++)
  { 
    wp = workers[i];
    std::cout << "Streamline " << wp->streamline << ", IOSlot " << wp->ioslot << ", Seed " << std::get<0>(wp->seed) << ", " <<  std::get<1>(wp->seed) <<  ", " << std::get<2>(wp->seed) << ", Cuboid ";
     std::cout << ", " << std::get<0>(wp->cuboid) << ", "
               << std::get<1>(wp->cuboid) << ", " 
               << std::get<2>(wp->cuboid) << ", "
               << std::get<3>(wp->cuboid) << ", " 
               << std::get<4>(wp->cuboid) << ", "
               << std::get<5>(wp->cuboid) << ", " 
               << std::get<6>(wp->cuboid);
    std::cout << std::endl;
  } 
}


SSCallback::SSCallback ( StreamlineWorker * sw ):
  sworker ( sw )
{}

// SAFS callback function
int SSCallback::invoke ( io_request *reqs[], int num )
{
  for (int i = 0; i < num; i++) {
    char *buf = reqs[i]->get_buf();
    off_t offset = reqs[i]->get_offset();
    file_id_t fid = reqs[i]->get_file_id();

// RBTODO iterate over all waiting I/Os
   
    std::string ioskey = sworker->ioslot_key(fid,offset);
    int ioslot = sworker->offset2ioslot.find(ioskey)->second;
    std::cout << "Callback Thread " << sworker->wid << "Offset " << offset << " corresponds to ioslot " << ioslot << std::endl;
    sworker->offset2ioslot.erase(ioskey);
    sworker->iostatus[ioslot]=0;

//        run_computation(buf, reqs[i]->get_size());
    free(buf);
  }
  return 0;
}
