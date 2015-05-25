#ifndef _DAYLIGHT_SUBSCRIBER_HPP_
#define _DAYLIGHT_SUBSCRIBER_HPP_

namespace daylite
{
  class subscriber
  {
  public:
    subscriber(const topic &path);
    ~subscriber();
    
    
    
  private:
    topic _path;
  };
}

#endif