#include "daylite/tcp_server_transport.hpp"

#include "tcp_input_channel.hpp"
#include "tcp_output_channel.hpp"
#include "broadcast_output_channel.hpp"

#warning temp
#include <iostream>

#define LISTEN_QUEUE_SIZE (10U)

using namespace daylite;

tcp_server_transport::tcp_server_transport(const socket_address &address)
  : _address(address)
  , _input_channel(0)
  , _output_channel(0)
{
  
}

tcp_server_transport::~tcp_server_transport()
{
  close();
}

void_result tcp_server_transport::open()
{
  void_result ret;
  
  if(!(ret = _socket.open())) return ret;
  if(!(ret = _socket.set_blocking(false)))
  {
    _socket.close();
    return ret;
  }
  if(!(ret = _socket.bind(_address)))
  {
    _socket.close();
    return ret;
  }
  if(!(ret = _socket.listen(LISTEN_QUEUE_SIZE)))
  {
    _socket.close();
    return ret;
  }

  _input_channel  = new tcp_input_channel(&_socket);
  _output_channel = new broadcast_output_channel();
  
  return success();
}

void_result tcp_server_transport::close()
{
  delete _input_channel;
  _input_channel = 0;
  
  delete _output_channel;
  _output_channel = 0;
  
  for(auto client : _clients)
  {
    client->close();
    delete client;
  }
  
  _socket.close();
  
  return success();
}

option<input_channel *>  tcp_server_transport::input()  const
{
  return _input_channel ? some(_input_channel) : none<input_channel *>();
}

option<output_channel *> tcp_server_transport::output() const
{
  return _output_channel ? some(_output_channel) : none<output_channel *>();
}

void_result tcp_server_transport::spin_update()
{
  // Detect disconnects
  for(auto it = _clients.begin(); it != _clients.end();)
  {
    auto client = *it;
    
    uint8_t tmp[1];
    result<size_t> ret = client->recv(tmp, sizeof(tmp), tcp_socket::comm_peek);
    
    // Recv will return no error and ret == 0 when a client disconnects
    // if there's still a connection but no data, it will return the EAGAIN error.
    if(!ret || ret.unwrap() > 0) { ++it; continue; }
    
    std::cout << "client disconnected" << std::endl;
    reinterpret_cast<broadcast_output_channel *>(_output_channel)->remove_output_channel(client);
    client->close();
    delete client;
    
    it = _clients.erase(it);
  }
  
  result<tcp_socket *> client_result = _socket.accept();
  if(!client_result) return success();
  
  std::cout << "Got client" << std::endl;
  
  tcp_socket *client = client_result.unwrap();
  client->set_blocking(false);
  _clients.push_back(client);
  
  reinterpret_cast<broadcast_output_channel *>(_output_channel)->add_output_channel(new tcp_output_channel(client), client);
  
  return success();
}
