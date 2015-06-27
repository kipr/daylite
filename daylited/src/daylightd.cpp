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

  auto sub = me->subscribe("/test", [](const bson_t *msg, void *)
    {
      std::cout << "Got packet" << std::endl;

      bson_iter_t it;
      if(bson_iter_init(&it, msg))
      {
        while(bson_iter_next(&it))
        {
          std::cout << "Key: " << bson_iter_key(&it);
          if(BSON_ITER_HOLDS_BOOL(&it)) std::cout << " Value (bool): " << bson_iter_bool(&it);
          else if(BSON_ITER_HOLDS_INT32(&it)) std::cout << " Value (int32): " << bson_iter_int32(&it);
          else if(BSON_ITER_HOLDS_UTF8(&it)) std::cout << " Value (utf8): " << bson_iter_utf8(&it, nullptr);
          std::cout << std::endl;
        }
      }
    });
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