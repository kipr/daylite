#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include <bson.h>

#include "daylite/node.hpp"
#include "daylite/spinner.hpp"

#include "string_msg.hpp"

using namespace daylite;
using namespace std;

void print_bson(const bson_t *const msg)
{
  size_t len;
  char *str;
  str = bson_as_json (msg, &len);
  cout << str << endl;
  bson_free (str);
}

int main(int argc, char *argv[])
{
  auto me = node::create_node("node");
  if(!me)
  {
    std::cerr << "could not create the node" << std::endl;
    return 1;
  }

  bool ret = me->join_daylite("127.0.0.1", 8374);
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
  
  auto sub = me->subscribe("/test", [](const bson_t *msg, void *)
    {
    print_bson(msg);
    });

  for(;;)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    string_msg msg;
    msg.value = "test";   
    pub->publish(msg.bind());
    spinner::spin_once();
    cout << "." << endl;
  }
  
  return 0;
}
