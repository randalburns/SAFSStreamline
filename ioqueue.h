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
      int streamline;
      std::tuple<double,double,double> seed;
      // version for z-index cubes 
      // std::vector<std::tuple<std::string,int,int>> ranges;
      // Version for vtk xmin, ymin, zmin, ...
      std::tuple<std::string,int,int,int,int,int,int> cuboid;
    };

  private:

    // input queue -- lists of files
    std::queue<ioqel*> ioq;
  
    // mutex for queue operations
    std::mutex q_mtx;

  public: 

    // place an item on the IO queue
    void enqueue ( int strealine, std::tuple<float,float,float> seed, std::tuple<std::string,int,int,int,int,int,int> cuboid );

    // remove an item from the IO queue
    ioqel* dequeue ( );

    //testing 
    void printioq ( );
};

#endif
