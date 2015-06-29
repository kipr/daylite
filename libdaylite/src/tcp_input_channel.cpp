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
    result<size_t> ret = _socket->recv(tmp, sizeof(tmp));
    if(!ret) return failure<packet>(ret.message(), ret.code());
    _buffer.insert(_buffer.end(), tmp, tmp + ret.unwrap());
  }

  // got something but not all
  if(_buffer.size() < sizeof(uint32_t)) return failure<packet>("Ran out of time", EAGAIN);

  // Keep reading until time is up or we reach the requisite length
  uint32_t target_size = *reinterpret_cast<uint32_t *>(_buffer.data());
  if(_buffer.size() < target_size)
  {
    result<size_t> ret = _socket->recv(tmp, sizeof(tmp));
    if(!ret) return failure<packet>(ret.message(), ret.code());
    _buffer.insert(_buffer.end(), tmp, tmp + ret.unwrap());
  }

  if(_buffer.size() < target_size) return failure<packet>("Ran out of time", EAGAIN);

  // Got all of the packet. Chop it off of the buffer and return it.
  bson_reader_t *reader;
  const bson_t *doc;

  reader = bson_reader_new_from_data(_buffer.data(), target_size);
  if(!reader)
  {
    bson_reader_destroy(reader);
    _buffer.erase(_buffer.begin(), _buffer.begin() + target_size);
    return failure<packet>("Could not create the BSON reader");
  }

  doc = bson_reader_read(reader, nullptr);

  packet p(doc);

  _buffer.erase(_buffer.begin(), _buffer.begin() + target_size);
  bson_reader_destroy(reader);

  return success(p);
}

bool tcp_input_channel::is_available() const
{
  return _socket && _socket->is_open();
}
