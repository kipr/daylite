#include <daylite/util.hpp>

using namespace daylite;

#include <signal.h>

static bool exit__ = false;

static void sig(int n)
{
  exit__ = true;
}

void daylite::register_signal_handlers()
{
  signal(SIGINT, sig); 
}


bool daylite::should_exit()
{
  return exit__;
}