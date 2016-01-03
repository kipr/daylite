#include "tcp_thread.hpp"
#include "tcp_transport.hpp"
#include <functional>
#include <iostream>

using namespace daylite;
using namespace std;

static void print_bson(const bson_t *const msg)
{
  size_t len;
  char *str;
  str = bson_as_json (msg, &len);
  cout << str << endl;
  bson_free (str);
}

tcp_thread::tcp_thread()
  : _exit(false)
{
  _thread = thread(bind(&tcp_thread::run, this));
}

void_result tcp_thread::add_socket(transport *const socket)
{
  assert(socket);
  if(_buffers.find(socket) != _buffers.end()) return failure("Socket already watched by thread");
  _mut.lock();
  _buffers[socket] = unique_ptr<buffer>(new buffer);
  _mut.unlock();
  return success();
}

void_result tcp_thread::remove_socket(transport *const socket)
{
  assert(socket);
  auto it = _buffers.find(socket);
  if(it == _buffers.end()) return failure("Socket not found");
  
  _mut.lock();
  _buffers.erase(it);
  _mut.unlock();
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

void_result tcp_thread::send(transport *const socket, const packet &p)
{
  assert(socket);
  assert(!p.null());
  auto it = _buffers.find(socket);
  if(it == _buffers.end()) return failure("No such socket found");
  it->second->mut.lock();
  it->second->out.push_back(p);
  it->second->mut.unlock();
  return success();
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
    _mut.lock();
    for(auto it = _buffers.begin(); it != _buffers.end(); ++it)
    {
      if(!it->second) continue;
      
      auto l = it->first;
      
      auto out = l->output();
      if(out.some())
      {
        it->second->mut.lock();
        for(const packet &k : it->second->out)
        {
          if(!out.unwrap()->write(k))
          {
            cout << "Failed to write packet!" << endl;
          }
        }
        it->second->out.clear();
        it->second->mut.unlock();
      }
      
      l->update();
      
      for(uint32_t i = 0; i < 10U; ++i)
      {
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
    }
    _mut.unlock();
    
    usleep(1000U);
    //this_thread::yield();
  }
}
