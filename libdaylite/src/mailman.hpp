#ifndef _DAYLITE_MAILMAN_HPP_
#define _DAYLITE_MAILMAN_HPP_

#include "result.hpp"
#include "packet.hpp"
#include "topic.hpp"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace daylite
{
  class mailbox;

  class mailman
  {
    typedef std::function<void(const packet&)> receiver_callback_t;

  public:
    void_result register_mailbox(std::shared_ptr<mailbox> mailbox);
    void_result unregister_mailbox(std::shared_ptr<mailbox> mailbox);

    /**
     * Causes mailman to send a packet on the given topic (topic included in the packet).
     * Delivered to all local mailboxes.
     */
    void_result send(std::unique_ptr<packet> packet);

  private:
    std::unordered_map<topic, std::unordered_map<uint32_t, std::shared_ptr<mailbox>>> _mailboxes;
  };
}

#endif