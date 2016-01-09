#ifndef _DAYLITE_DYNAMIC_CIRCULAR_BUFFER_HPP_
#define _DAYLITE_DYNAMIC_CIRCULAR_BUFFER_HPP_

#include <string.h>
#include <stdint.h>

namespace daylite
{
  /**
   * A dynamic circular buffer is the same concept as a circular
   * buffer except it can grow arbitarily large to avoid filling up.
   * This gives the best of a circular buffer and a vector.
   */
  class dynamic_circular_buffer
  {
  public:
    dynamic_circular_buffer(const size_t initial_capacity = 0xFFFF);
    ~dynamic_circular_buffer();
    
    void write(const uint8_t *const data, const size_t count);
    size_t read(uint8_t *const data, const size_t max_count);
    size_t discard(const size_t max_count);
    void clear();
    
    inline size_t physical_size() const { return _physical_size; }
    inline size_t size() const { return _virtual_size; }
    inline bool contiguous_read_possible(const size_t size) const
    {
      return size <= _virtual_size && _head + size <= _physical_size;
    }
    inline uint8_t *data() const { return _backing + _head; }
    
    const bool can_grow() const { return _physical_size < 1000000; }
    
  private:
    void resize_backing(const uint32_t new_size);
    
    size_t _head;
    size_t _virtual_size;
    size_t _physical_size;
    uint8_t *_backing;
  };
}

#endif