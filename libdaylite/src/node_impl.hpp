#ifndef _DAYLITE_NODE_IMPL_HPP_
#define _DAYLITE_NODE_IMPL_HPP_

#include <functional>
#include <memory>
#include <vector>

#include "daylite/node.hpp"
#include "daylite/publisher.hpp"
#include "daylite/subscriber.hpp"

#include "topic.hpp"
#include "packet.hpp"
#include "publisher_impl.hpp"
#include "spinner_impl.hpp"
#include "subscriber_impl.hpp"
#include "tcp_server_listener.hpp"
#include "result.hpp"
#include "option.hpp"
#include "socket_address.hpp"

namespace daylite
{
  class remote_node;
  class mailman;
  class tcp_socket;
  class tcp_server;

  class node_impl : public node, private tcp_server_listener
  {
  public:

    node_impl(const std::string &name, const option<socket_address> &us);
    ~node_impl();

    bool start_gateway_service(const char *local_host, uint16_t local_port);
    bool stop_gateway_service();

    bool join_daylite(const char *gateway_host, uint16_t gateway_port);
    bool leave_daylite();

    const char *get_name() const { return _name.c_str(); }

    publisher *advertise(const char *topic);
    void destroy_publisher(publisher *pub);

    subscriber *subscribe(const char *topic, subscriber::subscriber_callback_t callback);
    void destroy_subscriber(subscriber *sub);

    result<std::shared_ptr<publisher>> advertise(const topic &topic);
    result<std::shared_ptr<subscriber>> subscribe(const topic &topic, subscriber::subscriber_callback_t cb);

  private:
    virtual void server_connection(tcp_socket *const socket);
    virtual void server_disconnection(tcp_socket *const socket);

    std::string _name;
    std::unique_ptr<mailman> _dave;

    std::unique_ptr<tcp_server> _server;
    std::vector<std::shared_ptr<remote_node>> _remotes;
  };
}

#endif