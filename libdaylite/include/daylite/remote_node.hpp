#ifndef _DAYLITE_REMOTE_NODE_HPP_
#define _DAYLITE_REMOTE_NODE_HPP_

#include "spinner.hpp"

namespace daylite
{
  class tcp_transport;
  class mailman;
  
  class remote_node : private spinett
  {
  public:
    remote_node(tcp_transport *const link, mailman *const dave);
    ~remote_node();
    
    inline tcp_transport *link() const { return _link; }
    inline mailman *dave() const { return _dave; }
    
  private:
    virtual void_result spin_update();
    
    mailman *_dave;
    tcp_transport *_link;
  };
}

#endif