#ifndef _DAYLIGHT_PUBLISHER_HPP_
#define _DAYLIGHT_PUBLISHER_HPP_

#include <cstdint>

#include <bson.h>

#include "daylite/compat.hpp"
#include "daylite/result.hpp"

namespace daylite
{
  struct DLL_EXPORT publisher
  {
    virtual void_result publish(const bson_t *msg) = 0;
  };
}

#endif