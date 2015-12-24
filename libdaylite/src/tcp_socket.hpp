#ifndef _DAYLIGHT_TCP_SOCKET_HPP_
#define _DAYLIGHT_TCP_SOCKET_HPP_

#include "socket_address.hpp"
#include "daylite/result.hpp"
#include "daylite/option.hpp"

#include <cstdint>
#include <cstring>
#include <tuple>

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
    
    inline const option<socket_address> &associated_address() const { return _associated_address; }
    
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
    
    bool is_open() const;
    
  private:
    tcp_socket(int fd, const option<socket_address> &assoc = none<socket_address>());
    
    void_result setup_socket();
    
    int _fd;
    option<socket_address> _associated_address;

#ifdef WIN32
    bool _blocking;
#endif
  };
}

#endif
