#ifndef _DAYLITE_TOPIC_HPP_
#define _DAYLITE_TOPIC_HPP_

#include <string>

namespace daylite
{
  class topic
  {
  public:
    topic(const std::string &name);
    ~topic();
    
    inline const std::string &name() const { return _name; }
    
  private:
    std::string _name;
  };
}

#endif