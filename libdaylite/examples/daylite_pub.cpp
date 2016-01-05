#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include <bson.h>

#include "daylite/node.hpp"
#include "daylite/spinner.hpp"
#include "daylite/util.hpp"

#include "string_msg.hpp"

using namespace daylite;
using namespace std;

int main(int argc, char *argv[])
{
  auto me = node::create_node("node");
  if(!me)
  {
    std::cerr << "could not create the node" << std::endl;
    return 1;
  }

  bool ret = me->start("127.0.0.1", 8374);
  if(!ret)
  {
    std::cerr << "Could not connect to the daylite gateway" << std::endl;
    return 1;
  }

  auto pub = me->advertise("/test");
  if(!pub)
  {
    std::cerr << "Could not advertise to topic /test" << std::endl;
    return 1;
  }
  pub->set_firehose(true);
  
  
  uint32_t i = 0;
  while(!should_exit())
  {
    string_msg msg;
    msg.value = "test" + to_string(i++);
    pub->publish(bson(msg.bind()));
    spinner::spin_once();
    usleep(1000U);
  }
  
  return 0;
}
