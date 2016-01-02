#ifndef _DAYLITE_RESULT_HPP_
#define _DAYLITE_RESULT_HPP_

#include <cstdint>
#include <new>
#include <string>
#include <cstring>

#include "daylite/compat.hpp"

namespace daylite
{
  template<typename T>
  class result
  {
  public:
    template<typename D>
    inline result(const D &ret, const std::string &message, const uint32_t code = uint32_t())
      : _success(true)
      , _message(message)
      , _code(code)
    {
      new (_ret) T(ret);
    }
    
    inline result(const std::string &message = std::string(), const uint32_t code = uint32_t())
      : _success(false)
      , _message(message)
      , _code(code)
    {
    }
    
    template<typename D>
    result(const result<D> &rhs)
      : _success(rhs._success)
      , _message(rhs._message)
      , _code(rhs._code)
    {
      if(!_success) return;
      
      new (_ret) T(rhs.unwrap());
    }
    
    result(const result<T> &rhs)
      : _success(rhs._success)
      , _message(rhs._message)
      , _code(rhs._code)
    {
      if(!_success) return;
      
      new (_ret) T(rhs.unwrap());
    }
    
    result(result<T> &&rhs)
      : _success(rhs._success)
      , _message(std::move(rhs._message))
      , _code(rhs._code)
    {
      if(!_success) return;
      
      memcpy(_ret, rhs._ret, sizeof(T));
      
      // Ensure we won't call _ret's destructor in rhs
      rhs._success = false;
    }
    
    ~result()
    {
      if(!_success) return;
      
      reinterpret_cast<T *>(_ret)->~T();
    }
    
    inline bool success() const { return _success; }
    inline bool failure() const { return !_success; }
    
    inline const T &unwrap() const { return *reinterpret_cast<const T *>(_ret); }
    inline const T &or_else(const T &t) const { return _success ? unwrap() : t; }
    
    inline const std::string &message() const { return _message; }
    inline uint32_t code() const { return _code; }
    
    inline operator bool() const { return _success; }
    
    inline result<bool> to_void_result() const { return result<bool>(_success, _message); }
    
    template<typename D>
    result<T> &operator =(const result<D> &rhs)
    {
      if(_success) reinterpret_cast<T *>(_ret)->~T();
      
      _success = rhs._success;
      _message = rhs._message;
      _code = rhs._code;
      
      if(_success) new (_ret) T(rhs.unwrap());
      
      return *this;
    }
    
    result<T> &operator =(result<T> &&rhs)
    {
      if(_success) reinterpret_cast<T *>(_ret)->~T();
      
      _success = rhs._success;
      _message = std::move(rhs._message);
      _code = rhs._code;
      
      if(_success) memcpy(_ret, rhs._ret, sizeof(T));
      
      rhs._success = false;
      
      return *this;
    }
    
  private:
    bool _success;
    std::string _message;
    uint32_t _code;
    ALIGNED(uint8_t _ret[sizeof(T)], 8);
  };
  
  typedef result<bool> void_result;
  
  inline static void_result success(const std::string &message = std::string(), const uint32_t code = uint32_t())
  {
    return void_result(true, message, code);
  }
  
  inline static void_result failure(const std::string &message, const uint32_t code = uint32_t())
  {
    return void_result(message, code);
  }
  
  template<typename T>
  inline static result<T> success(const T &ret, const std::string &message = std::string(), const uint32_t code = uint32_t())
  {
    return result<T>(ret, message, code);
  }
  
  template<typename T>
  inline static result<T> success(const T &&ret, const std::string &message = std::string(), const uint32_t code = uint32_t())
  {
    return result<T>(ret, message, code);
  }
  
  template<typename T>
  inline static result<T> failure(const std::string &message, const uint32_t code = uint32_t())
  {
    return result<T>(message, code);
  }
}

#endif
