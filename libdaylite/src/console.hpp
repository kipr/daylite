#ifndef _DAYLITE_CONSOLE_HPP_
#define _DAYLITE_CONSOLE_HPP_

#include <iostream>

#define DAYLITE_DEBUG_STREAM(args) DAYLITE_PRINT_STREAM(__FILE__ << ":" << __LINE__ << "> " << args << std::endl)
#define DAYLITE_WARNING_STREAM(args) DAYLITE_PRINT_STREAM(__FILE__ << ":" << __LINE__ << "> " << args << std::endl)
#define DAYLITE_ERROR_STREAM(args) DAYLITE_PRINT_STREAM(__FILE__ << ":" << __LINE__ << "> " << args << std::endl)

#define DAYLITE_PRINT_STREAM(args) do {std::cerr << args;} while(0)

#endif