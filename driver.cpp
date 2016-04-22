#include<tuple>
#include<string>
#include<vector>
#include<string>
#include<iostream>
#include<sstream>

#include<omp.h>

#include "ioqueue.h"
#include "streamlineworker.h"

using namespace safs;


int main ()
{
  IOQueue ioq;

  //  initialize the SAFS system
  config_map::ptr safs_configs = config_map::create("/safs/local.txt");
  init_io_system(safs_configs);
  file_io_factory::shared_ptr factory = create_io_factory("ssfile", REMOTE_ACCESS);

  //Create 1000 seeds and queue them up for I/O
  for (int i=0; i<16; i++)
  {
    // initialize a seed
    std::tuple<double,double,double> seed { 0.1+i, 0.2+i, 0.3+i };

    // make a file name
    std::ostringstream os;
    os << "file" << i;
    std::string s = os.str();

    std::tuple<std::string,int,int,int,int,int,int> cuboid { s, 100+i, 101+i, 102+1, 200+i, 201+i, 202+i };

    ioq.enqueue( i, seed, cuboid );
  }

  // Look at the queue.  This is destructive
  // ioq.printioq();

  // create parallel threads
  #pragma omp parallel num_threads(2)
//  #pragma omp parallel num_threads(1)
  {
    // Create a streamline worker
    StreamlineWorker sw ( omp_get_thread_num(), ioq, factory );
//    StreamlineWorker sw ( omp_get_thread_num(), ioq, NULL );

    // start the I/O and streamline update process
    sw.process ();
  }
}

