#include<tuple>
#include<string>
#include<vector>
#include<string>
#include<iostream>
#include<sstream>

#include "ioqueue.h"
#include "streamlineworker.h"

using namespace safs;


int main ()
{
  IOQueue ioq;

  //  initialize the SAFS system
  config_map::ptr safs_configs = config_map::create("/safs/local.txt");
  init_io_system(safs_configs);
  file_io_factory::shared_ptr factory = create_io_factory("ssfile",REMOTE_ACCESS);
  

  //Create 1000 seeds and queue them up for I/O
  for (int i=0; i<32; i++)
  {
    // initialize a seed
    std::tuple<double,double,double> seed { 0.1+i, 0.2+i, 0.3+i };

    // make a file name
    std::ostringstream os;
    os << "file" << i;
    std::string s = os.str();

    std::vector<std::string> filenames;

    filenames.push_back( s );

    ioq.enqueue( seed, filenames );
  }

  // Look at the queue.  This is destructive
  //ioq.printioq();

  // create parallel threads
  #pragma omp parallel
  {
    // Create a streamline worker
    StreamlineWorker sw ( ioq );

    // start the I/O and streamline update process
    sw.process ();
  }
}




// dead text
/*

//    const char * filesp[] = { "file1", "file2", "file3", "file4" };
    const char * filesp[] = { "file1", "file2", "file3", "file4" };
    std::vector<std::string> filenames (filesp, std::end(filesp));

*/
