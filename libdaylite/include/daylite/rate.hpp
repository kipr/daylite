#ifndef _DAYLITE_RATE_HPP_
#define _DAYLITE_RATE_HPP_

#include <chrono>

namespace daylite
{
  class rate
  {
  public:
    inline rate(const std::chrono::milliseconds &duration)
      : _duration(duration), _last(std::chrono::steady_clock::now()) {}
    inline const std::chrono::steady_clock::time_point &last() const { return _last; }
    inline const std::chrono::milliseconds &duration() const { return _duration; }
    
    /**
     * Returns true when the given duration has elapsed.
     * This function simultaneously returns true and restarts the timeout.
     */
    inline bool should_fire()
    {
      using std::chrono::duration_cast;
      using std::chrono::milliseconds;
      using std::chrono::steady_clock;
      
      if(duration_cast<milliseconds>(steady_clock::now() - _last) <= _duration) return false;
      _last = steady_clock::now();
      return true;
    }
    
  private:
    std::chrono::steady_clock::time_point _last;
    std::chrono::milliseconds _duration;
  };
}

#endif