#ifndef _DAYLITE_SERVICE_HPP_
#define _DAYLITE_SERVICE_HPP_

#include <functional>
#include "bson.hpp"

namespace daylite
{
  class service
  {
  public:
    typedef std::function<bson (const bson &)> service_callback_t;
  };
}

#endif