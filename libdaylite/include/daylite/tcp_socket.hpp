#ifndef _DAYLIGHT_TCP_SOCKET_HPP_
#define _DAYLIGHT_TCP_SOCKET_HPP_

#include <cstdint>
#include <cstring>

#include "socket_address.hpp"
#include "result.hpp"
#include "option.hpp"

namespace daylite
{
  class tcp_socket
  {
  public:
    tcp_socket();
    ~tcp_socket();
    
    void_result open();
    void close();
    
    
    void_result bind(const socket_address &address);
    void_result connect(const socket_address &address);
    void_result listen(const uint32_t queue_size);
    result<tcp_socket *> accept();
    
    
    
    result<bool> blocking() const;
    void_result set_blocking(const bool blocking);
    
    enum comm_flags
    {
      comm_none = 0x00,
      comm_peek = 0x01
    };
    
    result<size_t> send(const uint8_t *const data, const size_t size, const comm_flags flags = comm_none);
    result<size_t> recv(uint8_t *const data, const size_t size, const comm_flags flags = comm_none);
    
    option<uint16_t> port() const;
    inline option<int> fd() const { return _fd < 0 ? none<int>() : some(_fd); }
    
  private:
    tcp_socket(int fd);
    int _fd;
  };
}

#endif