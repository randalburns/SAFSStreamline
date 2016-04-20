#include "ioqueue.h"

// place an item on the IO queue
void IOQueue::enqueue ( int streamline, std::tuple<float,float,float> seed, std::vector<std::tuple<std::string,int,int,int,int,int,int>> ranges )
{
  IOQueue::ioqel* qel = new IOQueue::ioqel();
  qel->streamline = streamline;
  qel->seed = seed;
  qel->ranges = ranges;
  
  q_mtx.lock();
  ioq.push ( qel );
  q_mtx.unlock();
} 

// remove an item from the IO queue
IOQueue::ioqel* IOQueue::dequeue ( )
{
  q_mtx.lock();
  if (ioq.empty())
  {
    q_mtx.unlock();
    return NULL;
  }
  //dequeue from ioq
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
    std::cout << "Streamline " << elp->streamline << ", Seed " << std::get<0>(elp->seed) << ", " <<  std::get<1>(elp->seed) <<  ", " << std::get<2>(elp->seed) << ", Ranges ";
    for (unsigned int i=0; i<elp->ranges.size(); i++)
    {
       std::cout << ", " << std::get<0>(elp->ranges[i]) << ", "
                 << std::get<1>(elp->ranges[i]) << ", " 
                 << std::get<2>(elp->ranges[i]) << ", "
                 << std::get<3>(elp->ranges[i]) << ", " 
                 << std::get<4>(elp->ranges[i]) << ", "
                 << std::get<5>(elp->ranges[i]) << ", " 
                 << std::get<6>(elp->ranges[i]);
    }
    std::cout << std::endl;

    ioq.pop();
  } 
}
