file(GLOB EXAMPLES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(EXAMPLE ${EXAMPLES})
  get_filename_component(NAME_ "${EXAMPLE}" NAME)
  add_executable(${NAME_} ${EXAMPLE})
  target_link_libraries(${NAME_} daylite)
endforeach()