#ifndef _DAYLIGHT_PACKET_HPP_
#define _DAYLIGHT_PACKET_HPP_

#include <cstdint>

#include <bson.h>

#include "topic.hpp"

namespace daylite
{
  class packet
  {
  public:
    packet(const bson_t *packed_msg = nullptr);
    packet(const class topic &t, const bson_t *raw_msg = nullptr);

    packet(const packet &rhs);
    packet(packet &&rhs);
    ~packet();
    
    inline const bson_t *msg() const { return _msg; }
    inline const bson_t *packed() const { return _packed; }
    inline const class topic &topic() const { return _topic; }
    
    packet &operator =(packet &&rhs);
    packet &operator =(const packet &rhs);
    
  private:
    class topic _topic;
    bson_t *_msg;
    bson_t *_packed;
  };
}

#endif