#include <daylite/util.hpp>

using namespace daylite;

#include <signal.h>

static bool exit__ = false;

static void sig(int n)
{
  exit__ = true;
}

__attribute__((constructor)) static void register_signal_handler()
{
  signal(SIGINT, sig);
}



bool daylite::should_exit()
{
  return exit__;
}