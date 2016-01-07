################################################################################
#   libdaylite CMake Build File                                                #
################################################################################

set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB LIBDAYLITE_INCLUDES ${INCLUDE}/daylite/*)
file(GLOB LIBDAYLITE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*")
file(GLOB LIBDAYLITE_MSGS "${CMAKE_CURRENT_SOURCE_DIR}/msgs/*")

if(WIN32)
  file(GLOB LIBDAYLITE_WINDOWS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src_win/*")
  set(LIBDAYLITE_SOURCES ${LIBDAYLITE_SOURCES} ${LIBDAYLITE_WINDOWS_SOURCES})
endif()

find_package(BsonBind REQUIRED)
find_package(BSON REQUIRED)

# add_definitions(-DSPLAT_ENABLED)

add_bson_binds(BSON_BIND_INCLUDES ${LIBDAYLITE_MSGS})
list(APPEND LIBDAYLITE_INCLUDES ${BSON_BIND_INCLUDES})

################################################################################
#   Include/link directories                                                   #
################################################################################

include_directories(${INCLUDE} ${BSON_INCLUDE_DIR} ${BSON_BIND_INCLUDE_DIR}
                    ${BOOST_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_library(daylite SHARED ${LIBDAYLITE_SOURCES} ${LIBDAYLITE_INCLUDES})

if(WIN32)
  set(LIBRARIES ws2_32.lib Mswsock.lib AdvApi32.lib)
endif()

target_link_libraries(daylite ${LIBRARIES} ${BSON_LIBRARY})


# for the dllimport/dllexport switch
set_target_properties(daylite PROPERTIES COMPILE_FLAGS -DBUILD_DAYLITE_DLL)
################################################################################
#   Tests                                                                      #
################################################################################
option(test "Build tests" OFF)

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

################################################################################
#   Install                                                                    #
################################################################################
install(FILES ${LIBDAYLITE_INCLUDES} DESTINATION include/daylite)

if(WIN32)
  install(TARGETS daylite
          ARCHIVE DESTINATION lib
          RUNTIME DESTINATION bin)
else()
  install(TARGETS daylite LIBRARY DESTINATION lib)
endif()
