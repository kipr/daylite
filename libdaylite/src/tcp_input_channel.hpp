#ifndef _DAYLITE_TCP_INPUT_CHANNEL_HPP_
#define _DAYLITE_TCP_INPUT_CHANNEL_HPP_

#include "input_channel.hpp"

#include <chrono>
#include <vector>

namespace daylite
{
  class tcp_socket;
  
  class tcp_input_channel : public input_channel
  {
  public:
    tcp_input_channel(tcp_socket *const socket);
    ~tcp_input_channel();
    
    inline void set_timeout(const std::chrono::milliseconds &timeout)
    {
      _timeout = timeout;
    }
    
    inline const std::chrono::milliseconds &timeout() const
    {
      return _timeout;
    }
    
    virtual result<packet> read();
    
  private:
    tcp_socket *_socket;
    std::chrono::milliseconds _timeout;
    std::vector<uint8_t> _buffer;
  };
}

#endif