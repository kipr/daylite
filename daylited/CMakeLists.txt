file(GLOB DAYLIGHTD_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(daylite ${DAYLIGHTD_SOURCES})