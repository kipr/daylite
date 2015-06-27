#ifndef _DAYLITE_OPTION_HPP_
#define _DAYLITE_OPTION_HPP_

#include <cstdint>
#include <new>

namespace daylite
{
  template<typename T>
  class option
  {
  public:
    template<typename D>
    option(const D &d)
      : _some(true)
    {
      new (_t) T(d);
    }
    
    option()
      : _some(false)
    {
    }
    
    template<typename D>
    option(const option<D> &rhs)
      : _some(rhs._some)
    {
      if(!_some) return;
      
      new (_t) T(rhs.unwrap());
    }
    
    option(const option<T> &rhs)
      : _some(rhs._some)
    {
      if(!_some) return;
      
      new (_t) T(rhs.unwrap());
    }
    
    option(option<T> &&rhs)
      : _some(rhs._some)
    {
      if(!_some) return;
      
      memcpy(_t, rhs._t, sizeof(T));
      rhs._some = false;
    }
    
    ~option()
    {
      if(!_some) return;
      reinterpret_cast<T *>(_t)->~T();
    }
    
    const T &unwrap() const
    {
      // exception if !_some
      return *reinterpret_cast<const T *>(_t);
    }
    
    const T &or_else(const T &rhs) const
    {
      if(_some) return *reinterpret_cast<const T *>(_t);
      return rhs;
    }
    
    inline bool some() const { return _some; }
    inline bool none() const { return !_some; }
    
    template<typename D>
    option<T> &operator =(const option<D> &rhs)
    {
      if(_some) reinterpret_cast<T *>(_t)->~T();
      _some = rhs._some;
      if(_some) new (_t) T(rhs.unwrap());
      return *this;
    }
    
    option<T> &operator =(option<T> &&rhs)
    {
      if(_some) reinterpret_cast<T *>(_t)->~T();
      _some = rhs._some;
      if(_some) memcpy(_t, rhs._t, sizeof(T));
      rhs._some = false;
      return *this;
    }
    
  private:
    bool _some;
    uint8_t _t[sizeof(T)];
  };
  
  template<typename T>
  inline option<T> some(const T &t) { return option<T>(t); }
  
  template<typename T>
  inline option<T> none() { return option<T>(); }
}

#endif