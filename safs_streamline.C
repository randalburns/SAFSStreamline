
#include<stdio.h>
#include<string.h>
#include<iostream>
#include<sstream>
#include<string>
#include<tuple>
#include<queue>
#include<array>

// class SAFSStrealine
//
// Takes request in the form of lists of files (maintaining an input queue)
//   and registers a callback function for the set of files
//
// Once all files are read, it invokes the callback and returns a list of buffers.


struct workitem
{ 
  int length;
  std::tuple<float,float,float> seed;
  std::vector<unsigned char*> buffers;
};


class SAFSStreamline {

  private: 
    // elements in the io queue
    struct ioqel {
      std::tuple<double,double,double> seed;
      std::vector<std::string> files;
    };

    // input queue -- lists of files
    std::queue<ioqel*> ioq;

    // output queue -- lists of buffers
    std::queue<workitem*> workerq;

  public: 

    // place an item on the 
    void enqueue ( std::tuple<float,float,float> seed, std::vector<std::string> files )
    {
      ioqel* qel = new ioqel();
      qel->seed = seed;
      qel->files = files;
      ioq.push ( qel );
    } 

    // Fake placeholder to turn a ioqel into a workitem
    // return 0 when no more work, 1 otherwise
    int process ( )
    {
      if (ioq.empty())
      {
        return 0;
      }
      
      //dequeue from ioq
      ioqel* qel = ioq.front();
      ioq.pop();

      // convert into a workitem 
      workitem* wi = new workitem();
      wi->length = (qel->files).size();
      wi->seed = qel->seed;
      for ( int i=0; i<wi->length; i++ )
      {
        std::ostringstream os;
        os  << "buffer" << qel->files[i] << "." << i;
        std::string s = os.str();
        char* buf = (char*) malloc ( sizeof(char) * s.length() );
        strncpy ( buf, s.c_str(), s.length() );
        (wi->buffers).push_back((unsigned char *)buf);
      }
  
      // Add to the worker queue
      workerq.push(wi);

      // Delete the qelement
      delete (qel);

      return 1;
    }

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

    void printioq ( )
    {
      ioqel* elp;
     
      while ( not ioq.empty() )
      {
        elp = ioq.front();
        std::cout << std::get<0>(elp->seed) << ", " <<  std::get<1>(elp->seed) <<  ", " << std::get<2>(elp->seed) <<
           elp->files[0] << elp->files[1] << elp->files[2] << std::endl;
        ioq.pop();
      } 
    }

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


int main ( )
{
  SAFSStreamline ss;

  std::tuple<double,double,double> seed0 { 0.23, 0.34, 0.45 };
  std::tuple<double,double,double> seed1 { 1.23, 1.34, 1.45 };
  std::tuple<double,double,double> seed2 { 2.23, 2.34, 2.45 };
  const char * filesp[] = { "file1", "file2", "file3", "file4" };
  std::vector<std::string> files (filesp, std::end(filesp));

  ss.enqueue( seed0, files );
  ss.enqueue( seed1, files );
  ss.enqueue( seed2, files );
  ss.printioq();

  ss.enqueue( seed0, files );
  ss.enqueue( seed1, files );
  ss.enqueue( seed2, files );

  while ( ss.process() ) {};

  workitem * wi = ss.dequeue ();
  while ( wi != NULL )
  {
    ss.printwi ( wi );
    ss.complete ( wi );
    wi = ss.dequeue ();
  }
}
