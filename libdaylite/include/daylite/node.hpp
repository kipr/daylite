#ifndef _DAYLITE_NODE_HPP_
#define _DAYLITE_NODE_HPP_
#include "compat.hpp"
#include "topic.hpp"
#include "publisher.hpp"
#include "subscriber.hpp"

#include "result.hpp"
#include "option.hpp"
#include "socket_address.hpp"

#include "mailman.hpp"

#include "spinner.hpp"
#include "tcp_server.hpp"

#include <vector>

namespace daylite
{
  class remote_node;

  // DLL-Export STL templates (MS Article 168958)
  EXPIMP_TEMPLATE template class DLL_EXPORT std::vector<remote_node *>;
  EXPIMP_TEMPLATE template class DLL_EXPORT std::basic_string<char>;
  
  class DLL_EXPORT node : private tcp_server_listener
  {
  public:
    node(const std::string &name);
    ~node();
    
    void_result start(const option<socket_address> &master = none<socket_address>(),
      const option<socket_address> &us = none<socket_address>());
    void_result end();
    
    inline const std::string &name() const { return _name; }
    inline const mailman *dave() const { return &_dave; }
    inline mailman *dave() { return &_dave; }
    
    void_result send(const topic &path, const packet &p);
    void_result recv(const topic &path, const packet &p);
    
  private:
    virtual void server_connection(tcp_socket *const socket);
    virtual void server_disconnection(tcp_socket *const socket);

    std::string _name;
    mailman _dave;
    
    tcp_server *_server;
    std::vector<remote_node *> _remotes;
  };
}

#endif