#ifndef _DAYLIGHT_SUBSCRIBER_HPP_
#define _DAYLIGHT_SUBSCRIBER_HPP_

#include <cstdint>

#include "daylite/compat.hpp"

namespace daylite
{
  class DLL_EXPORT subscriber
  {
  public:
    typedef void (*subscriber_callback_t)(const uint8_t *data, size_t size);
  };
}

#endif