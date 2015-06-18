#ifndef _DAYLITE_MAILMAN_HPP_
#define _DAYLITE_MAILMAN_HPP_

#include "compat.hpp"
#include "result.hpp"
#include "packet.hpp"
#include "topic.hpp"

#include <unordered_map>
#include <vector>

namespace daylite
{
  class node;
  class remote_node;
  
  class DLL_EXPORT mailman
  {
  public:
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

    void_result add_recv(const topic &path, node *const local);
    void_result remove_recv(node *const local);

    void_result add_send(const topic &path, remote_node *const remote);
    void_result remove_send(remote_node *const remote);
    
  private:
#pragma warning(push)
#pragma warning(disable: 4251) /* Ignore STL DLL export warnings; Can be ignored as they are not exposed by inline functions */
    std::unordered_map<node *, std::vector<topic>> _local_topics;
    std::unordered_map<topic, std::vector<node *>> _topic_locals;
    
    std::unordered_map<remote_node *, std::vector<topic>> _remote_topics;
    std::unordered_map<topic, std::vector<remote_node *>> _topic_remotes;

#pragma warning(pop)
  };
}

#endif