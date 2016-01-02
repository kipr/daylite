#ifndef _DAYLIGHT_PUBLISHER_HPP_
#define _DAYLIGHT_PUBLISHER_HPP_

#include <cstdint>

#include "daylite/compat.hpp"
#include "daylite/result.hpp"
#include "daylite/bson.hpp"

namespace daylite
{
  struct DLL_EXPORT publisher
  {
    virtual unsigned subscriber_count() = 0;
    virtual bool firehose() const = 0;
    virtual void set_firehose(const bool firehose) = 0;
    virtual void_result publish(const bson &msg) = 0;
  };
}

#endif
