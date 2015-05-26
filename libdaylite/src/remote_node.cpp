#include "daylite/remote_node.hpp"
#include "daylite/tcp_transport.hpp"
#include "daylite/mailman.hpp"

using namespace daylite;

remote_node::remote_node(tcp_transport *const link, mailman *const dave)
  : _link(link)
  , _dave(dave)
{
}

remote_node::~remote_node()
{
  delete _link;
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
    
    _dave->recv(p.unwrap());
  }
  
  return success();
}