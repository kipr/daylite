#include "tcp_output_channel.hpp"
#include "tcp_socket.hpp"

#include <bson.h>

#include <cstring>
#include <errno.h>

using namespace daylite;

tcp_output_channel::tcp_output_channel(tcp_socket *const socket)
  : _socket(socket)
{
}

tcp_output_channel::~tcp_output_channel()
{
}

void_result tcp_output_channel::write(const packet &p)
{
  uint32_t written = 0;
  const uint32_t size = p.get_msg()->len;
  const uint8_t *data = bson_get_data(p.get_msg());
  
  while(written < size)
  {
    result<size_t> ret = _socket->send(data + written, size - written);
    if(ret.code() == EAGAIN) continue;
    if(!ret) return ret.to_void_result();
    written += ret.unwrap();
  }
  
  return success();
}

bool tcp_output_channel::is_available() const
{
  return _socket && _socket->is_open();
}