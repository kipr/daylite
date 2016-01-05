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
  // Do not send packets marked as local_only
  if(p.local_only()) return success();
  
  // Check to see if anyone is actually listening
  // for this on this branch of the network.
  auto it = _parent->_subscription_count.find(p.meta().topic);
  const bool internal_check = _mode == remote_node::allow_internal && p.meta().topic == topic::internal.name();
  if(!internal_check && it == _parent->_subscription_count.end()) return success();

  packet mod = p;
  mod.meta().path.push_back(_parent->id());
  mod.meta().id = _parent->id();
  mod.build();
  
  bool should_send = true;
  
#ifdef SPLAT_ENABLED
  bool splattable = p.meta().droppable && p.meta().origin_id == _parent->_id;
  // cout << "REMOTE NODE SEND " << p.meta().topic << ": " << p.packed()->len << " (splat: " << splattable << ")" << endl;
  if(splattable)
  {
    auto ret = _parent->push_splat(mod);
    if(!ret)
    {
      cout << "Warning: Failed to push splat: " << ret.message() << endl;
    }
  }
  // FIXME: This is not the correct condition.
  // We still want to send to *truly* remote clients
  // over TCP, but there is currently no way to
  // identify unique systems on the network to
  // generate disconnected splat neighborhoods.
  should_send = !splattable;
#endif
  
  return should_send ? _parent->_thread.send(_link.get(), mod) : success();
}

void_result remote_node::spin_update()
{
#ifdef SPLAT_ENABLED
  _parent->update_splats();
#endif
  for(;;)
  {
    auto p = _parent->_thread.next(_link.get());
    if(p.null()) break;

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
