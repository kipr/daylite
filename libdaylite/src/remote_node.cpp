#include "daylite/remote_node.hpp"
#include "daylite/tcp_transport.hpp"
#include "daylite/mailman.hpp"

using namespace daylite;

remote_node::remote_node(std::unique_ptr<transport> link, mailman *const dave)
  : _link(std::move(link))
  , _dave(dave)
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
    
    _dave->recv(p.unwrap());
  }
  
  return success();
}