#include "daylite/spinner.hpp"

#include "spinner_impl.hpp"

#include <iostream>

using namespace daylite;
using namespace std;

spinett::spinett()
{
  spinner_impl::ref().register_spinett(this);
}

spinett::~spinett()
{
  spinner_impl::ref().unregister_spinett(this);
}

void_result spinner::spin()
{
  return spinner_impl::ref().spin();
}

void_result spinner::spin_once()
{
  return spinner_impl::ref().spin_once();
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
    if(!sp) continue;
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
  for(auto it = _spinetts.begin(); it != _spinetts.end(); ++it)
  {
    if(*it) continue;
    _spinetts.erase(it);
    break;
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
  for(auto &spinett : _spinetts)
  {
    if(spinett != sp) continue;
    spinett = 0;
  }
  
  return success();
}

spinner_impl::spinner_impl()
{
  
}
