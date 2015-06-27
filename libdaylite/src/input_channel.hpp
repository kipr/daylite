#ifndef _DAYLITE_INPUT_CHANNEL_HPP_
#define _DAYLITE_INPUT_CHANNEL_HPP_

#include "daylite/result.hpp"
#include "packet.hpp"

namespace daylite
{
  class input_channel
  {
  public:
    virtual ~input_channel() {}
    
    virtual result<packet> read() = 0;
  };
}

#endif