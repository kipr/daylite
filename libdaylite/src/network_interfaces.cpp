#include "network_interfaces.hpp"

#ifndef WIN32
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif

#include <iostream>

using namespace daylite;
using namespace std;

vector<network_interface> network_interfaces::interfaces()
{
  vector<network_interface> interfaces;

  ifaddrs *addr;
  getifaddrs(&addr);
  ifaddrs *head = addr;

  while(addr)
  {
    network_interface i;
    i.name = addr->ifa_name;
    auto broadcast = addr->ifa_broadaddr; 
    auto netmask = addr->ifa_netmask; 
    if(!broadcast || !netmask)
    {
      addr = addr->ifa_next;
      continue;
    }
    auto baddr = reinterpret_cast<sockaddr_in *>(broadcast)->sin_addr;
    auto maddr = reinterpret_cast<sockaddr_in *>(netmask)->sin_addr;
    i.address = inet_ntoa(baddr);
    i.netmask = inet_ntoa(maddr);
    interfaces.push_back(i);
    addr = addr->ifa_next;
  }

  if(head) freeifaddrs(head);

  return interfaces;
}
