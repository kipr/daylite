
find_package(BSON REQUIRED)

include_directories("${CMAKE_SOURCE_DIR}/libdaylite/include" ${BSON_INCLUDE_DIR})

add_executable(dl_listen listen.cpp)
target_link_libraries(dl_listen daylite ${BSON_LIBRARY})
