#ifndef _DAYLITE_SOCKET_ADDRESS_HPP_
#define _DAYLITE_SOCKET_ADDRESS_HPP_

#include <cstdint>
#include <string>
#include <sstream>

#include "socket_address.hpp"

namespace daylite
{
  class socket_address
  {
  public:
    inline socket_address(const std::string &host, const uint16_t port) : _host(host), _port(port) {}
    inline socket_address(const uint16_t port) : _host(""), _port(port) {}
    inline socket_address() : _host(""), _port(0) {}
    
    inline const std::string &host() const { return _host; }
    inline uint16_t port() const { return _port; }
    
    inline std::string to_string()
    {
      std::stringstream s;
      s << _host << ":" << _port;
      return s.str();
    }
    
  private:
    std::string _host;
    uint16_t _port;
  };
}

#endif