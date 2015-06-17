#ifndef _DAYLITE_REMOTE_NODE_HPP_
#define _DAYLITE_REMOTE_NODE_HPP_

#include <memory>

#include "packet.hpp"
#include "result.hpp"
#include "spinner.hpp"

namespace daylite
{
  class transport;
  class mailman;
  
  class remote_node : private spinett
  {
  public:
    remote_node(std::unique_ptr<transport> link, mailman *const dave);

    void_result send(const packet &p);
    
    inline transport *link() const { return _link.get(); }
    inline mailman *dave() const { return _dave; }
    
  private:
    virtual void_result spin_update();
    
    mailman *_dave;
    std::unique_ptr<transport> _link;
  };
}

#endif