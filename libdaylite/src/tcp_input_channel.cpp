#include "console.hpp"
#include "tcp_input_channel.hpp"
#include "tcp_socket.hpp"
#include "assert.hpp"

#include <cstring>
#include <errno.h>

using namespace daylite;

tcp_input_channel::tcp_input_channel(tcp_socket *const socket)
  : _socket(socket)
{
}

tcp_input_channel::~tcp_input_channel()
{
}

result<packet> tcp_input_channel::read()
{
  // Socket cannot be blocking. Hard fail (programmatic error).
  assert_result_eq(_socket->blocking(), false);

  uint8_t tmp[0xFFFF];

  // Read the size of the data to come (max 2^32 - 1)
  if(_buffer.size() < sizeof(uint32_t))
  {
    if(!_buffer.can_grow()) return failure<packet>("Backlog too large", EAGAIN);
    result<size_t> ret = _socket->recv(tmp, sizeof(tmp));
    if(!ret) return failure<packet>(ret.message(), ret.code());
    const uint32_t size = ret.unwrap();
    _buffer.write(tmp, size);
  }

  // got something but not all
  if(_buffer.size() < sizeof(uint32_t)) return failure<packet>("Ran out of time", EAGAIN);

  // Keep reading until we reach the requisite length
  uint32_t target_size = *reinterpret_cast<uint32_t *>(_buffer.data());
  while(_buffer.size() < target_size)
  {
    if(!_buffer.can_grow()) return failure<packet>("Backlog too large", EAGAIN);
    result<size_t> ret = _socket->recv(tmp, sizeof(tmp));
    if(ret.code() == EAGAIN) break;
    if(!ret) return failure<packet>(ret.message(), ret.code());
    const uint32_t size = ret.unwrap();
    _buffer.write(tmp, size);
  }

  if(_buffer.size() < target_size) return failure<packet>("Ran out of time", EAGAIN);

  // Got all of the packet. Chop it off of the buffer and return it.
  bson_reader_t *reader = 0;
  uint8_t *t = 0;
  if(_buffer.contiguous_read_possible(target_size))
  {
    reader = bson_reader_new_from_data(_buffer.data(), target_size);
    _buffer.discard(target_size);
  }
  else
  {
    t = new uint8_t[target_size];
    _buffer.read(t, target_size);
    reader = bson_reader_new_from_data(t, target_size);
  }
  
  if(!reader)
  {
    bson_reader_destroy(reader);
    return failure<packet>("Could not create the BSON reader");
  }
  const bson_t *const b = bson_reader_read(reader, nullptr);
  packet p;
  if(b) p = packet(bson(b));
  bson_reader_destroy(reader);
  delete[] t;

  return p.null() ? failure<packet>("Failed to decode packet") : success(p);
}

bool tcp_input_channel::is_available() const
{
  return _socket && _socket->is_open();
}
