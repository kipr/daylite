project(daylite)

cmake_minimum_required(VERSION 2.8.12)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(NOT WIN32)
  add_definitions(-std=c++1y)
endif()

if(APPLE)
  add_definitions(-Wno-deprecated-register)
endif()

if(NOT WIN32)
  option(debug "Build in debug mode" ON)
  if(debug)
    add_definitions(-g -pg)
  else()
    add_definitions(-DNDEBUG -O3)
  endif()
endif()

################################################################################
#   Find boost                                                                 #
################################################################################

if(WIN32)
  find_path(BOOST_INCLUDE_DIR boost PATHS "C:/Program Files/boost/boost_1_55_0")
endif()

add_subdirectory(libdaylite)
add_subdirectory(daylited)



option(tools "Build daylite tools" ON)
if(tools)
  add_subdirectory(tools)
endif()
