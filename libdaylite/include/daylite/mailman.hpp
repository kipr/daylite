#ifndef _DAYLITE_MAILMAN_HPP_
#define _DAYLITE_MAILMAN_HPP_

#include "result.hpp"
#include "packet.hpp"
#include "topic.hpp"

#include <unordered_map>
#include <vector>

namespace daylite
{
  class node;
  class remote_node;
  
  class mailman
  {
  public:
    friend class node;
    friend class remote_node;
    
    mailman();
    
    /**
     * Causes mailman to receive a packet on the given topic.
     * Delivered to local nodes.
     */
    void_result recv(const packet &p);
    
    /**
     * Causes mailman to send a packet on the given topic.
     * Delivered to remote nodes.
     */
    void_result send(const topic &path, const packet &p);
    
  private:
    void_result add_recv(const topic &path, node *const local);
    void_result remove_recv(node *const local);
    
    void_result add_send(const topic &path, remote_node *const remote);
    void_result remove_send(remote_node *const remote);
    
    std::unordered_map<node *, std::vector<topic>> _local_topics;
    std::unordered_map<topic, std::vector<node *>> _topic_locals;
    
    std::unordered_map<remote_node *, std::vector<topic>> _remote_topics;
    std::unordered_map<topic, std::vector<remote_node *>> _topic_remotes;
  };
}

#endif