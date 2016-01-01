#ifndef _DAYLITE_BSON_HPP_
#define _DAYLITE_BSON_HPP_

#include <memory>
#include <bson.h>

namespace daylite
{
  class bson
  {
  public:
    bson(bson_t *const raw, bool owns = true);
    
    // force copy constructor for const ptrs (same as owns = false)
    bson(const bson_t *const raw);
    
    bson();
    bson(const bson &rhs);
    ~bson();
    
    bson &operator =(const bson &rhs);
    
    inline bson_t *raw() { return _raw.get(); }
    inline const bson_t *raw() const { return _raw.get(); }
    
    inline operator bson_t *() { return _raw.get(); }
    inline operator const bson_t *() const { return _raw.get(); }
    
    inline const bson_t *operator ->() const { return _raw.get(); }
    inline bson_t *operator ->() { return _raw.get(); }
    
  private:
    std::shared_ptr<bson_t> _raw;
  };
}

#endif