#ifndef _DAYLITE_TCP_INPUT_CHANNEL_HPP_
#define _DAYLITE_TCP_INPUT_CHANNEL_HPP_

#include "input_channel.hpp"

#include <vector>

namespace daylite
{
  class tcp_socket;
  
  class tcp_input_channel : public input_channel
  {
  public:
    tcp_input_channel(tcp_socket *const socket);
    ~tcp_input_channel();
    
    virtual result<packet> read();
    virtual bool is_available() const;
    
  private:
    tcp_socket *_socket;
    std::vector<uint8_t> _buffer;
  };
}

#endif