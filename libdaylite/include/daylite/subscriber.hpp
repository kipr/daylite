#ifndef _DAYLIGHT_SUBSCRIBER_HPP_
#define _DAYLIGHT_SUBSCRIBER_HPP_

#include <cstdint>

#include <bson.h>

#include <functional>

#include "daylite/compat.hpp"

namespace daylite
{
  class DLL_EXPORT subscriber
  {
  public:
    typedef std::function<void (const bson_t *, void *)> subscriber_callback_t;
  };
}

#endif