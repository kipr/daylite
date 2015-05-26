#include "daylite/node.hpp"
#include "daylite/tcp_server.hpp"
#include "daylite/tcp_transport.hpp"
#include "daylite/remote_node.hpp"

#warning remove
#include <iostream>

using namespace daylite;

node::node(const std::string &name)
  : _name(name)
  , _server(0)
{
}

node::~node()
{
  _dave.remove_recv(this);
  end();
}

void_result node::start(const option<socket_address> &master, const option<socket_address> &us)
{
  _server = new tcp_server(us.or_else(socket_address()));
  void_result ret;
  
  if(!(ret = _server->open()))
  {
    delete _server;
    _server = 0;
    return ret;
  }
  
  if(master.none()) return success();
  
  tcp_transport *const master_transport = new tcp_transport(master.unwrap());
  if(!(ret = master_transport->open()))
  {
    delete _server;
    _server = 0;
    delete master_transport;
    return ret;
  }
  
  _remotes.push_back(new remote_node(master_transport, &_dave));
  
  return success();
}

void_result node::end()
{
  if(!_server) return failure("Node not started");
  
  _server->close();
  delete _server;
  _server = 0;
  
  for(auto remote : _remotes)
  {
    delete remote;
  }
  
  _remotes.clear();
  
  return success();
}

void_result node::send(const topic &path, const packet &p)
{
  return _dave.send(path, p);
}

void_result node::recv(const topic &path, const packet &p)
{
  using namespace std;
  cout << path.name() << ": " << reinterpret_cast<const char *>(p.data()) << endl;
  
  return success();
}

void node::server_connection(tcp_socket *const socket)
{
  _remotes.push_back(new remote_node(new tcp_transport(socket), &_dave));
}

void node::server_disconnection(tcp_socket *const socket)
{
  for(auto it = _remotes.begin(); it != _remotes.end();)
  {
    if((*it)->link()->socket() != socket) { ++it; continue; }
    delete *it;
    it = _remotes.erase(it);
  }
}

