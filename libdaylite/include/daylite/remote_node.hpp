#ifndef _DAYLITE_REMOTE_NODE_HPP_
#define _DAYLITE_REMOTE_NODE_HPP_

#include <functional>
#include <memory>
#include <unordered_map>

#include "packet.hpp"
#include "result.hpp"
#include "spinner.hpp"

namespace daylite
{
  class transport;

  class remote_node : private spinett
  {
    typedef std::function<void(const packet&)> recv_callback_t;

  public:
    remote_node(std::unique_ptr<transport> link);

    void_result send(const packet &p);

    void_result register_recv(uint32_t id, recv_callback_t recv);
    void_result unregister_recv(uint32_t id);

    inline transport *link() const { return _link.get(); }

  private:
    virtual void_result spin_update();

    std::unique_ptr<transport> _link;
    std::unordered_map<uint32_t, recv_callback_t> _recv_callbacks;
  };
}

#endif