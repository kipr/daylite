#ifndef _DAYLITE_REMOTE_NODE_HPP_
#define _DAYLITE_REMOTE_NODE_HPP_

#include <functional>
#include <memory>
#include <unordered_map>
#include <set>

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
    enum mode
    {
      allow_internal = 0,
      block_internal
    };

    remote_node(node_impl *const parent, std::unique_ptr<transport> link, const enum mode mode = allow_internal);
    virtual ~remote_node();

    inline transport *link() const { return _link.get(); }
    inline const std::set<uint32_t> &associated_ids() const { return _associated_ids; }
    inline enum mode mode() const { return _mode; }
    
    void shutdown();

  private:
    enum mode _mode;
    node_impl *_parent;
    virtual void_result spin_update();

    void_result send(const packet &packet);

    std::unique_ptr<transport> _link;
    std::set<uint32_t> _associated_ids;
  };
}

#endif
