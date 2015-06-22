#ifndef _DAYLIGHT_PACKET_HPP_
#define _DAYLIGHT_PACKET_HPP_

#include <cstdint>

#include "topic.hpp"

namespace daylite
{
  class packet
  {
  public:
    packet(topic t, const char *const str = "");
    packet(topic t, const uint8_t *const data, const uint32_t size);
    packet(const packet &rhs);
    packet(packet &&rhs);
    ~packet();
    
    inline const uint8_t *data() const { return _data; }
    inline const uint32_t size() const { return _size; }
    inline topic get_topic() const { return _topic; }
    
    packet &operator =(packet &&rhs);
    packet &operator =(const packet &rhs);
    
  private:
    uint8_t *_data;
    uint32_t _size;
    topic _topic;
  };
}

#endif