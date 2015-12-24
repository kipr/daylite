#ifndef _DAYLITE_NODE_HPP_
#define _DAYLITE_NODE_HPP_

#include "daylite/compat.hpp"
#include "daylite/publisher.hpp"
#include "daylite/subscriber.hpp"
#include "daylite/result.hpp"

#include <memory>

namespace daylite
{
  struct DLL_EXPORT node
  {
    static std::shared_ptr<node> create_node(const std::string &name);

    virtual void_result start(const std::string &known_host, uint16_t known_port) = 0;
    virtual void_result gateway(const std::string &host, const uint16_t port) = 0;
    virtual void_result stop() = 0;

    virtual const std::string &get_name() const = 0;

    virtual std::shared_ptr<publisher> advertise(const std::string &topic) = 0;
    virtual std::shared_ptr<subscriber> subscribe(const std::string &topic, subscriber::subscriber_callback_t callback,
      void *usr_arg = nullptr) = 0;
  };
}

#endif
