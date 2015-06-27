#ifndef _DAYLITE_OUTPUT_CHANNEL_HPP_
#define _DAYLITE_OUTPUT_CHANNEL_HPP_

#include <memory>

#include "packet.hpp"
#include "daylite/result.hpp"

namespace daylite
{
  class output_channel
  {
  public:
    virtual ~output_channel() {}
    
    virtual void_result write(const packet &package) = 0;
  };
}

#endif