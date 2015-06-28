#include "tcp_socket.hpp"
#include "daylite/option.hpp"

#include <cstring>

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#define NOMINMAX
#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>
#include <windows.h>
typedef LONG_PTR ssize_t;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "console.hpp"

using namespace daylite;

namespace
{
#ifdef WIN32
  void update_errno()
  {
    switch(WSAGetLastError())
    {
    case 0:
      break;

    case 10035: /* WSAEWOULDBLOCK */
      errno = EAGAIN;
      break;

    default:
      errno = EOTHER;
      break;
    }
  }
#else
  #define update_errno() do{}while(0)
#endif

  option<sockaddr_in> socket_address_to_sockaddr(const socket_address &address)
  {
    sockaddr_in ret;
    memset(&ret, 0, sizeof(sockaddr_in));
    
    ret.sin_family = AF_INET;
    ret.sin_port = htons(address.port());
    
    ret.sin_addr.s_addr = address.host().empty() ? INADDR_ANY : inet_addr(address.host().c_str());
 
    if(ret.sin_addr.s_addr != INADDR_NONE) return some(ret);
    
#ifdef WIN32
    struct addrinfo *result = NULL;
    auto ret_val = getaddrinfo(address.host().c_str(), NULL, NULL, &result);
    if(ret_val != 0)
    {
      freeaddrinfo(result);
      return none<sockaddr_in>();
    }

    if (NULL == result)
    {
      return none<sockaddr_in>();
    }

    memcpy(&ret.sin_addr, result->ai_addr, sizeof(ret.sin_addr));
    freeaddrinfo(result);
#else
    hostent *host = gethostbyname(address.host().c_str());
    if(!host) return none<sockaddr_in>();
  
    memcpy(&ret.sin_addr, host->h_addr, sizeof(ret.sin_addr));
#endif
    
    return some(ret);
  }
  
  option<socket_address> sockaddr_to_socket_address(const sockaddr_in &address)
  {
    return some(socket_address(inet_ntoa(address.sin_addr), address.sin_port));
  }

  void_result get_std_error()
  {
    update_errno();
    return failure(strerror(errno), errno);
  }
  
  template<typename T>
  void_result get_std_error(const T value)
  {
    if(value < 0) return get_std_error();
    return success();
  }
}

tcp_socket::tcp_socket()
  : _associated_address(none<socket_address>())
  , _fd(-1)
#ifdef WIN32
  , _blocking(true)
#endif
{
#ifdef WIN32
  WSADATA wsa;
  auto error = WSAStartup(MAKEWORD(2, 2), &wsa);
  if(error)
  {
    DAYLITE_ERROR_STREAM("WSAStartup failed! Code: " << error);
  }
#endif
}

tcp_socket::~tcp_socket()
{
  close();

#ifdef WIN32
  WSACleanup();
#endif
}

void_result tcp_socket::open()
{
  void_result res = get_std_error(_fd = ::socket(AF_INET, SOCK_STREAM, 0));
  if(res) setup_socket();
  return res;
}

void tcp_socket::close()
{
  if(_fd < 0) return;
#ifdef WIN32
  closesocket(_fd);
  _blocking = true;
#else
  ::close(_fd);
#endif
  _fd = -1;
  _associated_address = none<socket_address>();
}

void_result tcp_socket::bind(const socket_address &address)
{
  if(!is_open()) return failure("tcp_socket not open");
  
  option<sockaddr_in> addr = socket_address_to_sockaddr(address);
  if(!addr.some()) return get_std_error();
  
  void_result ret = get_std_error(::bind(_fd, reinterpret_cast<const sockaddr *>(&addr.unwrap()), sizeof(sockaddr_in)));
  if(ret) _associated_address = some(address);
  return ret;
}

void_result tcp_socket::connect(const socket_address &address)
{
  if(!is_open()) return failure("tcp_socket not open");
  
  option<sockaddr_in> addr = socket_address_to_sockaddr(address);
  if(!addr.some()) return get_std_error();
  
  void_result ret = get_std_error(::connect(_fd, reinterpret_cast<const sockaddr *>(&addr.unwrap()), sizeof(sockaddr_in)));
  if(ret) _associated_address = some(address);
  return ret;
}

void_result tcp_socket::listen(const uint32_t queue_size)
{
  if(!is_open()) return failure("tcp_socket not open");
  return get_std_error(::listen(_fd, queue_size));
}

result<tcp_socket *> tcp_socket::accept()
{
  sockaddr_in addr;
  socklen_t len = sizeof(sockaddr);
  int fd = ::accept(_fd, reinterpret_cast<sockaddr *>(&addr), &len);
  if(fd < 0)
  {
    update_errno();
    return failure<tcp_socket *>(strerror(errno), errno);
  }
  return success(new tcp_socket(fd, sockaddr_to_socket_address(addr)));
}

result<bool> tcp_socket::blocking() const
{
  if (!is_open()) return failure("tcp_socket not open");

#ifdef WIN32
  return success<bool>(_blocking);
#else
  int flags = fcntl(_fd, F_GETFL, 0);
  if(flags < 0) return get_std_error();
  return success<bool>(!(flags & O_NONBLOCK));
#endif
}

void_result tcp_socket::set_blocking(const bool blocking)
{
   if(!is_open()) return failure("tcp_socket not open");

#ifdef WIN32
   u_long mode = blocking ? 0 : 1;
   _blocking = blocking;
   return success<bool>(ioctlsocket(_fd, FIONBIO, &mode) == 0);
#else
   int flags = fcntl(_fd, F_GETFL, 0);
   if(flags < 0) return get_std_error();
   flags = blocking ? flags & ~O_NONBLOCK : flags | O_NONBLOCK;
   return fcntl(_fd, F_SETFL, flags) == 0 ? success() : get_std_error();
#endif
}

result<size_t> tcp_socket::send(const uint8_t *const data, const size_t size, const comm_flags flags)
{
  int additional_flags = 0;
#if __linux__
  additional_flags |= MSG_NOSIGNAL;
#endif
  ssize_t ret = ::send(_fd, (char*)data, size, (flags == comm_peek ? MSG_PEEK : 0) | additional_flags);
  if(ret < 0) return failure<size_t>(strerror(errno), errno);
  return success<size_t>(ret);
}

result<size_t> tcp_socket::recv(uint8_t *const data, const size_t size, const comm_flags flags)
{
  int additional_flags = 0;
#if __linux__
  additional_flags |= MSG_NOSIGNAL;
#endif
  ssize_t ret = ::recv(_fd, (char *)data, size, (flags == comm_peek ? MSG_PEEK : 0) | additional_flags);
  if(ret < 0)
  {
    update_errno();
    return failure<size_t>(strerror(errno), errno);
  }
  return success<size_t>(ret);
}

option<uint16_t> tcp_socket::port() const
{
  if(!is_open()) return none<uint16_t>();
  socklen_t len = 0;
  sockaddr_in addr;
  getsockname(_fd, reinterpret_cast<sockaddr *>(&addr), &len);
  return some(addr.sin_port);
}

bool tcp_socket::is_open() const
{
  return _fd >= 0;
}

tcp_socket::tcp_socket(int fd, const option<socket_address> &assoc)
  : _fd(fd)
  , _associated_address(assoc)
{
  setup_socket();
}

void tcp_socket::setup_socket()
{
#ifndef __linux__
  int set = 1;
  setsockopt(_fd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif
  set_blocking(false);
}