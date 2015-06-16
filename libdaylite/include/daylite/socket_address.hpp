#ifndef _DAYLITE_SOCKET_ADDRESS_HPP_
#define _DAYLITE_SOCKET_ADDRESS_HPP_

#include <cstdint>
#include <string>

#include "daylite/compat.hpp"
#include "daylite/socket_address.hpp"

namespace daylite
{
  // DLL-Export STL templates (MS Article 168958)
  EXPIMP_TEMPLATE template class DLL_EXPORT std::basic_string<char>;

  class DLL_EXPORT socket_address
  {
  public:
    inline socket_address(const std::string &host, const uint16_t port) : _host(host), _port(port) {}
    inline socket_address(const uint16_t port) : _host(""), _port(port) {}
    inline socket_address() : _host(""), _port(0) {}
    
    inline const std::string &host() const { return _host; }
    inline uint16_t port() const { return _port; }
    
  private:
    std::string _host;
    uint16_t _port;
  };
}

#endif