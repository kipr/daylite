#include "daylite/mailman.hpp"
#include "daylite/node.hpp"
#include "daylite/remote_node.hpp"
#include "daylite/tcp_transport.hpp"

#include <cassert>

using namespace daylite;
using namespace std;

mailman::mailman() {}

void_result mailman::recv(const packet &p)
{
  if(p.size() < 1) return failure("invalid packet");
  if(p.data()[0] != 0) return failure("invalid packet");
  

  
  // Find the topic's NULL term
  // FIXME: We assume there is always a null term
  uint32_t end = 1;
  while(p.data()[end++]);
  --end;
  
  topic path(string(reinterpret_cast<const char *>(p.data() + 1)));
  
  packet d(p.data() + end, p.size() - end);
  
  auto it = _topic_locals.find(path);
  if(it == _topic_locals.end()) return success();
  
  for(auto local : it->second) local->recv(path, d);
  
  return success();
}

void_result mailman::send(const topic &path, const packet &p)
{
  if(path.name().empty()) return failure("NULL topic invalid for sending");
  
  auto it = _topic_remotes.find(path);
  if(it == _topic_remotes.end()) return success();
  
  uint32_t packed_size = 1 + path.name().size() + 1 + p.size();
  uint8_t *packed_data = new uint8_t[packed_size];
  
  // Packet type topic
  packed_data[0] = 0;
  // Topic
  memcpy(packed_data + 1, path.name().c_str(), path.name().size() + 1);
  // Original packet
  memcpy(packed_data + path.name().size() + 2, p.data(), p.size());
  
  packet pack(packed_data, packed_size);
  
  for(auto remote : it->second) remote->send(pack);
  
  return success();
}

void_result mailman::add_recv(const topic &path, node *const local)
{
  auto it = _topic_locals.find(path);
  if(it == _topic_locals.end())
  {
    _topic_locals.emplace(path, vector<node *> { local });
  }
  else
  {
    it->second.push_back(local);
  }
  
  auto lit = _local_topics.find(local);
  if(lit == _local_topics.end())
  {
    _local_topics.emplace(local, vector<topic> { path });
    return success();
  }
  
  lit->second.emplace_back(path);
  return success();
}

void_result mailman::remove_recv(node *const local)
{
  auto it = _local_topics.find(local);
  if(it == _local_topics.end()) return failure("No such local in mailman");
  
  for(const auto &path : it->second)
  {
    auto lit = _topic_locals.find(path);
    
    // Assuming our internal state is valid, lit will
    // always be valid.
    assert(lit != _topic_locals.end());
    
    for(auto llit = lit->second.begin(); llit != lit->second.end();)
    {
      if(*llit != local) { ++llit; continue; }
      llit = lit->second.erase(llit);
    }
  }
  
  return success();
}

void_result mailman::add_send(const topic &path, remote_node *const remote)
{
  auto it = _topic_remotes.find(path);
  if(it == _topic_remotes.end())
  {
    _topic_remotes.emplace(path, vector<remote_node *> { remote });
  }
  else
  {
    it->second.push_back(remote);
  }
  
  auto lit = _remote_topics.find(remote);
  if(lit == _remote_topics.end())
  {
    _remote_topics.emplace(remote, vector<topic> { path });
    return success();
  }
  
  lit->second.emplace_back(path);
  return success();
}

void_result mailman::remove_send(remote_node *const remote)
{
  auto it = _remote_topics.find(remote);
  if(it == _remote_topics.end()) return failure("No such remote in mailman");
  
  for(const auto &path : it->second)
  {
    auto lit = _topic_remotes.find(path);
    
    // Assuming our internal state is valid, lit will
    // always be valid.
    assert(lit != _topic_remotes.end());
    
    for(auto llit = lit->second.begin(); llit != lit->second.end();)
    {
      if(*llit != remote) { ++llit; continue; }
      llit = lit->second.erase(llit);
    }
  }
  
  return success();
}
