#include "tcp_thread.hpp"
#include "tcp_transport.hpp"
#include <functional>
#include <iostream>
#include <unordered_set>
#include <string>
#include <mutex>

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
  , _sleep(10000U)
{
  _thread = thread(bind(&tcp_thread::run, this));
}

void_result tcp_thread::add_socket(transport *const socket)
{
  assert(socket);
  if(_buffers.find(socket) != _buffers.end()) return failure("Socket already watched by thread");
  lock_guard<mutex> lock(_mut);
  _buffers[socket] = unique_ptr<buffer>(new buffer);
  return success();
}

void_result tcp_thread::remove_socket(transport *const socket)
{
  assert(socket);
  auto it = _buffers.find(socket);
  if(it == _buffers.end()) return failure("Socket not found");
  
  lock_guard<mutex> lock(_mut);
  _buffers.erase(it);
  return success();
}

packet tcp_thread::next(transport *const socket)
{
  assert(socket);
  auto it = _buffers.find(socket);
  if(it == _buffers.end()) return packet();
  
  auto &buff = it->second;
  
  lock_guard<mutex> lock(buff->mut);

  if(buff->in.empty()) return packet();
  
  auto ret = buff->in.front();
  buff->in.pop_front();
  
  if(ret.meta().droppable)
  {
    const auto &name = ret.topic().name();
    auto pit = buff->in_firehose.find(name);
    if(pit != buff->in_firehose.end()) buff->in_firehose.erase(pit);
  }
  
  return ret;
}

void_result tcp_thread::send(transport *const socket, const packet &p)
{
  assert(socket);
  assert(!p.null());
  auto it = _buffers.find(socket);
  if(it == _buffers.end()) return failure("No such socket found");
  auto &buff = it->second;
  
  lock_guard<mutex> lock(buff->mut);
  if(p.meta().droppable)
  {
    const auto &name = p.topic().name();
    if(buff->out_firehose.find(name) != buff->out_firehose.end())
    {
      ++buff->reject_out_count;
      return success();
    }
    buff->out_firehose.insert(name); 
  }
  buff->out.push_back(p);
  
  return success();
}

void tcp_thread::exit()
{
  _exit = true;
  _thread.join();
}

void tcp_thread::run()
{
  uint32_t update_sleep_it = 0;
  while(!_exit)
  {
    _mut.lock();
    uint32_t out_packet_count = 0;
    uint32_t in_packet_count = 0;
    
    for(auto it = _buffers.begin(); it != _buffers.end(); ++it)
    {
      if(!it->second) continue;
      
      auto l = it->first;
      auto &buff = it->second;
      
      auto out = l->output();
      if(out.some())
      {
        lock_guard<mutex> lock(buff->mut);
        for(const packet &k : buff->out)
        {
          if(!out.unwrap()->write(k)) cout << "Failed to write packet!" << endl;
        }
        
        out_packet_count = buff->out.size() + buff->reject_out_count;
        buff->out.clear();
        buff->reject_out_count = 0;
        buff->out_firehose.clear();
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
        const auto &topic = p_val.topic().name();

        {
          lock_guard<mutex> lock(buff->mut);
          if(p_val.meta().droppable)
          {
            if(buff->in_firehose.find(topic) != buff->in_firehose.end()) continue;
            buff->in_firehose.insert(topic);
          }
          buff->in.push_back(p_val);
        }
      }
      in_packet_count = buff->in.size();
    }
    _mut.unlock();
    
    update_sleep(std::max(in_packet_count, out_packet_count));
  }
}

void tcp_thread::update_sleep(const uint32_t new_packet_count)
{
  
  const static uint32_t max_sleep = 20000U;
  const static uint32_t min_sleep = 2000U;
  const uint32_t temporal_packet_distance = new_packet_count ? _sleep / new_packet_count : max_sleep;
  _sleep = std::max(temporal_packet_distance, min_sleep);
  usleep(_sleep);
  // static uint8_t i = 0;
  // if(!i++) cout << "Sleep time: " << _sleep << "(" << new_packet_count << ")" << endl;
}
