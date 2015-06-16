#ifndef _DAYLIGHT_TCP_SERVER_HPP_
#define _DAYLIGHT_TCP_SERVER_HPP_

#include "transport.hpp"
#include "tcp_socket.hpp"
#include "spinner.hpp"
#include <vector>

namespace daylite
{
  class tcp_server_listener
  {
  public:
    virtual void server_connection(tcp_socket *const client) = 0;
    virtual void server_disconnection(tcp_socket *const client) = 0;
  };
  
  class tcp_server : private spinett
  {
  public:
    tcp_server(const socket_address &address);
    virtual ~tcp_server();
    
    void_result open();
    void_result close();
    
    void_result add_tcp_server_listener(tcp_server_listener *const listener);
    void_result remove_tcp_server_listener(tcp_server_listener *const listener);
    
  private:
    virtual void_result spin_update();
    
    socket_address _address;
    input_channel *_input_channel;
    output_channel *_output_channel;
    tcp_socket _socket;
    std::vector<tcp_socket *> _clients;
    
    std::vector<tcp_server_listener *> _listeners;
  };
}

#endif