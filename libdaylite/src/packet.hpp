#ifndef _DAYLIGHT_PACKET_HPP_
#define _DAYLIGHT_PACKET_HPP_

#include <cstdint>

#include <bson.h>

#include "topic.hpp"
#include "meta.hpp"

namespace daylite
{
  class packet
  {
  public:
    packet(const bson_t *packed_msg = nullptr);
    packet(const class topic &t, const network_time &stamp, const bson_t *raw_msg = nullptr);

    packet(const packet &rhs);
    packet(packet &&rhs);
    ~packet();
    
    inline const bson_t *msg() const { return _msg; }
    inline const bson_t *packed() const { return _packed; }
    inline struct meta &meta() { return _meta; }
    inline const struct meta &meta() const { return _meta; }
    inline class topic topic() const { return daylite::topic(_meta.topic); }
    inline const struct network_time &stamp() const { return _meta.stamp; }
    
    packet &operator =(packet &&rhs);
    packet &operator =(const packet &rhs);
    
  private:
    struct meta _meta;
    bson_t *_msg;
    bson_t *_packed;
  };
}

#endif
