#include <algorithm>

#include "remote_node.hpp"
#include "tcp_transport.hpp"
#include "mailman.hpp"
#include "node_impl.hpp"

#include <iostream>

using namespace daylite;
using namespace std;

remote_node::remote_node(node_impl *const parent, unique_ptr<transport> link)
  : _parent(parent)
  , mailbox(topic::any, [this](shared_ptr<packet> p) { return send(*p.get()); })
  , _link(move(link))
{
}

remote_node::~remote_node()
{
}

void_result remote_node::send(const packet &p)
{
  packet mod = p;
  mod.meta().path.push_back(_parent->id());
  mod.meta().id = _parent->id();
  auto out = _link->output();
  if(out.none()) return failure("link not open");
  return out.unwrap()->write(p);
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

    auto p_val = p.unwrap();
    // If we have already received this message,
    // ignore it.
    auto path = p_val.meta().path;
    bool found = false;
    for(auto id : path)
    {
      if((found = id == _parent->id())) break;
    }
    if(found) continue;

    // Reset the keep alive for this node
    _parent->touch_node(p_val.meta().id);

    place_outgoing_mail(make_unique<packet>(p_val));
  }

  return success();
}
