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

namespace daylite
{
  class subscriber_impl : public subscriber
  {
  public:
    subscriber_impl(std::shared_ptr<mailbox> mailbox, subscriber::subscriber_callback_t callback, void* usr_arg);

  private:
    std::shared_ptr<mailbox> _mailbox;
  };
}

#endif