#ifndef _DAYLITE_CONSOLE_HPP_
#define _DAYLITE_CONSOLE_HPP_

#include <iostream>

#ifdef WIN32
  #ifdef _DEBUG
    #define LIBDAYLITE_DEBUG_BUILD
  #endif
#else
  // TOOD: Check how non-VS compiler differentiate between Debug and Release build
  #define LIBDAYLITE_DEBUG_BUILD
#endif

#ifdef LIBDAYLITE_DEBUG_BUILD
#define DAYLITE_DEBUG_STREAM(args) DAYLITE_PRINT_STREAM(__FILE__ << ":" << __LINE__ << "> " << args << std::endl)
#else
#define DAYLITE_DEBUG_STREAM(args) do {} while(0)
#endif

#define DAYLITE_WARNING_STREAM(args) DAYLITE_PRINT_STREAM(__FILE__ << ":" << __LINE__ << "> " << args << std::endl)
#define DAYLITE_ERROR_STREAM(args) DAYLITE_PRINT_STREAM(__FILE__ << ":" << __LINE__ << "> " << args << std::endl)

#define DAYLITE_PRINT_STREAM(args) do {std::cerr << args;} while(0)

#endif