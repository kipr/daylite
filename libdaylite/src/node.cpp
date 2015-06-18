#include "daylite/node.hpp"
#include "daylite/console.hpp"
#include "daylite/tcp_server.hpp"
#include "daylite/tcp_transport.hpp"
#include "daylite/remote_node.hpp"
#include "daylite/mailman.hpp"

using namespace daylite;

node::node(const std::string &name)
  : _name(name)
  , _dave(new mailman())
{
}

node::~node()
{
  _dave->remove_recv(this);
  end();
}

void_result node::start(const option<socket_address> &master, const option<socket_address> &us)
{
  _server.reset(new tcp_server(us.or_else(socket_address())));
  void_result ret;

  if(!(ret = _server->open()))
  {
    _server.release();
    return ret;
  }

  if(master.none()) return success();

  tcp_transport *const master_transport = new tcp_transport(master.unwrap());
  if(!(ret = master_transport->open()))
  {
    _server.release();
    delete master_transport;
    return ret;
  }
  auto master_node = new remote_node(std::unique_ptr<transport>(master_transport));
  master_node->register_recv((uint32_t)&_dave, [this](const packet& p) { _dave->recv(p); });
  _remotes.push_back(master_node);

  return success();
}

void_result node::end()
{
  if(!_server) return failure("Node not started");

  _server->close();
  _server.release();

  for(auto remote : _remotes)
  {
    delete remote;
  }

  _remotes.clear();

  return success();
}

void_result node::send(const topic &path, const packet &p)
{
  return _dave->send(path, p);
}

void_result node::recv(const topic &path, const packet &p)
{
  DAYLITE_DEBUG_STREAM(path.name() << ": " << reinterpret_cast<const char *>(p.data()));

  return success();
}

void node::server_connection(tcp_socket *const socket)
{
  auto remote = new remote_node(std::unique_ptr<transport>(new tcp_transport(socket)));
  remote->register_recv((uint32_t)&_dave, [this](const packet& p) { _dave->recv(p); });
  _remotes.push_back(remote);
}

void node::server_disconnection(tcp_socket *const socket)
{
  for(auto it = _remotes.begin(); it != _remotes.end();)
  {
    if(dynamic_cast<tcp_transport*>((*it)->link())->socket() != socket) { ++it; continue; }
    delete *it;
    it = _remotes.erase(it);
  }
}

