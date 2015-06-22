#ifndef _DAYLIGHT_PUBLISHER_HPP_
#define _DAYLIGHT_PUBLISHER_HPP_

#include <cstdint>

#include "daylite/compat.hpp"

namespace daylite
{
  struct DLL_EXPORT publisher
  {
    virtual bool publish(const uint8_t *data, size_t size) = 0;
  };
}

#endif