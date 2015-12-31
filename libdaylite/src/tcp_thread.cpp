#include "tcp_thread.hpp"
#include <functional>

using namespace daylite;
using namespace std;

tcp_thread::tcp_thread()
  : _thread(bind(&tcp_thread::run, this))
{
}

void_result tcp_thread::add_socket(tcp_socket *const socket)
{
  assert(socket);
  if(_buffers.find(socket) != _buffers.end()) return failure("Socket already watched by thread");
  _buffers[socket] = unique_ptr<buffer>(new buffer);
  return success();
}

void_result tcp_thread::remove_socket(tcp_socket *const socket)
{
  assert(socket);
  auto it = _buffers.find(socket);
  if(it == _buffers.end()) return failure("Socket not found");
  _buffers.erase(it);
  return success();
}

result<packet> tcp_thread::next(tcp_socket *const socket)
{
  assert(socket);
  auto it = _buffers.find(socket);
  if(it == _buffers.end()) return failure<packet>("Socket not found");
  auto &buff = it->second;
  lock_guard<mutex> lock(buff->mut);
  auto ret = move(buff->queue.front());
  buff->queue.pop_front();
  return success(ret);
}

void tcp_thread::run()
{
}
