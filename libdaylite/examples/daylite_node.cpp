#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "daylite/node.hpp"
#include "daylite/spinner.hpp"

using namespace daylite;
using namespace std;

int main(int argc, char *argv[])
{
  node *me = node::create_node("node");
  if(!me)
  {
    std::cerr << "could not create the node" << std::endl;
    return 1;
  }

  bool ret = me->join_daylite("127.0.0.1", 8374);
  if(!ret)
  {
    std::cerr << "Could not connect to the daylite gaitway" << std::endl;
    node::destroy_node(me);
    return 1;
  }

  auto pub = me->advertise("/test");
  if(!pub)
  {
    std::cerr << "Could not advertise to topic /test" << std::endl;
    node::destroy_node(me);
    return 1;
  }

  for(;;)
  {
    pub->publish((const uint8_t*) "Hello World", strlen("Hello World") + 1);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Tick..." << std::endl;
    spinner::spin_once();
  }
  
  me->destroy_publisher(pub);
  node::destroy_node(me);
  
  return 0;
}