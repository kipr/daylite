#ifndef _DAYLIGHT_PUBLISHER_HPP_
#define _DAYLIGHT_PUBLISHER_HPP_

#include <cstdint>

#include <bson.h>

#include "daylite/compat.hpp"

namespace daylite
{
  struct DLL_EXPORT publisher
  {
    virtual bool publish(const bson_t *msg) = 0;
  };
}

#endif