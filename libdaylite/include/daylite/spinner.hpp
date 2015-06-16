#ifndef _DAYLITE_SPINNER_HPP_
#define _DAYLITE_SPINNER_HPP_

#include "compat.hpp"
#include "result.hpp"

#include <vector>

namespace daylite
{
  class DLL_EXPORT spinett
  {
  public:
    spinett();
    virtual ~spinett();
    
    virtual void_result spin_update() = 0;
  };

  // DLL-Export STL templates (MS Article 168958)
  EXPIMP_TEMPLATE template class DLL_EXPORT std::vector<spinett *>;
  
  class DLL_EXPORT spinner
  {
  public:
    friend class spinett;
    
    static spinner &ref();
    
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
    spinner();
    
    void_result register_spinett(spinett *const sp);
    void_result unregister_spinett(spinett *const sp);

    std::vector<spinett *> _spinetts;
    
    static spinner _singleton;
  };
}

#endif