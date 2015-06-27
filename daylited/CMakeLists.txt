file(GLOB DAYLIGHTD_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(daylited ${DAYLIGHTD_SOURCES})

target_link_libraries(daylited daylite)

include_directories("${CMAKE_SOURCE_DIR}/libdaylite/include" ${LIBBSON_INCLUDE_DIR})

target_link_libraries(daylited ${LIBBSON_LIBRARY})