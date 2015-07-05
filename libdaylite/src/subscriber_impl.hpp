#ifndef _DAYLIGHT_SUBSCRIBER_IMPL_HPP_
#define _DAYLIGHT_SUBSCRIBER_IMPL_HPP_

#include <functional>
#include <memory>

#include <bson.h>

#include "daylite/subscriber.hpp"

#include "topic.hpp"
#include "packet.hpp"
#include "daylite/result.hpp"
#include "mailbox.hpp"
#include "mailman.hpp"

namespace daylite
{
  class subscriber_impl : public subscriber
  {
  public:
    subscriber_impl(const topic &t, const std::shared_ptr<mailman> &dave,
      subscriber::subscriber_callback_t callback, void* usr_arg);
    ~subscriber_impl();

  private:
    topic _t;
    std::shared_ptr<mailman> _dave;
    std::shared_ptr<mailbox> _mailbox;
  };
}

#endif