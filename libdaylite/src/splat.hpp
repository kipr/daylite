#ifndef _DAYLITE_SPLAT_HPP_
#define _DAYLITE_SPLAT_HPP_

#include <string>
#include <stdint.h>
#include "daylite/result.hpp"
#include "packet.hpp"

#ifdef SPLAT_ENABLED

namespace daylite
{
  struct splat_data;
  
  class splat
  {
  public:
    splat(const uint32_t node_id, const std::string &topic);
    ~splat();
    
    inline uint32_t node_id() const { return _node_id; }
    inline std::string topic() const { return _topic; }
    inline size_t size() const { return _size; }
    
    void push(const packet &latest);
    void poll();
    
    inline bool is_on() const { return _mode != off; }
    inline bool is_owner() const { return _mode == pub; }
    
    bool update_available() const;
    inline const packet &latest() const { return _latest; }
    
    void_result create(const size_t max_size);
    void_result connect(const size_t defined_max_size);
    
    void_result close();
    
  private:
    void_result map(const size_t size, const int mode);
    void_result unmap();
    
    uint32_t _node_id;
    std::string _topic;
    int _fd;
    splat_data *_backing;
    size_t _size;
    
    uint32_t _current_version;
    packet _latest;
    
    enum
    {
      off, pub, sub
    } _mode;
    
  };
}

#endif

#endif