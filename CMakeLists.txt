project(daylight)

cmake_minimum_required(VERSION 2.8.12)

add_definitions(-std=c++11)

option(debug "Build in debug mode" ON)
if(debug)
  add_definitions(-g -pg)
else()
  add_definitions(-DNDEBUG -O3)
endif()

add_subdirectory(libdaylite)
