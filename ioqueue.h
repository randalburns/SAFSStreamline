
#ifndef ioqueue_h
#define ioqueue_h

#include<stdio.h>
#include<string.h>
#include<iostream>
#include<sstream>
#include<string>
#include<tuple>
#include<queue>
#include<mutex>


class IOQueue {

  public:
    // elements in the IO queue -- this is arbitrarilty large and shared by all threads
    struct ioqel {
      std::tuple<double,double,double> seed;
      std::vector<std::string> files;
    };

  private:

    // input queue -- lists of files
    std::queue<ioqel*> ioq;
  
    // mutex for queue operations
    std::mutex q_mtx;

  public: 

    // place an item on the IO queue
    void enqueue ( std::tuple<float,float,float> seed, std::vector<std::string> files );

    // remove an item from the IO queue
    ioqel* dequeue ( );

    //testing 
    void printioq ( );
};

#endif
