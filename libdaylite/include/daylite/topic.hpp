#ifndef _DAYLITE_TOPIC_HPP_
#define _DAYLITE_TOPIC_HPP_

#include <string>

namespace daylite
{
  class topic
  {
  public:
    topic(const std::string &name) : _name(name) {}
    ~topic() {}
    
    inline const std::string &name() const { return _name; }
    
  private:
    std::string _name;
  };
  
  
  
  inline static bool operator ==(const topic &lhs, const topic &rhs)
  {
    return lhs.name() == rhs.name();
  }
}

namespace std
{
  template<>
  struct hash<daylite::topic>
  {
    inline size_t operator()(const daylite::topic &rhs) const { return hash<string>()(rhs.name()); }
  };
}

#endif