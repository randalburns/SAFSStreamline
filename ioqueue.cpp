#include "ioqueue.h"

// place an item on the IO queue
void IOQueue::enqueue ( int streamline, std::tuple<float,float,float> seed, std::tuple<std::string,int,int,int,int,int,int> cuboid )
{
  IOQueue::ioqel* qel = new IOQueue::ioqel();
  qel->streamline = streamline;
  qel->seed = seed;
  qel->cuboid = cuboid;
  
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
    std::cout << "Streamline " << elp->streamline << ", Seed " << std::get<0>(elp->seed) << ", " <<  std::get<1>(elp->seed) <<  ", " << std::get<2>(elp->seed) << ", Cuboid ";
    std::cout << ", " << std::get<0>(elp->cuboid) << ", "
              << std::get<1>(elp->cuboid) << ", " 
              << std::get<2>(elp->cuboid) << ", "
              << std::get<3>(elp->cuboid) << ", " 
              << std::get<4>(elp->cuboid) << ", "
              << std::get<5>(elp->cuboid) << ", " 
              << std::get<6>(elp->cuboid);
    std::cout << std::endl;

    ioq.pop();
  } 
}
