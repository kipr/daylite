#include "daylite/tcp_server_transport.hpp"
#include "daylite/rate.hpp"

#include <iostream>

using namespace daylite;
using namespace std;
using namespace std::chrono;

struct hello_world : private spinett
{
  hello_world(output_channel *const out)
    : _r(milliseconds(1000)), _out(out) {}
  
  void_result spin_update()
  {
    if(!_r.should_fire()) return success();
    _out->write("Hello, World!");
    return success();
  }
  
  rate _r;
  output_channel *_out;
};

int main(int argc, char *argv[])
{
  tcp_socket server;
  
  socket_address addr(8374);
  tcp_server_transport transport(addr);
  auto ret = transport.open();
  if(!ret)
  {
    cerr << ret.message() << endl;
    return 1;
  }
  
  hello_world hello(transport.output().unwrap());
  
  spinner::ref().spin();
  
  return 0;
}