#include "ioqueue.h"
#include "safs_streamline.h"

// class SAFSStreamline
//
// Takes request in the form of lists of files (maintaining an input queue)
//   and registers a callback function for the set of files
//
// Once all files are read, it invokes the callback and returns a list of buffers.


StreamlineWorker::StreamlineWorker ( IOQueue& ioqref ):
  ioslot(0), ioq(ioqref)
{};

// Dequeue an I/O, send to FlashGraph, set up callbacks.
int StreamlineWorker::process ( )
{
  IOQueue::ioqel* qel = ioq.dequeue();

  if ( qel == NULL )
  {
    return 0;
  }

  // convert into a workitem 
  StreamlineWorker::workitem* wi = new workitem();
  wi->ioslot = ioslot;
  wi->length = (qel->files).size();
  wi->seed = qel->seed;
  wi->files = qel->files;
  for ( int i=0; i<wi->length; i++ )
  {
    wi->buffers[i] = NULL;
  }

  // Create a SAFS request for each element.

  // Delete the qelement
  delete (qel);

  return 1;
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


// testing


    void printwi ( workitem* wi )
    {
      std::cout << "length " << wi->length <<", seed " <<  std::get<0>(wi->seed) << ", " <<  std::get<1>(wi->seed) <<  ", " << std::get<2>(wi->seed);
      for (int i=0; i<wi->length; i++)
      {
        std::cout << "\nBuffer = " << wi->buffers[i];
      }
      std::cout << std::endl;
    }
};
 */


int main ( )
{
  IOQueue ioq; 

  std::tuple<double,double,double> seed0 { 0.23, 0.34, 0.45 };
  std::tuple<double,double,double> seed1 { 1.23, 1.34, 1.45 };
  std::tuple<double,double,double> seed2 { 2.23, 2.34, 2.45 };
  const char * filesp[] = { "file1", "file2", "file3", "file4" };
  std::vector<std::string> files (filesp, std::end(filesp));

  ioq.enqueue( seed0, files );
  ioq.enqueue( seed1, files );
  ioq.enqueue( seed2, files );
  ioq.printioq();

  ioq.enqueue( seed0, files );
  ioq.enqueue( seed1, files );
  ioq.enqueue( seed2, files );

  IOQueue::ioqel* qel = ioq.dequeue();
  while ( qel != NULL )
  {
    std::cout << "Dequeued an element." << std::endl;
    qel = ioq.dequeue();
  };
/*
  workitem * wi = ss.dequeue ();
  while ( wi != NULL )
  {
    ss.printwi ( wi );
    ss.complete ( wi );
    wi = ss.dequeue ();
  }
*/


/*
        os  << "buffer" << qel->files[i] << "." << i;
        std::string s = os.str();
        char* buf = (char*) malloc ( sizeof(char) * s.length() );
        strncpy ( buf, s.c_str(), s.length() );
        (wi->buffers).push_back((unsigned char *)buf);
*/
}
