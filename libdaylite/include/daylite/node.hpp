#ifndef _DAYLITE_NODE_HPP_
#define _DAYLITE_NODE_HPP_

#include "topic.hpp"
#include "publisher.hpp"
#include "subscriber.hpp"

#include "result.hpp"
#include "option.hpp"
#include "socket_address.hpp"
#include "transport.hpp"

#include "spinner.hpp"

#include <vector>
#include <unordered_map>

namespace daylite
{
  class node : private spinett
  {
  public:
    node(const std::string &name);
    ~node();
    
    void_result start(const option<socket_address> &master = none<socket_address>(),
      const option<socket_address> &us = none<socket_address>());
    void_result end();
    
    inline const std::string &name() const { return _name; }
    
  private:
    virtual void_result spin_update();
    
    void_result process_packet(const packet &p);
    
    void_result add_remote_topic(const packet &p);
    void_result remove_remote_topic(const packet &p);
    
    void_result publication(const packet &p);
    
    
    std::string _name;
    
    transport *_server;
    std::vector<transport *> _clients;
    
    // std::unordered_map<topic, std::vector<transport *>> _client_routes;
  };
}

#endif