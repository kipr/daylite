#ifndef _DAYLITE_NODE_IMPL_HPP_
#define _DAYLITE_NODE_IMPL_HPP_

#include <functional>
#include <memory>
#include <vector>

#include <boost/uuid/uuid.hpp>

#include "daylite/node.hpp"
#include "daylite/publisher.hpp"
#include "daylite/subscriber.hpp"

#include "topic.hpp"
#include "packet.hpp"
#include "publisher_impl.hpp"
#include "service_impl.hpp"
#include "spinner_impl.hpp"
#include "subscriber_impl.hpp"
#include "tcp_server_listener.hpp"
#include "daylite/result.hpp"
#include "daylite/option.hpp"
#include "socket_address.hpp"
#include "node_info.hpp"
#include "tcp_thread.hpp"

namespace daylite
{
  class remote_node;
  class mailman;
  class tcp_socket;
  class tcp_server;

  class node_impl : public node, private tcp_server_listener, private spinett
  {
  public:
    friend class subscriber_impl;
    friend class publisher_impl;
    friend class remote_node;

    node_impl(const std::string &name, const option<socket_address> &us);
    ~node_impl();

    void_result start(const std::string &known_host, const uint16_t known_port);
    void_result gateway(const std::string &host, const uint16_t port);
    void_result stop();


    const std::string &get_name() const { return _name; }

    result<bson> call(const std::string &topic, const bson &value);
    std::shared_ptr<service> advertise_service(const std::string &topic, service::service_callback_t callback);
    std::shared_ptr<publisher> advertise(const std::string &topic);
    std::shared_ptr<subscriber> subscribe(const std::string &topic, subscriber::subscriber_callback_t callback, void *usr_arg);

    result<std::shared_ptr<subscriber>> advertise_service(const topic &topic, service::service_callback_t cb);
    result<std::shared_ptr<publisher>> advertise(const topic &topic);
    result<std::shared_ptr<subscriber>> subscribe(const topic &topic, subscriber::subscriber_callback_t cb);

    inline const std::unordered_map<decltype(node_info::id), node_info> &latest_info() const { return _latest_info; }

    inline uint32_t id() const
    {
      return _id;
    }

    inline const network_time &time() const { return _network_time; }
    void update_time();
    
    uint32_t thread_sleep_duration() const { return _thread.sleep_duration(); }

  private:
    void_result start_gateway_service(const std::string &local_host, uint16_t local_port);
    void_result stop_gateway_service();

    void_result join_daylite(const std::string &gateway_host, uint16_t gateway_port, bool peer);
    void_result leave_daylite();

    void register_subscriber(subscriber_impl *const subscriber);
    void register_publisher(publisher_impl *const publisher);
    void unregister_subscriber(subscriber_impl *const subscriber);
    void unregister_publisher(publisher_impl *const publisher);
    struct node_info info() const;
    bool touch_node(uint32_t id);
    void send_info(const bool include_dead);

    void update_peer_link(const node_info &info);

    virtual void server_connection(tcp_socket *const socket);
    virtual void server_disconnection(tcp_socket *const socket);

    virtual void_result spin_update();

    std::string _name;
    std::shared_ptr<mailman> _dave;
    std::shared_ptr<mailbox> _mailbox;

    std::unique_ptr<tcp_server> _server;
    std::vector<std::shared_ptr<remote_node>> _remotes;

    std::vector<subscriber_impl *> _active_subscribers;
    std::vector<publisher_impl *> _active_publishers;
    
    uint32_t _id;
    std::unordered_map<decltype(node_info::id), node_info> _latest_info;
    void tally(const node_info &info);
    void untally(const node_info &info);
    
    std::unordered_map<std::string, uint32_t> _subscription_count;
    std::unordered_map<std::string, uint32_t> _local_subscription_count;
    std::unordered_map<std::string, uint32_t> _splat_count;

    network_time _network_time;
    timeval _last_time;
    timeval _last_prune;

    network_time _keepalive;
    
    tcp_thread _thread;
    
#ifdef SPLAT_ENABLED
    void_result register_splat(const splat_info &info);
    void_result unregister_splat(const splat_info &info);
    void push_splat(const packet &p);
    void update_splats();
    bool is_only_splat(const std::string &topic);
    std::unordered_map<std::string, unique_ptr<splat>> _splat;
#endif
  };
}

#endif
