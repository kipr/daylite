#ifndef _DAYLITE_NODE_HPP_
#define _DAYLITE_NODE_HPP_

#include "daylite\compat.hpp"
#include "daylite\publisher.hpp"
#include "daylite\subscriber.hpp"

namespace daylite
{
  struct DLL_EXPORT node
  {
    static node *create_node(const char *name);
    static void destroy_node(node *node_ptr);

    virtual bool start_gateway_service(const char *local_host, uint16_t local_port) = 0;
    virtual bool stop_gateway_service() = 0;

    virtual bool join_daylite(const char *gateway_host, uint16_t gateway_port) = 0;
    virtual bool leave_daylite() = 0;

    virtual const char *get_name() const = 0;

    virtual publisher *advertise(const char *topic) = 0;
    virtual void destroy_publisher(publisher *pub) = 0;

    virtual subscriber *subscribe(const char *topic, subscriber::subscriber_callback_t callback) = 0;
    virtual void destroy_subscriber(subscriber *sub) = 0;
  };
}

#endif