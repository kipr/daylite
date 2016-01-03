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
  , mailbox(topic::any, [this](const packet &p) { return send(p); })
  , _link(move(link))
  , _mode(mode)
{
  _parent->_thread.add_socket(_link.get());
}

remote_node::~remote_node()
{
  _parent->_thread.remove_socket(_link.get());
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
  for(uint16_t i = 0; i < 1000U; ++i)
  {
    auto p = _parent->_thread.next(_link.get());
    if(p.null()) continue;

    // If we have already received this message,
    // ignore it.
    auto path = p.meta().path;
    bool found = p.meta().id == _parent->id();
    for(auto id : path)
    {
      found |= id == _parent->id();
    }
    if(found) continue;

    _associated_ids.insert(p.meta().id);

    place_outgoing_mail(p);
  }

  return success();
}
