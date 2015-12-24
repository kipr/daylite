#include <algorithm>

#include "remote_node.hpp"
#include "tcp_transport.hpp"
#include "mailman.hpp"
#include "node_impl.hpp"

#include <iostream>

using namespace daylite;
using namespace std;

remote_node::remote_node(node_impl *const parent, unique_ptr<transport> link, const enum mode mode)
  : _parent(parent)
  , mailbox(topic::any, [this](shared_ptr<packet> p) { return send(*p.get()); })
  , _link(move(link))
  , _mode(mode)
{
}

remote_node::~remote_node()
{
}

void_result remote_node::send(const packet &p)
{
  // Check to see if anyone is actually listening
  // for this on this branch of the network.
  auto it = _parent->_subscription_count.find(p.meta().topic);
  const bool internal_check = _mode == remote_node::allow_internal && p.meta().topic == topic::internal.name();
  if(!internal_check && it == _parent->_subscription_count.end()) return success();

  packet mod = p;
  mod.meta().path.push_back(_parent->id());
  mod.meta().id = _parent->id();
  mod.build();
  auto out = _link->output();
  if(out.none()) return failure("link not open");
  return out.unwrap()->write(mod);
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
    bool found = p_val.meta().id == _parent->id();
    for(auto id : path)
    {
      found |= id == _parent->id();
    }
    if(found) continue;

    _associated_ids.insert(p_val.meta().id);

    // Reset the keep alive for this node
    if(!_parent->touch_node(p_val.meta().id))
    {
      std::cout << "Warning: Unable to update keepalive for node " << p_val.meta().id << std::endl;
    }

    place_outgoing_mail(make_unique<packet>(p_val));
  }

  return success();
}
