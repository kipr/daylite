#ifndef _DAYLITE_TRANSPORT_HPP_
#define _DAYLITE_TRANSPORT_HPP_

#include "input_channel.hpp"
#include "output_channel.hpp"
#include "daylite/option.hpp"
#include "daylite/result.hpp"

namespace daylite
{
  class transport
  {
  public:
    virtual ~transport() {}
    
    virtual void_result open() = 0;
    virtual void_result close() = 0;
    
    virtual option<input_channel *>  input()  const = 0;
    virtual option<output_channel *> output() const = 0;
  };
}

#endif