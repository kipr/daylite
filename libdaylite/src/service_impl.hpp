#ifndef _DAYLITE_SERVICE_IMPL_HPP_
#define _DAYLITE_SERVICE_IMPL_HPP_

#include <functional>
#include <memory>

#include <bson.h>

#include "topic.hpp"
#include "daylite/subscriber.hpp"

#include "packet.hpp"
#include "daylite/result.hpp"
#include "mailbox.hpp"
#include "mailman.hpp"

namespace daylite
{
  class node_impl;

  class service_impl : public service
  {
  public:
    service_impl(node_impl *const parent, const daylite::topic &t,
      service::service_callback_t callback);

    inline class daylite::topic topic() const { return _t; }

  private:
    node_impl *_parent;
    daylite::topic _t;
    service::service_callback_t _callback;
    
    std::shared_ptr<publisher> _out;
    std::shared_ptr<subscriber> _in;
    
  };
}

#endif
