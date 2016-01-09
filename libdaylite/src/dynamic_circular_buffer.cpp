#include "dynamic_circular_buffer.hpp"

#include <iostream>
#include <assert.h>

using namespace daylite;
using namespace std;

dynamic_circular_buffer::dynamic_circular_buffer(const size_t initial_capacity)
  : _head(0U)
  , _virtual_size(0U)
  , _physical_size(initial_capacity)
  , _backing(new uint8_t[_physical_size])
    
{
}

dynamic_circular_buffer::~dynamic_circular_buffer()
{
  delete[] _backing;
}

void dynamic_circular_buffer::write(const uint8_t *const data, const size_t count)
{
  if(count + _virtual_size > _physical_size) resize_backing((count + _virtual_size) << 1);
  
  const size_t insertion_point = (_head + _virtual_size) % _physical_size;
  
  if(insertion_point + count <= _physical_size)
  {
    // Case 1: The buffer insertion is contiguous. Straight copy.
    memcpy(_backing + insertion_point, data, count);
  }
  else
  {
    // Case 2: The buffer wraps around. Perform copies of both sections.
    const size_t first_chunk_size = _physical_size - insertion_point;
    const size_t second_chunk_size = count - first_chunk_size;
    assert(first_chunk_size + second_chunk_size == count);
    memcpy(_backing + insertion_point, data, first_chunk_size);
    memcpy(_backing, data + first_chunk_size, second_chunk_size);
  }
  
  _virtual_size += count;
}

size_t dynamic_circular_buffer::read(uint8_t *const data, const size_t max_count)
{
  const size_t count = max_count > _virtual_size ? _virtual_size : max_count;
  if(_head + count <= _physical_size)
  {
    // Case 1: The buffer is contiguous. Straight copy.
    memcpy(data, _backing + _head, count);
  }
  else
  {
    // Case 2: The buffer wraps around. Perform copies of both sections.
    const size_t first_chunk_size = _physical_size -  _head;
    const size_t second_chunk_size = count - first_chunk_size;
    assert(first_chunk_size + second_chunk_size == count);
    memcpy(data, _backing + _head, first_chunk_size);
    memcpy(data + first_chunk_size, _backing, second_chunk_size);
  }
  
  _head = (_head + count) % _physical_size;
  _virtual_size -= count;

  return count;
}

size_t dynamic_circular_buffer::discard(const size_t max_count)
{
  const size_t count = max_count > _virtual_size ? _virtual_size : max_count;
  _head = (_head + count) % _physical_size;
  _virtual_size -= count;
  return count;
}

void dynamic_circular_buffer::clear()
{
  _head = 0U;
  _virtual_size = 0U;
}

void dynamic_circular_buffer::resize_backing(const uint32_t new_size)
{
  if(new_size > 1000000U)
  {
    cout << "WARNING: Dynamic circular buffer has grown to " << new_size << endl;
  }
  
  uint8_t *const new_backing = new uint8_t[new_size];
  
  const size_t old_virtual_size = _virtual_size;
  read(new_backing, _virtual_size);
  
  // This operation moves the data to the start of the buffer
  _head = 0U;
  _physical_size = new_size;
  _virtual_size = old_virtual_size;
  
  delete[] _backing;
  _backing = new_backing;
}
