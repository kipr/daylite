#if 0
#include "daylite/tcp_transport.hpp"
#include <iostream>

using namespace daylite;
using namespace std;

int main(int argc, char *argv[])
{
  socket_address addr("127.0.0.1", 8374);
  tcp_transport client(addr);
  void_result ret = client.open();
  if(!ret)
  {
    cerr << ret.message() << endl;
    return 1;
  }
  
  for(;;)
  {
    auto in = client.input();
    if(in.none()) break;
    auto packet_result = in.unwrap()->read();
    if(packet_result.success())
    {
      auto packet = packet_result.unwrap();
      cout << reinterpret_cast<const char *>(packet.data()) << endl;
    }
    spinner::ref().spin_once();
  }
  
  return 0;
}
#else
int main(int argc, char *argv[])
{

  return 0;
}
#endif