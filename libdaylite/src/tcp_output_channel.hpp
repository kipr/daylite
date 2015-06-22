#ifndef _DAYLITE_TCP_OUTPUT_CHANNEL_HPP_
#define _DAYLITE_TCP_OUTPUT_CHANNEL_HPP_

#include <memory>

#include "output_channel.hpp"

namespace daylite
{
  class tcp_socket;
  
  class tcp_output_channel : public output_channel
  {
  public:
    tcp_output_channel(tcp_socket *const socket);
    ~tcp_output_channel();
    
    virtual void_result write(const packet &package);
    
  private:
    tcp_socket *_socket;
  };
}

#endif