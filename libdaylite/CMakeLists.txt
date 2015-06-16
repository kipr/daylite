################################################################################
#   libdaylite CMake Build File                                                #
################################################################################

set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB LIBDAYLITE_INCLUDES ${INCLUDE}/daylite/*)
file(GLOB LIBDAYLITE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*")

# for the dllimport/dllexport switch
add_definitions(-DBUILD_DAYLITE_DLL)


################################################################################
#   Include/link directories                                                   #
################################################################################

include_directories(${INCLUDE})

add_library(daylite SHARED ${LIBDAYLITE_SOURCES} ${LIBDAYLITE_INCLUDES})

if(WIN32)
  set(LIBRARIES ws2_32.lib Mswsock.lib AdvApi32.lib)
endif()

target_link_libraries(daylite ${LIBRARIES})

################################################################################
#   Tests                                                                      #
################################################################################
option(test "Build tests" ON)

if(test)
  add_subdirectory(test)
endif()

################################################################################
#   Examples                                                                   #
################################################################################
option(examples "Build examples" ON)
if(examples)
  add_subdirectory(examples)
endif()