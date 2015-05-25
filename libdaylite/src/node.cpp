#include "daylite/node.hpp"
#include "daylite/tcp_server_transport.hpp"
#include "daylite/tcp_client_transport.hpp"

using namespace daylite;

node::node(const std::string &name)
  : _name(name)
  , _server(0)
{
  
}

node::~node()
{
  end();
}

void_result node::start(const option<socket_address> &master, const option<socket_address> &us)
{
  _server = new tcp_server_transport(us.or_else(socket_address()));
  void_result ret;
  
  if(!(ret = _server->open()))
  {
    delete _server;
    _server = 0;
    return ret;
  }
  
  if(master.none()) return success();
  
  transport *const client = new tcp_client_transport(master.unwrap());
  if(!(ret = client->open()))
  {
    delete _server;
    _server = 0;
    delete client;
    return ret;
  }
  
  _clients.push_back(client);
  
  return success();
}

void_result node::end()
{
  if(!_server) return failure("Node not started");
  
  _server->close();
  delete _server;
  _server = 0;
  
  for(auto client : _clients)
  {
    client->close();
    delete client;
  }
  
  _clients.clear();
  
  return success();
}

void_result node::spin_update()
{
  bool processed = false;
  for(;;)
  {
    for(auto c : _clients)
    {
      auto packet_result = c->input().unwrap()->read();
      if(!packet_result) break;
      processed = true;
      process_packet(packet_result.unwrap());
    }
    if(!processed) break;
  }
  
  return success();
}

void_result node::process_packet(const packet &p)
{
  if(!p.size()) return failure("Invalid packet size (0)");
  
  typedef void_result (node::*processor)(const packet &);
  
  static processor processors[] =
  {
    &node::add_remote_topic,
    &node::remove_remote_topic,
    &node::publication
  };
  
  const uint8_t type = p.data()[0];
  if(type >= sizeof(processors) / sizeof(processor)) return failure("Invalid type");
  
  return (this->*processors[type])(p);
}

void_result node::add_remote_topic(const packet &p)
{
  
  return success();
}

void_result node::remove_remote_topic(const packet &p)
{
  
  return success();
}

void_result node::publication(const packet &p)
{
  
  return success();
}
