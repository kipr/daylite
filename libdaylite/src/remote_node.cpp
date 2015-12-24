#include <algorithm>

#include "remote_node.hpp"
#include "tcp_transport.hpp"
#include "mailman.hpp"

#include <iostream>

using namespace daylite;
using namespace std;

remote_node::remote_node(unique_ptr<transport> link)
  : mailbox(topic::any, [this](shared_ptr<packet> p) { return send(*p.get()); })
  , _link(move(link))
{
}

remote_node::~remote_node()
{
}

void_result remote_node::send(const packet &p)
{
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
    place_outgoing_mail(make_unique<packet>(p_val));
  }

  return success();
}
