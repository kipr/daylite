#ifndef _DAYLIGHT_TCP_SERVER_LISTENER_HPP_
#define _DAYLIGHT_TCP_SERVER_LISTENER_HPP_

namespace daylite
{
  class tcp_socket;

  class tcp_server_listener
  {
  public:
    virtual void server_connection(tcp_socket *const client) = 0;
    virtual void server_disconnection(tcp_socket *const client) = 0;
  };
}

#endif
