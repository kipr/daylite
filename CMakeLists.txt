project(daylite)

cmake_minimum_required(VERSION 2.8.12)

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
#   Find libbson                                                               #
################################################################################

if(WIN32)
  find_file(LIBBSON_DLL libbson-1.0.dll PATHS "${PROJECT_SOURCE_DIR}/../prefix/usr/bin")
  find_path(LIBBSON_INCLUDE_DIR bson.h PATHS "${PROJECT_SOURCE_DIR}/../prefix/usr/include/libbson-1.0")
  find_library(LIBBSON_LIBRARY bson-1.0.lib PATHS "${PROJECT_SOURCE_DIR}/../prefix/usr/lib")
elseif(UNIX)
  find_path(LIBBSON_INCLUDE_DIR bson.h PATHS "/usr/include/libbson-1.0" "/usr/local/include/libbson-1.0")
  find_library(LIBBSON_LIBRARY bson-1.0)
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
