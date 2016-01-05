#ifndef _DAYLITE_TCP_THREAD_HPP_
#define _DAYLITE_TCP_THREAD_HPP_

#include "packet.hpp"

#include <daylite/result.hpp>
#include <deque>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <memory>
#include <string>
#include <unordered_set>


namespace daylite
{
  class transport;

  class tcp_thread
  {
  public:
    tcp_thread();
    void_result add_socket(transport *const socket);
    void_result remove_socket(transport *const socket);
    packet next(transport *const socket);
    void_result send(transport *const socket, const packet &p);

    void exit();
    
    uint16_t sleep_duration() const { return _sleep; }

  private:
    struct buffer
    {
      std::mutex mut;
      std::unordered_map<std::string, std::deque<packet>> topic_queues;
      std::deque<packet> in;
      std::unordered_set<std::string> in_firehose;
      std::deque<packet> out;
      std::unordered_set<std::string> out_firehose;
      uint32_t reject_out_count;
    };

    void run();
    void update_sleep(const uint16_t new_packet_count);
    
    std::mutex _mut;
    std::unordered_map<transport *, std::unique_ptr<buffer>> _buffers;
    std::thread _thread;
    uint16_t _sleep;
    bool _exit;
  };
}

#endif
