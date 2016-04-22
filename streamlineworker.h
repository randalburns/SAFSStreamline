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

  friend class SSCallback;

  public:

    struct workitem
    { 
      int streamline;
      int ioslot;
      std::tuple<float,float,float> seed;
      std::vector<std::tuple<std::string,int,int,int,int,int,int>> ranges;
      std::vector<unsigned char*> buffers;
    };

//    static const int IODEPTH = 64;
    static const int IODEPTH = 2;

  private: 

      int ioslot;
      IOQueue& ioq;
      std::vector<char> iostatus;

      // output queue -- lists of buffers
      std::vector<workitem*> workers;

      // File I/O factory
      file_io_factory::shared_ptr factory; 

      // I/O interface
      io_interface::ptr io;
  
      // Reverse lookup for I/O
      std::unordered_multimap<std::string,int> offset2ioslot;  

      std:string safs_hash_key ( file_id_t fid, off_t offset ); 

  public: 

    StreamlineWorker ( IOQueue& ioqref, file_io_factory::shared_ptr factoryp );

    // Start IODEPTH number of workers
    // Dequeue an I/O, send to FlashGraph, set up callbacks.
    void process ( );

    // testing
    void printworkers();

};

class SSCallback : public callback
{
  private:
    StreamlineWorker* sworker;

  public: 
    virtual int invoke ( io_request *reqs[], int num );
    SSCallback ( StreamlineWorker * sw );
};

#endif
