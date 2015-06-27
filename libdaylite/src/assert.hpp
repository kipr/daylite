#ifndef _DAYLITE_ASSERT_HPP_
#define _DAYLITE_ASSERT_HPP_

#include "daylite/result.hpp"
#include <iostream>
#include <cassert>

namespace daylite
{
  template<typename T>
  void assert_result_eq(const result<T> &result, const T &expected)
  {
    if(!result)
    {
      std::cerr << "RESULT ASSERTION ERROR (" << result.code() << "): " << result.message() << std::endl;
      abort();
    }
    
    assert(result.unwrap() == expected);
  }
  
  template<typename T>
  void assert_result(const result<T> &result)
  {
    if(result) return;
    
    std::cerr << "RESULT ASSERTION ERROR (" << result.code() << "): " << result.message() << std::endl;
    abort();
  }
}

#endif