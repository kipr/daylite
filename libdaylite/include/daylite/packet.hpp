#ifndef _DAYLIGHT_PACKET_HPP_
#define _DAYLIGHT_PACKET_HPP_

#include <cstdint>

#include "compat.hpp"

namespace daylite
{
  class DLL_EXPORT packet
  {
  public:
    packet(const char *const str);
    packet(const uint8_t *const data, const uint32_t size);
    packet(const packet &rhs);
    packet(packet &&rhs);
    ~packet();
    
    inline const uint8_t *data() const { return _data; }
    inline const uint32_t size() const { return _size; }
    
    packet &operator =(packet &&rhs);
    packet &operator =(const packet &rhs);
    
  private:
    uint8_t *_data;
    uint32_t _size;
  };
}

#endif