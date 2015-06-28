#include "console.hpp"
#include "tcp_server.hpp"

#define LISTEN_QUEUE_SIZE (10U)

using namespace daylite;

tcp_server::tcp_server(const socket_address &address)
  : _address(address)
{

}

tcp_server::~tcp_server()
{
  close();
}

void_result tcp_server::open()
{
  void_result ret;

  if(!(ret = _socket.open())) return ret;
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

  return success();
}

void_result tcp_server::close()
{
  for(auto client : _clients)
  {
    client->close();
    delete client;
  }

  _socket.close();

  return success();
}

void_result tcp_server::add_tcp_server_listener(tcp_server_listener *const listener)
{
  _listeners.push_back(listener);
  return success();
}

void_result tcp_server::remove_tcp_server_listener(tcp_server_listener *const listener)
{
  for(auto it = _listeners.begin(); it != _listeners.end();)
  {
    if(*it != listener) { ++it; continue; }
    it = _listeners.erase(it);
  }

  return success();
}

void_result tcp_server::spin_update()
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

    DAYLITE_DEBUG_STREAM("client disconnected");

    for(auto listener : _listeners) listener->server_disconnection(client);

    client->close();
    delete client;

    it = _clients.erase(it);
  }

  for(;;)
  {
    result<tcp_socket *> client_result = _socket.accept();
    if(!client_result) break;

    DAYLITE_DEBUG_STREAM("Got client");

    tcp_socket *client = client_result.unwrap();
    _clients.push_back(client);

    for(auto listener : _listeners) listener->server_connection(client);
  }

  return success();
}
