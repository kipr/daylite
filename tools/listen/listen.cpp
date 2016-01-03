#include <daylite/node.hpp>
#include <daylite/spinner.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>

using namespace daylite;
using namespace std;

namespace
{  
  void print_bson(const bson_t *const msg)
  {
    size_t len;
    char *str = bson_as_json (msg, &len);
    cout << str << endl;
    bson_free (str);
  }

  bool interrupt;

  void sig(int signo)
  {
    interrupt = true;
  }
}

int main(int argc, char *argv[])
{
  interrupt = false;

  // TODO: Make arguments
  string host = "127.0.0.1";
  uint16_t port = 8374;

  if(argc != 2)
  {
    cerr << argv[0] << " [topic]" << endl;
    return 1;
  }

  auto n = node::create_node("listen");
  if(!n)
  {
    cerr << "Failed to create daylite node. Something is very wrong." << endl;
    return 1;
  }

  if(!n->start(host, port))
  {
    cerr << "Failed to connect to daylite network on " << host << ":" << port << endl;
    return 1;
  }

  auto sub = n->subscribe(argv[1], [] (const bson &data, void *)
  {
    print_bson(data);  
  });

  if (signal(SIGINT, sig) == SIG_ERR)
  {
    cerr << "An error occurred while setting a signal handler." << endl;
    return 1;
  }

  cout << "Press Ctrl-C to stop listening" << endl;

  while(!interrupt)
  {
    this_thread::sleep_for(chrono::milliseconds(100));
    spinner::spin_once();
  }

  return 0;
}
