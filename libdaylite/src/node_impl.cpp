#include "node_impl.hpp"
#include "console.hpp"
#include "tcp_server.hpp"
#include "tcp_transport.hpp"
#include "remote_node.hpp"
#include "mailman.hpp"
#include "network_interfaces.hpp"
#include <boost/uuid/random_generator.hpp>

#include <iostream>

using namespace daylite;
using namespace std;

shared_ptr<node> node::create_node(const std::string &name)
{
  return shared_ptr<node>(new node_impl(name, none<socket_address>()));
}

void print_bson(const bson_t *const msg)
{
  size_t len;
  char *str;
  str = bson_as_json (msg, &len);
  cout << str << endl;
  bson_free (str);
}

node_impl::node_impl(const string &name, const option<socket_address> &us)
  : _name(name)
  , _dave(make_shared<mailman>(mailman()))
  , _mailbox(new mailbox(topic::internal, [this] (std::shared_ptr<packet> p) {
      node_info info;
      print_bson(p->msg());
      try
      {
        info = node_info::unbind(p->msg());
      }
      catch(const std::invalid_argument &e)
      {
        std::cerr << "Failed to unbind node info. Something is probably very wrong. (" << e.what() << ")" << std::endl;
      }
      _latest_info[info.id] = info;
      for(auto it = _latest_info.begin(); it != _latest_info.end(); ++it)
      {
        cout << it->first << ":" << endl;
        cout << "  pubs:" << endl;
        for(auto p : info.out_topics) cout << "    " << p << endl;
        cout << "  subs:" << endl;
        for(auto p : info.in_topics) cout << "    " << p << endl;
      }
      return success();
    }))
{
  boost::uuids::basic_random_generator<boost::mt19937> gen;
  _id = gen();
  _dave->register_mailbox(_mailbox);
}

node_impl::~node_impl()
{
  _dave->unregister_mailbox(_mailbox);
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
  send_info();

  return success();
}

void_result node_impl::leave_daylite()
{
  _remotes.clear();

  return success();
}

shared_ptr<publisher> node_impl::advertise(const std::string &t)
{
  return shared_ptr<publisher>(new publisher_impl(this, topic(t), _dave));
}

shared_ptr<subscriber> node_impl::subscribe(const std::string &t, subscriber::subscriber_callback_t callback, void* usr_arg)
{
  return shared_ptr<subscriber>(new subscriber_impl(this, topic(t), _dave, callback, usr_arg));
}

void node_impl::register_subscriber(subscriber_impl *const subscriber)
{
  unregister_subscriber(subscriber);
  _active_subscribers.push_back(subscriber);
  send_info();
}

void node_impl::register_publisher(publisher_impl *const publisher)
{
  unregister_publisher(publisher);
  _active_publishers.push_back(publisher);
  send_info();
}

void node_impl::unregister_subscriber(subscriber_impl *const subscriber)
{
  for(auto it = _active_subscribers.begin(); it != _active_subscribers.end();)
  {
    if(*it == subscriber) it = _active_subscribers.erase(it);
    else ++it;
  }
}

void node_impl::unregister_publisher(publisher_impl *const publisher)
{
  for(auto it = _active_publishers.begin(); it != _active_publishers.end();)
  {
    if(*it == publisher) it = _active_publishers.erase(it);
    else ++it;
  }
}

struct node_info node_impl::info() const
{
  node_info ret;
  memcpy(&ret.id, _id.data, sizeof(ret.id));
  ret.hosts = network_interfaces::interfaces();
  for(auto s : _active_subscribers) ret.in_topics.push_back(s->topic().name());
  for(auto s : _active_publishers) ret.out_topics.push_back(s->topic().name());
  return ret;
}

void node_impl::send_info() 
{
  packet p(topic::internal, info().bind());
  _mailbox->place_outgoing_mail(make_unique<packet>(p));
}

void node_impl::server_connection(tcp_socket *const socket)
{
  auto transport = make_unique<tcp_transport>(socket, false);
  auto result = transport->open();
  if(!result)
  {
    DAYLITE_ERROR_STREAM("Could not open tcp transport");
    return;
  }

  auto remote = make_shared<remote_node>(move(transport));
  _dave->register_mailbox(remote);
  _remotes.push_back(remote);
  send_info();
}

void node_impl::server_disconnection(tcp_socket *const socket)
{
  for(auto it = _remotes.begin(); it != _remotes.end();)
  {
    if(dynamic_cast<tcp_transport *>((*it)->link())->socket() != socket) { ++it; continue; }
    _dave->unregister_mailbox(*it);
    it = _remotes.erase(it);
  }
}

void_result node_impl::spin_update()
{
  for(auto it = _remotes.begin(); it != _remotes.end();)
  {
    if(dynamic_cast<tcp_transport *>((*it)->link())->socket()) { ++it; continue; }
    _dave->unregister_mailbox(*it);
    it = _remotes.erase(it);
  }
  return success();
}
