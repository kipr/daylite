#ifndef _DAYLITE_REMOTE_NODE_HPP_
#define _DAYLITE_REMOTE_NODE_HPP_

#include <functional>
#include <memory>
#include <unordered_map>

#include "packet.hpp"
#include "daylite/result.hpp"
#include "spinner_impl.hpp"
#include "mailbox.hpp"
#include "node_info.hpp"

namespace daylite
{
  class transport;
  class node_impl;

  class remote_node : private spinett, public mailbox
  {
  public:
    remote_node(node_impl *const parent, std::unique_ptr<transport> link);
    virtual ~remote_node();

    inline transport *link() const { return _link.get(); }


  private:
    node_impl *_parent;
    virtual void_result spin_update();

    void_result send(const packet &packet);

    std::unique_ptr<transport> _link;
  };
}

#endif
