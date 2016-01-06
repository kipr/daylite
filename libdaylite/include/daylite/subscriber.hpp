#ifndef _DAYLIGHT_SUBSCRIBER_HPP_
#define _DAYLIGHT_SUBSCRIBER_HPP_

#include <cstdint>

#include <functional>

#include "daylite/compat.hpp"
#include "daylite/bson.hpp"

namespace daylite
{
  class DLL_EXPORT subscriber
  {
  public:
    typedef std::function<void (const bson &, void *)> subscriber_callback_t;
    
    virtual void set_send_packed(const bool packed) = 0;
    virtual bool send_packed() const = 0;
  };
}

#endif
