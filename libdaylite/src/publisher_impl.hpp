#ifndef _DAYLIGHT_PUBLISHER_IMPL_HPP_
#define _DAYLIGHT_PUBLISHER_IMPL_HPP_

#include <memory>

#include <bson.h>

#include "daylite/publisher.hpp"

#include "topic.hpp"
#include "packet.hpp"
#include "daylite/result.hpp"
#include "mailbox.hpp"
#include "mailman.hpp"

namespace daylite
{
  class publisher_impl : public publisher
  {
  public:
    publisher_impl(const topic &t, const std::shared_ptr<mailman> &dave);
    ~publisher_impl();

    void_result publish(const bson_t *msg);
    void_result publish(std::unique_ptr<packet> packet);

  private:
    topic _t;
    std::shared_ptr<mailman> _dave;
    std::shared_ptr<mailbox> _mailbox;
  };
}

#endif