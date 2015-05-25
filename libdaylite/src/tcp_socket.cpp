#include "daylite/tcp_socket.hpp"
#include "daylite/option.hpp"

#include <cstring>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

using namespace daylite;

namespace
{
  option<sockaddr_in> socket_address_to_sockaddr(const socket_address &address)
  {
    sockaddr_in ret;
    memset(&ret, 0, sizeof(sockaddr_in));
    
    ret.sin_family = AF_INET;
    ret.sin_port = htons(address.port());
    
    ret.sin_addr.s_addr = address.host().empty() ? INADDR_ANY : inet_addr(address.host().c_str());
 
    if(ret.sin_addr.s_addr != INADDR_NONE) return some(ret);
    
    hostent *host = gethostbyname(address.host().c_str());
    if(!host) return none<sockaddr_in>();
  
    memcpy(&ret.sin_addr, host->h_addr, sizeof(ret.sin_addr));
    
    return some(ret);
  }
  
  template<typename T>
  void_result get_std_error(const T value)
  {
    if(value < 0) return failure(strerror(errno), errno);
    return success();
  }
  
  void_result get_std_error()
  {
    return failure(strerror(errno), errno);
  }
}

tcp_socket::tcp_socket()
{
  
}

tcp_socket::~tcp_socket()
{
  close();
}

void_result tcp_socket::open()
{
  return get_std_error(_fd = ::socket(AF_INET, SOCK_STREAM, 0));
}

void tcp_socket::close()
{
  if(_fd < 0) return;
  ::close(_fd);
  _fd = -1;
}

void_result tcp_socket::bind(const socket_address &address)
{
  if(_fd < 0) return failure("tcp_socket not open");
  
  option<sockaddr_in> addr = socket_address_to_sockaddr(address);
  if(!addr.some()) return get_std_error();
  
  return get_std_error(::bind(_fd, reinterpret_cast<const sockaddr *>(&addr.unwrap()), sizeof(sockaddr_in)));
}

void_result tcp_socket::connect(const socket_address &address)
{
  if(_fd < 0) return failure("tcp_socket not open");
  
  option<sockaddr_in> addr = socket_address_to_sockaddr(address);
  if(!addr.some()) return get_std_error();
  
  return get_std_error(::connect(_fd, reinterpret_cast<const sockaddr *>(&addr.unwrap()), sizeof(sockaddr_in)));
}

void_result tcp_socket::listen(const uint32_t queue_size)
{
  if(_fd < 0) return failure("tcp_socket not open");
  return get_std_error(::listen(_fd, queue_size));
}

result<tcp_socket *> tcp_socket::accept()
{
  sockaddr_in addr;
  socklen_t len;
  int fd = ::accept(_fd, reinterpret_cast<sockaddr *>(&addr), &len);
  if(fd < 0) return failure<tcp_socket *>(strerror(errno), errno);
  return success(new tcp_socket(fd));
}

result<bool> tcp_socket::blocking() const
{
  if(_fd < 0) return failure("tcp_socket not open");
  int flags = fcntl(_fd, F_GETFL, 0);
  if(flags < 0) return get_std_error();
  return success<bool>(!(flags & O_NONBLOCK));
}

void_result tcp_socket::set_blocking(const bool blocking)
{
   if(_fd < 0) return failure("tcp_socket not open");

#ifdef WIN32
   uint64_t mode = blocking ? 0 : 1;
   return ioctlsocket(_fd, FIONBIO, &mode) == 0;
#else
   int flags = fcntl(_fd, F_GETFL, 0);
   if(flags < 0) return get_std_error();
   flags = blocking ? flags & ~O_NONBLOCK : flags | O_NONBLOCK;
   return fcntl(_fd, F_SETFL, flags) == 0 ? success() : get_std_error();
#endif
}

result<size_t> tcp_socket::send(const uint8_t *const data, const size_t size, const comm_flags flags)
{
  ssize_t ret = ::send(_fd, data, size, flags == comm_peek ? MSG_PEEK : 0);
  if(ret < 0) return failure<size_t>(strerror(errno), errno);
  return success<size_t>(ret);
}

result<size_t> tcp_socket::recv(uint8_t *const data, const size_t size, const comm_flags flags)
{
  ssize_t ret = ::recv(_fd, data, size, flags == comm_peek ? MSG_PEEK : 0);
  if(ret < 0) return failure<size_t>(strerror(errno), errno);
  return success<size_t>(ret);
}

option<uint16_t> tcp_socket::port() const
{
  if(_fd < 0) return none<uint16_t>();
  socklen_t len = 0;
  sockaddr_in addr;
  getsockname(_fd, reinterpret_cast<sockaddr *>(&addr), &len);
  return some(addr.sin_port);
}

tcp_socket::tcp_socket(int fd)
  : _fd(fd)
{
}
