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
#include "spinner_impl.hpp"
#include "subscriber_impl.hpp"
#include "tcp_server_listener.hpp"
#include "daylite/result.hpp"
#include "daylite/option.hpp"
#include "socket_address.hpp"

namespace daylite
{
  class remote_node;
  class mailman;
  class tcp_socket;
  class tcp_server;

  class node_impl : public node, private tcp_server_listener, private spinett
  {
  public:

    node_impl(const std::string &name, const option<socket_address> &us);
    ~node_impl();

    void_result start_gateway_service(const std::string &local_host, uint16_t local_port);
    void_result stop_gateway_service();

    void_result join_daylite(const std::string &gateway_host, uint16_t gateway_port);
    void_result leave_daylite();

    const std::string &get_name() const { return _name; }

    std::shared_ptr<publisher> advertise(const std::string &topic);
    std::shared_ptr<subscriber> subscribe(const std::string &topic, subscriber::subscriber_callback_t callback, void *usr_arg);

    result<std::shared_ptr<publisher>> advertise(const topic &topic);
    result<std::shared_ptr<subscriber>> subscribe(const topic &topic, subscriber::subscriber_callback_t cb);

  private:
    virtual void server_connection(tcp_socket *const socket);
    virtual void server_disconnection(tcp_socket *const socket);

    virtual void_result spin_update();

    std::string _name;
    std::shared_ptr<mailman> _dave;

    std::unique_ptr<tcp_server> _server;
    std::vector<std::shared_ptr<remote_node>> _remotes;
    
    boost::uuids::uuid _id;
  };
}

#endif