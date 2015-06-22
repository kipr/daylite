#include "packet.hpp"
#include <cstring>

using namespace daylite;

packet::packet(topic t, const char *const str)
  : _topic(std::move(t))
  , _data(0)
  , _size(strlen(str) + 1)
{
  if(!_size) return;
  
  _data = new uint8_t[_size];
  memcpy(_data, str, _size);
}

packet::packet(topic t, const uint8_t *const data, const uint32_t size)
  : _topic(std::move(t))
  , _data(0)
  , _size(size)
{
  if(!_size) return;
  
  _data = new uint8_t[_size];
  memcpy(_data, data, _size);
}

packet::packet(const packet &rhs)
  : _topic(rhs._topic)
  , _data(0)
  , _size(rhs._size)
{
  if(!_size) return;
  
  _data = new uint8_t[_size];
  memcpy(_data, rhs._data, _size);
}

packet::packet(packet &&rhs)
  : _topic(std::move(rhs._topic))
  , _data(rhs._data)
  , _size(rhs._size)
{
  rhs._data = 0;
  rhs._size = 0;
}

packet::~packet()
{
  delete[] _data;
}

packet &packet::operator =(packet &&rhs)
{
  delete[] _data;
  
  _topic = std::move(rhs._topic);
  _data = rhs._data;
  _size = rhs._size;
  
  rhs._data = 0;
  rhs._size = 0;
  return *this;
}

packet &packet::operator =(const packet &rhs)
{
  delete[] _data;
  _topic = rhs._topic;
  _data = 0;
  _size = rhs._size;
  if(!_size) return *this;
  
  _data = new uint8_t[_size];
  memcpy(_data, rhs._data, _size);
  
  return *this;
}
