#ifndef _DAYLIGHT_PUBLISHER_IMPL_HPP_
#define _DAYLIGHT_PUBLISHER_IMPL_HPP_

#include <memory>

#include "daylite/publisher.hpp"

#include "topic.hpp"
#include "packet.hpp"
#include "result.hpp"
#include "mailbox.hpp"

namespace daylite
{
  class publisher_impl : public publisher
  {
  public:
    publisher_impl(std::shared_ptr<mailbox> mailbox);

    bool publish(const uint8_t *data, uint32_t size);
    void_result publish(std::unique_ptr<packet> packet);

  private:
    std::shared_ptr<mailbox> _mailbox;
  };
}

#endif