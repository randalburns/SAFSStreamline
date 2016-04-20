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

StreamlineWorker::StreamlineWorker ( IOQueue& ioqref ):
  ioq(ioqref), iostatus(IODEPTH, 0), workers(IODEPTH)
{
};

// Dequeue an I/O, send to FlashGraph, set up callbacks.
void StreamlineWorker::process ( )
{
  IOQueue::ioqel* qel;
  
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
        wi->ranges = qel->ranges;
        for ( unsigned int i=0; i<wi->ranges.size(); i++ )
        {
          wi->buffers.push_back(NULL);
        }

        // add the workitem to the workers
        workers[ioslot] = wi;

        // Update i/o status
        iostatus[ioslot] = 1;

        // create the SAFS requests

        // Delete the qelement
        delete (qel);
      }
    } //endfor -- 
    
    printworkers();
 
    assert(0);
    // wait on I/O when an I/O is complete, restart the do loop

  }
  while ( qel != NULL );

  return;
}

//testing 
void StreamlineWorker::printworkers ( )
{
  StreamlineWorker::workitem* wp;
 
  for (unsigned int i=0; i<workers.size(); i++)
  { 
    wp = workers[i];
    std::cout << "Streamline " << wp->streamline << ", IOSlot " << wp->ioslot << ", Seed " << std::get<0>(wp->seed) << ", " <<  std::get<1>(wp->seed) <<  ", " << std::get<2>(wp->seed) << ", Ranges ";
    for (unsigned int j=0; j<wp->ranges.size(); j++)
    {
       std::cout << ", " << std::get<0>(wp->ranges[j]) << ", "
                 << std::get<1>(wp->ranges[j]) << ", " 
                 << std::get<2>(wp->ranges[j]) << ", "
                 << std::get<3>(wp->ranges[j]) << ", " 
                 << std::get<4>(wp->ranges[j]) << ", "
                 << std::get<5>(wp->ranges[j]) << ", " 
                 << std::get<6>(wp->ranges[j]);
    }
    std::cout << std::endl;
  } 
}



// SAFS callback function
int SSCallback::handleIO ( io_request *reqs[], int num )
{
  assert(0);

/*  //RBTODO
  if ( moreio for workerel ) 
  {
     wait longer
     return 0;
  } else {
    // Call JP's streamline integration
    //     with a IOcompletion structure
    //  JP returns a new seed and new list of files
    if ( return_code == morework )
    {
      // enqueue new IO
    }
    else
    {
      streamline done.
    }
  }
*/
}


/*
    // Return a seed and data for which I/O is finished.
    workitem* dequeue ( )
    {
      if ( workerq.empty() )
      {
        return NULL;
      } else {
        workitem * wi = workerq.front();
        workerq.pop();
        return wi;
      }
    }

    // Release complete work 
    void complete ( workitem* wi )
    {
      // itereate over buffers and release
      for (int i; i< wi->length; i++ )
      {
        free ( wi->buffers[i] );
      }
      delete ( wi );
    }
*/
