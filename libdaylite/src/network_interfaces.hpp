#ifndef _DAYLITE_NETWORK_INTERFACES_HPP_
#define _DAYLITE_NETWORK_INTERFACES_HPP_

#include <string>
#include <vector>
#include "network_interface.hpp"

namespace daylite
{
  class network_interfaces
  {
  public:
    static std::vector<network_interface> interfaces();
  };
}

#endif
