
find_package(BSON REQUIRED)

file(GLOB DAYLIGHTD_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(daylited ${DAYLIGHTD_SOURCES})

target_link_libraries(daylited daylite)

include_directories("${CMAKE_SOURCE_DIR}/libdaylite/include" ${BSON_INCLUDE_DIR})

target_link_libraries(daylited ${BSON_LIBRARY})  

if(WIN32)
  # copy daylite.dll into build folder
  add_custom_command(TARGET daylited POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE_DIR:daylited>/../../libdaylite/$<CONFIG>/daylite.dll
        $<TARGET_FILE_DIR:daylited>)
        
  # copy libbson-1.0.dll into build folder
  add_custom_command(TARGET daylited POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${BSON_DLL}
        $<TARGET_FILE_DIR:daylited>)
endif()

install(TARGETS daylited RUNTIME DESTINATION bin)