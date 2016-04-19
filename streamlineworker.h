
#ifndef h_safs_streamline
#define h_safs_streamline


#include<stdio.h>
#include<string.h>
#include<iostream>
#include<sstream>
#include<string>
#include<tuple>
#include<queue>
#include<array>

#include "ioqueue.h"


// class SAFSStreamline
//
// Takes request in the form of lists of files (maintaining an input queue)
//   and registers a callback function for the set of files
//
// Once all files are read, it invokes the callback and returns a list of buffers.


class StreamlineWorker {

  public:

    struct workitem
    { 
      int ioslot;
      int length;
      std::tuple<float,float,float> seed;
      std::vector<std::string> files;
      std::vector<unsigned char*> buffers;
    };

//    static const int IODEPTH = 64;
    static const int IODEPTH = 8;

  private: 

      int ioslot;
      IOQueue& ioq;
      std::vector<char> iostatus;

      // output queue -- lists of buffers
      std::vector<workitem> worker;

  public: 

    StreamlineWorker ( IOQueue& ioqref );

    // Start IODEPTH number of workers
    // Dequeue an I/O, send to FlashGraph, set up callbacks.
    void process ( );
};

#endif
