#include "node_impl.hpp"
#include "console.hpp"
#include "tcp_server.hpp"
#include "tcp_transport.hpp"
#include "remote_node.hpp"
#include "mailman.hpp"
#include "network_interfaces.hpp"
#include <boost/uuid/random_generator.hpp>
#ifdef WIN32
extern "C" int gettimeofday(struct timeval *tp, struct timezone *tzp);
extern "C" void timersub(struct timeval *a, struct timeval *b, struct timeval *res);
extern "C" void timeradd(struct timeval *a, struct timeval *b, struct timeval *res);
#else
#include <sys/time.h>
#endif

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

static inline bool operator >(const network_time &l, const network_time &r)
{
  return l.seconds > r.seconds || (l.seconds == r.seconds && l.microseconds > r.microseconds);
}

node_impl::node_impl(const string &name, const option<socket_address> &us)
  : _name(name)
  , _dave(make_shared<mailman>(mailman()))
  , _mailbox(new mailbox(topic::internal, [this] (const packet &p) {
      node_info info;
      update_time();
      if(p.stamp() > _network_time)
      {
        _network_time = p.stamp();
      }

      try
      {
        info = node_info::unbind(p.msg());
      }
      catch(const std::invalid_argument &e)
      {
        std::cerr << "Failed to unbind node info. Something is probably very wrong. (" << e.what() << ")" << std::endl;
      }

      if(info.id == id()) return success();

      auto lit = _latest_info.find(info.id);
      if(lit != _latest_info.end())
      {
        for(const auto &topic : _latest_info[info.id].in_topics) --_subscription_count[topic];
      }

      _latest_info[info.id] = info;
      for(const auto &topic : _latest_info[info.id].in_topics)
      {
        ++_subscription_count[topic];
      }
      

      for(auto it = _latest_info.begin(); it != _latest_info.end(); ++it)
      {
        /*cout << it->first << ":" << endl;
        cout << "  pubs:" << endl;
        for(auto p : info.out_topics) cout << "    " << p << endl;
        cout << "  subs:" << endl;
        for(auto p : info.in_topics) cout << "    " << p << endl;*/
      }
      return success();
    }))
  , _id(std::hash<std::string>()(_name))
{
  gettimeofday(&_last_time, 0);
  gettimeofday(&_last_prune, 0);
  srand(_last_time.tv_sec * 1000000 + _last_time.tv_usec);
  _id ^= rand();
  _network_time.seconds = 0;
  _network_time.microseconds = 0;
  _dave->register_mailbox(_mailbox);
  _keepalive.seconds = 3600;
  _keepalive.microseconds = 0;
}

node_impl::~node_impl()
{
  _dave->unregister_mailbox(_mailbox);
  leave_daylite();
  stop_gateway_service();
}


void_result node_impl::start(const std::string &known_host, const uint16_t known_port)
{
  gateway("", 0);
  return join_daylite(known_host, known_port, false);
}

void_result node_impl::gateway(const std::string &host, const uint16_t port)
{
  return start_gateway_service(host, port);
}

void_result node_impl::stop()
{
  stop_gateway_service();
  leave_daylite();
  return success();
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

void_result node_impl::join_daylite(const std::string &host, uint16_t port, bool peer)
{
  socket_address gateway(host, port);
  void_result ret;

  auto gateway_transport = make_unique<tcp_transport>(gateway);
  if(!(ret = gateway_transport->open())) return ret;

  auto gateway_node = make_shared<remote_node>(this, move(gateway_transport), peer ? remote_node::block_internal : remote_node::allow_internal);
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
  ret.id = _id;
  ret.name = _name;
  ret.hosts = network_interfaces::interfaces();
  if(_server && _server->socket().port().some())
  {
    for(auto &host : ret.hosts) host.port = bson_bind::some(_server->socket().port().unwrap());
  }
  ret.maxkeepalive = _keepalive;
  ret.keepalive = ret.maxkeepalive;
  for(auto s : _active_subscribers) ret.in_topics.push_back(s->topic().name());
  for(auto s : _active_publishers) ret.out_topics.push_back(s->topic().name());
  ret.alive = true;
  return ret;
}

bool node_impl::touch_node(uint32_t id)
{
  auto it = _latest_info.find(id);
  if(it == _latest_info.end()) return false;
  it->second.keepalive = it->second.maxkeepalive;
  //std::cout << "new keepalive for " << id << " : " << it->second.keepalive.seconds + it->second.keepalive.microseconds / 1000000.0 << std::endl;
  return true;
}

void node_impl::prune_nodes()
{
  timeval now;
  gettimeofday(&now, nullptr);
  
  timeval diff;
  timersub(&now, &_last_prune, &diff);

  // std::cout << "time since last prune: " << diff.tv_sec << std::endl;
  for(auto it = _latest_info.begin(); it != _latest_info.end();)
  {
    auto &ka = it->second.keepalive;
    timeval k;
    k.tv_sec = ka.seconds;
    k.tv_usec = ka.microseconds;

    if(timercmp(&diff, &k, >))
    {
      // Prune
      // std::cout << "Pruning " << it->first << std::endl;
      it = _latest_info.erase(it);
      continue;
    }
    
    timeval ans;
    timersub(&k, &diff, &ans);
    ka.seconds = ans.tv_sec;
    ka.microseconds = ans.tv_usec;
    // std::cout << "Node " << it->first << " now has " << ka.seconds << " remaining" << std::endl;

    ++it;
  }

  _last_prune = now;
}

void node_impl::send_info() 
{
  update_time();
  packet p(topic::internal, _network_time, info().bind());
  _mailbox->place_outgoing_mail(p);
  prune_nodes();
  for(auto peer : _latest_info)
  {
    packet p(topic::internal, _network_time, peer.second.bind());
    _mailbox->place_outgoing_mail(p);
  }
}

void node_impl::update_peer_link(const node_info &info)
{
  // First try loopback
}

void node_impl::update_time()
{
  timeval now;
  gettimeofday(&now, nullptr);
  
  timeval diff;
  timersub(&now, &_last_time, &diff);

  timeval net;
  net.tv_sec = _network_time.seconds;
  net.tv_usec = _network_time.microseconds;

  timeval ans;
  timeradd(&net, &diff, &ans);

  _network_time.microseconds = ans.tv_usec;
  _network_time.seconds = ans.tv_sec;

  _last_time = now;
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

  auto remote = make_shared<remote_node>(this, move(transport));
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
    for(uint32_t id : (*it)->associated_ids())
    {
      auto lit = _latest_info.find(id);
      if(lit == _latest_info.end()) continue;
      lit->second.alive = false;
      lit->second.out_topics.clear();
      lit->second.in_topics.clear();
    }
    it = _remotes.erase(it);
  }
  send_info();
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
