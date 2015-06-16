project(daylight)

cmake_minimum_required(VERSION 2.8.12)

if(NOT WIN32)
  add_definitions(-std=c++11)
endif()

if(NOT WIN32)
  option(debug "Build in debug mode" ON)
  if(debug)
    add_definitions(-g -pg)
  else()
    add_definitions(-DNDEBUG -O3)
  endif()
endif()

add_subdirectory(libdaylite)
