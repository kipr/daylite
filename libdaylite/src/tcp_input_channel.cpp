#include "tcp_input_channel.hpp"
#include "daylite/tcp_socket.hpp"

#include <cstring>
#include <cassert>
#include <errno.h>

using namespace daylite;
using namespace std::chrono;

namespace
{
  inline milliseconds elapsed(const steady_clock::time_point &start)
  {
    return duration_cast<milliseconds>(steady_clock::now() - start);
  }
}

tcp_input_channel::tcp_input_channel(tcp_socket *const socket)
  : _socket(socket)
  , _timeout(1000)
{
}

tcp_input_channel::~tcp_input_channel()
{
}

result<packet> tcp_input_channel::read()
{
  // Socket cannot be blocking. Hard fail (programmatic error).
  assert(!_socket->blocking().or_else(true));
  
  const auto start = steady_clock::now();
  
  uint8_t tmp[0xFFFF];
  
  // Read the size of the data to come (max 2^32 - 1) or until time is up
  while(_buffer.size() < sizeof(uint32_t) && elapsed(start) < _timeout)
  {
    result<size_t> ret = _socket->recv(tmp, sizeof(tmp));
    if(ret.code() == EAGAIN) continue;
    if(!ret) return failure<packet>(ret.message());
    _buffer.insert(_buffer.end(), tmp, tmp + ret.unwrap());
  }
  
  // Keep reading until time is up or we reach the requisite length
  uint32_t target_size = *reinterpret_cast<uint32_t *>(_buffer.data());
  while(_buffer.size() < target_size + sizeof(uint32_t) && elapsed(start) < _timeout)
  {
    result<size_t> ret = _socket->recv(tmp, sizeof(tmp));
    if(ret.code() == EAGAIN) continue;
    if(!ret) return failure<packet>(ret.message());
    _buffer.insert(_buffer.end(), tmp, tmp + ret.unwrap());
  }
  
  if(elapsed(start) >= _timeout) return failure<packet>("Ran out of time");
  
  // Got all of the packet. Chop it off of the buffer and return it.
  packet p(_buffer.data() + sizeof(uint32_t), target_size);
  _buffer.erase(_buffer.begin(), _buffer.begin() + target_size + sizeof(uint32_t));
  return success(p);
}
