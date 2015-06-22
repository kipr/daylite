#include "tcp_output_channel.hpp"
#include "tcp_socket.hpp"

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
  size_t written = 0;
  
  const uint32_t size = p.size();
  result<size_t> ret = _socket->send(reinterpret_cast<const uint8_t *>(&size), sizeof(uint32_t));
  if(!ret || ret.unwrap() != sizeof(uint32_t))
  {
    return !ret ? ret.to_void_result() : failure("Failed to write all bytes of size");
  }
  
  while(written < p.size())
  {
    result<size_t> ret = _socket->send(p.data() + written, p.size() - written);
    if(ret.code() == EAGAIN) continue;
    if(!ret) return ret.to_void_result();
    written += ret.unwrap();
  }
  
  return success();
}