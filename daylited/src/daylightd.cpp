#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include <bson.h>

#include "daylite/node.hpp"
#include "daylite/spinner.hpp"

using namespace daylite;

int main(int argc, char *argv[])
{
  auto me = node::create_node("master");
  if(!me)
  {
    std::cerr << "could not create the node" << std::endl;
    return 1;
  }

  auto ret = me->gateway("", 8374);
  if(!me)
  {
    std::cerr << "Cannot start the gateway service" << std::endl;
    return 1;
  }
  
  for(;;)
  {
    spinner::spin_once();
    usleep(me->thread_sleep_duration());
  }
  
  return 0;
}
