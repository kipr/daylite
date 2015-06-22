#ifndef _DAYLITE_SPINNER_IMPL_HPP_
#define _DAYLITE_SPINNER_IMPL_HPP_

#include "result.hpp"

#include <vector>

namespace daylite
{
  class spinett
  {
  public:
    spinett();
    virtual ~spinett();
    
    virtual void_result spin_update() = 0;
  };
  
  class spinner_impl 
  {
  public:
    friend class spinett;
    
    static spinner_impl &ref();
    
    inline void_result spin()
    {
      for(;;)
      {
        void_result ret = spin_once();
        if(!ret) return ret;
      }
      return success();
    }
    
    void_result spin_once();
    
  private:
    spinner_impl();
    
    void_result register_spinett(spinett *const sp);
    void_result unregister_spinett(spinett *const sp);

    std::vector<spinett *> _spinetts_to_add;
    std::vector<spinett *> _spinetts_to_remove;
    std::vector<spinett *> _spinetts;
    
    static spinner_impl _singleton;
  };
}

#endif