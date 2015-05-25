#include "daylite/spinner.hpp"

using namespace daylite;

spinett::spinett()
{
  spinner::ref().register_spinett(this);
}

spinett::~spinett()
{
  spinner::ref().unregister_spinett(this);
}

spinner daylite::spinner::_singleton = spinner();

spinner &spinner::ref()
{
  return _singleton;
}

void_result spinner::spin_once()
{
  for(auto sp : _spinetts)
  {
    void_result ret = sp->spin_update();
    if(!ret) return ret;
  }
  return success();
}

void_result spinner::register_spinett(spinett *const sp)
{
  _spinetts.push_back(sp);
  return success();
}

void_result spinner::unregister_spinett(spinett *const sp)
{
  for(auto it = _spinetts.begin(); it != _spinetts.end(); ++it)
  {
    if(*it != sp) continue;
    _spinetts.erase(it);
    return success();
  }
  
  return failure("Spinett not found in spinner");
}

spinner::spinner()
{
  
}