#include "daylite/spinner.hpp"

#include "spinner_impl.hpp"

using namespace daylite;

spinett::spinett()
{
  spinner_impl::ref().register_spinett(this);
}

spinett::~spinett()
{
  spinner_impl::ref().unregister_spinett(this);
}

bool spinner::spin()
{
  return  spinner_impl::ref().spin();
}

bool spinner::spin_once()
{
  return  spinner_impl::ref().spin_once();
}

spinner_impl daylite::spinner_impl::_singleton = spinner_impl();

spinner_impl &spinner_impl::ref()
{
  return _singleton;
}

void_result spinner_impl::spin_once()
{
  for(auto sp : _spinetts)
  {
    void_result ret = sp->spin_update();
    if(!ret) return ret;
  }

  // Add spinnets (we cannot add them to _spinnets directly as this might break the previous loop)
  if(_spinetts_to_add.size())
  {
    _spinetts.insert(_spinetts.end(), _spinetts_to_add.begin(), _spinetts_to_add.end());
    _spinetts_to_add.clear();
  }

  // Remove spinnets (we cannot remove them from _spinnets directly as this might break the previous loop)
  if(_spinetts_to_remove.size())
  {
    for(auto sp : _spinetts_to_remove)
    {
      for(auto it = _spinetts.begin(); it != _spinetts.end(); ++it)
      {
        if(*it != sp) continue;
        _spinetts.erase(it);
        break;
      }
    }
  }
  return success();
}

void_result spinner_impl::register_spinett(spinett *const sp)
{
  _spinetts_to_add.push_back(sp);
  return success();
}

void_result spinner_impl::unregister_spinett(spinett *const sp)
{
  _spinetts_to_remove.push_back(sp);
  return success();
  
  return failure("Spinett not found in spinner");
}

spinner_impl::spinner_impl()
{
  
}
