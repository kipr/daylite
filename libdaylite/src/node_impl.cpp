#include "node_impl.hpp"
#include "console.hpp"
#include "tcp_server.hpp"
#include "tcp_transport.hpp"
#include "remote_node.hpp"
#include "mailman.hpp"

using namespace daylite;
using namespace std;

node *node::create_node(const std::string &name)
{
  return new node_impl(name, none<socket_address>());
}

void node::destroy_node(node *node_ptr)
{
  delete node_ptr;
}

node_impl::node_impl(const string &name, const option<socket_address> &us)
  : _name(name)
  , _dave(make_unique<mailman>(mailman()))
{
}

node_impl::~node_impl()
{
  leave_daylite();
  stop_gateway_service();
}

void_result node_impl::start_gateway_service(const std::string &local_host, uint16_t local_port)
{
  void_result ret;

  _server = make_unique<tcp_server>(socket_address(local_host, local_port));
  if(!(ret = _server->open()))
  {
    _server.release();
    return ret;
  }

  ret = _server->add_tcp_server_listener(this);
  if(!ret)
  {
    stop_gateway_service();
    return ret;
  }

  return success();
}

void_result node_impl::stop_gateway_service()
{
  if(!_server) return failure("Node not started");

  _server->remove_tcp_server_listener(this);

  _server->close();
  _server.release();

  return success();
}

void_result node_impl::join_daylite(const std::string &gateway_host, uint16_t gateway_port)
{
  socket_address gateway(gateway_host, gateway_port);
  void_result ret;

  auto gateway_transport = make_unique<tcp_transport>(gateway);
  if(!(ret = gateway_transport->open())) return ret;

  auto gateway_node = make_shared<remote_node>(move(gateway_transport));
  _dave->register_mailbox(gateway_node);
  _remotes.push_back(gateway_node);

  return success();
}

void_result node_impl::leave_daylite()
{
  _remotes.clear();

  return success();
}

publisher *node_impl::advertise(const std::string &t)
{
  auto mb = std::make_shared<mailbox>(topic(t));
  auto pub = new publisher_impl(mb);
  auto ret = _dave->register_mailbox(mb);
  return ret ? pub : nullptr;
}

void node_impl::destroy_publisher(publisher *pub)
{
  delete pub;
}

subscriber *node_impl::subscribe(const std::string &t, subscriber::subscriber_callback_t callback, void* usr_arg)
{
  auto mb = std::make_shared<mailbox>(topic(t));
  auto sub = new subscriber_impl(mb, callback, usr_arg);
  auto ret = _dave->register_mailbox(mb);
  return ret ? sub : nullptr;
}

void node_impl::destroy_subscriber(subscriber *sub)
{
  delete sub;
}

void node_impl::server_connection(tcp_socket *const socket)
{
  auto transport = make_unique<tcp_transport>(socket);
  auto result = transport->open();
  if(!result)
  {
    DAYLITE_ERROR_STREAM("Could not open tcp transport");
    return;
  }

  auto remote = make_shared<remote_node>(move(transport));
  _dave->register_mailbox(remote);
  _remotes.push_back(remote);
}

void node_impl::server_disconnection(tcp_socket *const socket)
{
  for(auto it = _remotes.begin(); it != _remotes.end();)
  {
    if(dynamic_cast<tcp_transport *>((*it)->link())->socket() != socket) { ++it; continue; }
    it = _remotes.erase(it);
  }
}
