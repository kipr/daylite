#ifndef _DAYLITE_NODE_HPP_
#define _DAYLITE_NODE_HPP_

#include "daylite/compat.hpp"
#include "daylite/publisher.hpp"
#include "daylite/subscriber.hpp"
#include "daylite/result.hpp"

namespace daylite
{
  struct DLL_EXPORT node
  {
    static node *create_node(const std::string &name);
    static void destroy_node(node *node_ptr);

    virtual void_result start_gateway_service(const std::string &local_host, uint16_t local_port) = 0;
    virtual void_result stop_gateway_service() = 0;

    virtual void_result join_daylite(const std::string &gateway_host, uint16_t gateway_port) = 0;
    virtual void_result leave_daylite() = 0;

    virtual const std::string &get_name() const = 0;

    virtual publisher *advertise(const std::string &topic) = 0;
    virtual void destroy_publisher(publisher *pub) = 0;

    virtual subscriber *subscribe(const std::string &topic, subscriber::subscriber_callback_t callback, void *usr_arg = nullptr) = 0;
    virtual void destroy_subscriber(subscriber *sub) = 0;
  };
}

#endif
