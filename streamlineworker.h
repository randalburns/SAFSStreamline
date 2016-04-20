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

#include "io_interface.h"
using namespace safs;

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
//      int streamline_id;
      int ioslot;
      int length;
      std::tuple<float,float,float> seed;
      std::vector<std::tuple<std::string,int,int>> ranges;
      std::vector<unsigned char*> buffers;
    };

//    static const int IODEPTH = 64;
    static const int IODEPTH = 8;

  private: 

      int ioslot;
      IOQueue& ioq;
      std::vector<char> iostatus;

      // output queue -- lists of buffers
      std::vector<workitem*> workers;

      // IO callback
    
     

  public: 

    StreamlineWorker ( IOQueue& ioqref );

    // Start IODEPTH number of workers
    // Dequeue an I/O, send to FlashGraph, set up callbacks.
    void process ( );

};

class SSCallback : public callback
{
  public: 
    virtual int handleIO ( io_request *reqs[], int num );
};

    struct streamIOcomplete
    { 
      int streamline_id;
      int length;
      std::tuple<float,float,float> seed;
      std::vector<int> buflengths;
      std::vector<unsigned char*> buffers;
      int return_code;
    };

#endif
