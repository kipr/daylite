#include "daylite/node.hpp"
#include "daylite/spinner.hpp"
#include <iostream>

using namespace daylite;
using namespace std;

int main(int argc, char *argv[])
{
  node me("me");
  
  void_result ret = me.start(socket_address("127.0.0.1", 8374));
  if(!ret)
  {
    cerr << ret.message() << endl;
    return 1;
  }
  
  spinner::ref().spin();
  
  me.end();
  
  return 0;
}