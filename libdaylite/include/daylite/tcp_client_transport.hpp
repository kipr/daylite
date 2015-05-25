#ifndef _DAYLIGHT_TCP_CLIENT_TRANSPORT_HPP_
#define _DAYLIGHT_TCP_CLIENT_TRANSPORT_HPP_

#include "transport.hpp"
#include "tcp_socket.hpp"
#include "spinner.hpp"

namespace daylite
{
  class tcp_client_transport : public transport, private spinett
  {
  public:
    tcp_client_transport(const socket_address &address);
    virtual ~tcp_client_transport();
    
    virtual void_result open();
    virtual void_result close();
    
    virtual option<input_channel  *> input()  const;
    virtual option<output_channel *> output() const;
    
  private:
    virtual void_result spin_update();
    
    socket_address _address;
    input_channel *_input_channel;
    output_channel *_output_channel;
    tcp_socket _socket;
  };
}

#endif