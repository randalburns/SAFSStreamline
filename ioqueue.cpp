#include "ioqueue.h"

// place an item on the IO queue
void IOQueue::enqueue ( std::tuple<float,float,float> seed, std::vector<std::string> files )
{
  IOQueue::ioqel* qel = new IOQueue::ioqel();
  qel->seed = seed;
  qel->files = files;
  
  q_mtx.lock();
  ioq.push ( qel );
  q_mtx.unlock();
} 

// remove an item from the IO queue
IOQueue::ioqel* IOQueue::dequeue ( )
{
  // Don't protect.  If there's no works. there's no work.
  if (ioq.empty())
  {
    return NULL;
  }
  //dequeue from ioq
  q_mtx.lock();
  IOQueue::ioqel* qel = ioq.front();
  ioq.pop();
  q_mtx.unlock();
  return qel;
}

//testing 
void IOQueue::printioq ( )
{
  IOQueue::ioqel* elp;
 
  while ( not ioq.empty() )
  {
    elp = ioq.front();
    std::cout << std::get<0>(elp->seed) << ", " <<  std::get<1>(elp->seed) <<  ", " << std::get<2>(elp->seed) <<
       elp->files[0] << elp->files[1] << elp->files[2] << std::endl;
    ioq.pop();
  } 
}
