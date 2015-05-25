#ifndef _DAYLITE_BROADCAST_OUTPUT_CHANNEL_HPP_
#define _DAYLITE_BROADCAST_OUTPUT_CHANNEL_HPP_

#include "daylite/output_channel.hpp"
#include <vector>
#include <tuple>

namespace daylite
{
  class broadcast_output_channel : public output_channel
  {
  public:
    broadcast_output_channel();
    virtual ~broadcast_output_channel();
    
    void_result add_output_channel(output_channel *const channel, const void *const assoc);
    void_result remove_output_channel(const void *const assoc);
    bool contains_output_channel(const void *const assoc);
    
    virtual void_result write(const packet &p);
    
  private:
    std::vector<std::tuple<output_channel *, const void *>> _channels;
  };
}

#endif