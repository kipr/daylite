file(GLOB LIBDAYLITE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(daylite SHARED ${LIBDAYLITE_SOURCES})

option(test "Build tests" ON)

if(test)
  add_subdirectory(test)
endif()

add_subdirectory(examples)