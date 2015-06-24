################################################################################
#   libdaylite CMake Build File                                                #
################################################################################

set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB LIBDAYLITE_INCLUDES ${INCLUDE}/daylite/*)
file(GLOB LIBDAYLITE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*")

################################################################################
#   Find libbson                                                               #
################################################################################

if(WIN32)
  find_file(LIBBSON_DLL libbson-1.0.dll PATHS "${PROJECT_SOURCE_DIR}/../libbson/build/install/bin" NO_DEFAULT_PATH)
  find_path(LIBBSON_INCLUDE_DIR bson.h PATHS "${PROJECT_SOURCE_DIR}/../libbson/build/install/include/libbson-1.0" NO_DEFAULT_PATH)
  find_library(LIBBSON_LIBRARY bson-1.0.lib PATHS "${PROJECT_SOURCE_DIR}/../libbson/build/install/lib" NO_DEFAULT_PATH)
endif()

################################################################################
#   Include/link directories                                                   #
################################################################################

include_directories(${INCLUDE} ${LIBBSON_INCLUDE_DIR})

add_library(daylite SHARED ${LIBDAYLITE_SOURCES} ${LIBDAYLITE_INCLUDES})

if(WIN32)
  set(LIBRARIES ws2_32.lib Mswsock.lib AdvApi32.lib)
endif()

target_link_libraries(daylite ${LIBRARIES} ${LIBBSON_LIBRARY})

# for the dllimport/dllexport switch
set_target_properties(daylite PROPERTIES COMPILE_FLAGS -DBUILD_DAYLITE_DLL)
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