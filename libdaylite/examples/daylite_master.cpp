#include "daylite/node.hpp"
#include "daylite/spinner.hpp"
#include <iostream>

using namespace daylite;
using namespace std;

int main(int argc, char *argv[])
{
  node me("master");
  void_result ret = me.start(none<socket_address>(), some(socket_address(8374)));
  if(!ret)
  {
    cerr << ret.message() << endl;
    return 1;
  }
  
  spinner::ref().spin();
  
  me.end();
  
  return 0;
}