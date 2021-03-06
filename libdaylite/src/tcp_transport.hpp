#ifndef _DAYLIGHT_TCP_TRANSPORT_HPP_
#define _DAYLIGHT_TCP_TRANSPORT_HPP_

#include "transport.hpp"
#include "tcp_socket.hpp"
#include "spinner_impl.hpp"

namespace daylite
{
  class tcp_transport : public transport
  {
  public:
    tcp_transport(const socket_address &address);
    tcp_transport(tcp_socket *const socket, const bool owns = true);
    virtual ~tcp_transport();
    
    virtual void_result open();
    virtual void_result close();
    
    virtual option<input_channel  *> input()  const;
    virtual option<output_channel *> output() const;
    
    inline const socket_address &address() { return _address; }
    inline tcp_socket *socket() const { return _socket; }
    
    virtual void_result update();
    
    socket_address _address;
    input_channel *_input_channel;
    output_channel *_output_channel;
    tcp_socket *_socket;
    bool _owns;
  };
}

#endif