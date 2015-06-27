#ifndef _DAYLITE_REMOTE_NODE_HPP_
#define _DAYLITE_REMOTE_NODE_HPP_

#include <functional>
#include <memory>
#include <unordered_map>

#include "packet.hpp"
#include "daylite/result.hpp"
#include "spinner_impl.hpp"
#include "mailbox.hpp"

namespace daylite
{
  class transport;

  class remote_node : private spinett, public mailbox
  {
  public:
    remote_node(std::unique_ptr<transport> link);

    inline transport *link() const { return _link.get(); }

  private:
    virtual void_result spin_update();

    void_result send(const packet& packet);

    std::unique_ptr<transport> _link;
  };
}

#endif