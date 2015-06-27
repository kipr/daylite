#ifndef _DAYLITE_SPINNER_HPP_
#define _DAYLITE_SPINNER_HPP_

#include "compat.hpp"
#include "daylite/result.hpp"

namespace daylite
{
  namespace spinner
  {
    DLL_EXPORT void_result spin();
    DLL_EXPORT void_result spin_once();
  }
}

#endif