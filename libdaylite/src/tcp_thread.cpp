#include "tcp_thread.hpp"
#include "tcp_transport.hpp"
#include <functional>
#include <iostream>

using namespace daylite;
using namespace std;

tcp_thread::tcp_thread()
  : _thread(bind(&tcp_thread::run, this))
{
}

void_result tcp_thread::add_socket(transport *const socket)
{
  assert(socket);
  if(_buffers.find(socket) != _buffers.end()) return failure("Socket already watched by thread");
  _buffers[socket] = unique_ptr<buffer>(new buffer);
  return success();
}

void_result tcp_thread::remove_socket(transport *const socket)
{
  assert(socket);
  auto it = _buffers.find(socket);
  if(it == _buffers.end()) return failure("Socket not found");
  _buffers.erase(it);
  return success();
}

packet tcp_thread::next(transport *const socket)
{
  assert(socket);
  auto it = _buffers.find(socket);
  if(it == _buffers.end()) return packet();
  
  auto &buff = it->second;
  
  buff->mut.lock();

  for(auto pit = buff->topic_queues.begin(); pit != buff->topic_queues.end(); ++pit)
  {
    if(pit->second.empty()) continue;
    
    packet ret(pit->second.front());
    pit->second.pop_front();
    if(pit->second.empty()) buff->topic_queues.erase(pit);
    
    buff->mut.unlock();
    return ret;
  }
  
  buff->mut.unlock();
  
  return packet();
}

void tcp_thread::exit()
{
  _exit = true;
  _thread.join();
}

void tcp_thread::run()
{
  while(!_exit)
  {
    for(auto it = _buffers.begin(); it != _buffers.end(); ++it)
    {
      auto l = it->first;
      l->update();
      auto in = l->input();
      // other side is gone...
      if(in.none()) continue;

      auto p = in.unwrap()->read();
      if(!p) break;

      auto p_val = p.unwrap();
      auto topic = p_val.topic().name();
      
      it->second->mut.lock();
      
      auto lit = it->second->topic_queues.find(topic);
      if(lit == it->second->topic_queues.end())
      {
        it->second->topic_queues[topic] = deque<packet> { p_val };
      }
      else
      {
        // Drop packets that are now outdated
        for(auto pit = lit->second.begin(); pit != lit->second.end();)
        {
          if(!pit->meta().droppable) { ++pit; continue; }

          pit = lit->second.erase(pit);
        }

        lit->second.push_back(p_val);

      }
      
      it->second->mut.unlock();
    }
    
    this_thread::yield();
  }
}
