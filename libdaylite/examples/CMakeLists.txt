
find_package(BSON REQUIRED)

file(GLOB EXAMPLES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(EXAMPLE ${EXAMPLES})
  get_filename_component(NAME_ "${EXAMPLE}" NAME)
  add_executable(${NAME_} ${EXAMPLE})
  target_link_libraries(${NAME_} daylite ${BSON_LIBRARY})
  
  if(WIN32)
    # copy daylite.dll into build folder
    add_custom_command(TARGET ${NAME_} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          $<TARGET_FILE_DIR:${NAME_}>/../../$<CONFIG>/daylite.dll
          $<TARGET_FILE_DIR:${NAME_}>)
          
    # copy libbson-1.0.dll into build folder
    add_custom_command(TARGET ${NAME_} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${BSON_DLL}
          $<TARGET_FILE_DIR:${NAME_}>)
  endif()
  
endforeach()