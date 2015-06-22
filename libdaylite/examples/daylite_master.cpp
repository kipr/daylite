#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "daylite/node.hpp"
#include "daylite/spinner.hpp"

using namespace daylite;

int main(int argc, char *argv[])
{
  node *me = node::create_node("master");
  if(!me)
  {
    std::cerr << "could not create the node" << std::endl;
    return 1;
  }

  auto ret = me->start_gateway_service("", 8374);
  if(!me)
  {
    std::cerr << "Cannot start the gateway service" << std::endl;
    return 1;
  }

  auto sub = me->subscribe("/test", [](const uint8_t *data, uint32_t size) { std::cout << "Got packet " << data << std::endl; });
  if(!sub)
  {
    std::cerr << "Could not subscribe to topic /test" << std::endl;
    node::destroy_node(me);
    return 1;
  }

  for(;;)
  {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Tick..." << std::endl;
    spinner::spin_once();
  }
  
  me->destroy_subscriber(sub);
  node::destroy_node(me);
  
  return 0;
}