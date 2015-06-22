#include <algorithm>

#include "remote_node.hpp"
#include "tcp_transport.hpp"
#include "mailman.hpp"

using namespace daylite;

remote_node::remote_node(std::unique_ptr<transport> link)
  : mailbox(topic::any, [this](std::shared_ptr<packet> p) { return send(*p.get()); })
  , _link(std::move(link))
{
}

void_result remote_node::send(const packet &p)
{
  return _link->output().unwrap()->write(p);
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

    place_outgoing_mail(std::make_unique<packet>(p.unwrap()));
  }

  return success();
}