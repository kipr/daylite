#include "daylite/tcp_client_transport.hpp"

#include "tcp_input_channel.hpp"
#include "tcp_output_channel.hpp"

using namespace daylite;

tcp_client_transport::tcp_client_transport(const socket_address &address)
  : _address(address)
  , _input_channel(0)
  , _output_channel(0)
{
  
}

tcp_client_transport::~tcp_client_transport()
{
  close();
}

void_result tcp_client_transport::open()
{
  void_result ret;
  
  if(!(ret = _socket.open())) return ret;
  if(!(ret = _socket.set_blocking(false)))
  {
    _socket.close();
    return ret;
  }
  if(!(ret = _socket.connect(_address)))
  {
    _socket.close();
    return ret;
  }

  _input_channel  = new tcp_input_channel(&_socket);
  _output_channel = new tcp_output_channel(&_socket);
  
  return success();
}

void_result tcp_client_transport::close()
{
  delete _input_channel;
  _input_channel = 0;
  
  delete _output_channel;
  _output_channel = 0;
  
  _socket.close();
  
  return success();
}

option<input_channel *>  tcp_client_transport::input()  const
{
  return _input_channel ? some(_input_channel) : none<input_channel *>();
}

option<output_channel *> tcp_client_transport::output() const
{
  return _output_channel ? some(_output_channel) : none<output_channel *>();
}

void_result tcp_client_transport::spin_update()
{
  uint8_t tmp[1];
  result<size_t> ret = _socket.recv(tmp, sizeof(tmp), tcp_socket::comm_peek);
  
  // Recv will return no error and ret == 0 when a client disconnects
  // if there's still a connection but no data, it will return the EAGAIN error.
  if(!ret || ret.unwrap() > 0) return success();
  
  close();
  
  return success();
}
