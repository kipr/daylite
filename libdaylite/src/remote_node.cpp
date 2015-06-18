#include <algorithm>

#include "daylite/remote_node.hpp"
#include "daylite/tcp_transport.hpp"
#include "daylite/mailman.hpp"

using namespace daylite;

remote_node::remote_node(std::unique_ptr<transport> link)
  : _link(std::move(link))
{
}

void_result remote_node::send(const packet &p)
{
  return _link->output().unwrap()->write(p);
}

void_result remote_node::register_recv(uint32_t id, recv_callback_t recv)
{
  _recv_callbacks.emplace(id, recv);
  return success();
}

void_result remote_node::unregister_recv(uint32_t id)
{
  _recv_callbacks.erase(id);
  return success();
}

void_result remote_node::spin_update()
{
  for(;;)
  {
    auto in = _link->input();
    // other side is gone...
    if(in.none()) break;

    auto p = in.unwrap()->read();
    if(!p) break;

    for(auto it = _recv_callbacks.begin(); it != _recv_callbacks.end(); ++it)
    {
      it->second(p.unwrap());
    }
  }

  return success();
}