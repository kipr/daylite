#ifndef _DAYLIGHT_PACKET_HPP_
#define _DAYLIGHT_PACKET_HPP_

#include <cstdint>

#include <daylite/bson.hpp>

#include "topic.hpp"
#include "meta.hpp"

namespace daylite
{
  class packet
  {
  public:
    packet(const bson &packed_msg = bson());
    packet(const class topic &t, const network_time &stamp, const bson &raw_msg = bson());

    packet(const packet &rhs);
    packet(packet &&rhs);
    ~packet();
    
    inline const bson msg() const { return _msg; }
    inline const bson packed() const { return _packed; }
    inline struct meta &meta() { return _meta; }
    inline const struct meta &meta() const { return _meta; }
    inline class topic topic() const { return daylite::topic(_meta.topic); }
    inline const struct network_time &stamp() const { return _meta.stamp; }

    void build();
    
    packet &operator =(packet &&rhs);
    packet &operator =(const packet &rhs);
    
  private:
    struct meta _meta;
    bson _msg;
    bson _packed;
  };
}

#endif
