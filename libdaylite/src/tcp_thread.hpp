#ifndef _DAYLITE_TCP_THREAD_HPP_
#define _DAYLITE_TCP_THREAD_HPP_

#include "packet.hpp"
#include <daylite/result.hpp>
#include <deque>
#include <unordered_map>
#include <thread>
#include <memory>


namespace daylite
{
  class tcp_socket;

  class tcp_thread
  {
  public:
    tcp_thread();
    void_result add_socket(tcp_socket *const socket);
    void_result remove_socket(tcp_socket *const socket);
    result<packet> next(tcp_socket *const socket);

  private:
    struct buffer
    {
      std::mutex mut;
      std::deque<packet> queue;
    };

    void run();

    std::unordered_map<tcp_socket *, std::unique_ptr<buffer>> _buffers;
    std::thread _thread;
  };
}

#endif
