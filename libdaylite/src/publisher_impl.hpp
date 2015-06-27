#ifndef _DAYLIGHT_PUBLISHER_IMPL_HPP_
#define _DAYLIGHT_PUBLISHER_IMPL_HPP_

#include <memory>

#include <bson.h>

#include "daylite/publisher.hpp"

#include "topic.hpp"
#include "packet.hpp"
#include "daylite/result.hpp"
#include "mailbox.hpp"

namespace daylite
{
  class publisher_impl : public publisher
  {
  public:
    publisher_impl(std::shared_ptr<mailbox> mailbox);

    void_result publish(const bson_t *msg);
    void_result publish(std::unique_ptr<packet> packet);

  private:
    std::shared_ptr<mailbox> _mailbox;
  };
}

#endif