find_program(BSON_BIND_EXECUTABLE bson_bind)

function(add_bson_bind INCLUDES PATH_)
  get_filename_component(NAME ${PATH_} NAME_WE)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.hpp
    COMMAND ${BSON_BIND_EXECUTABLE} ${PATH_} ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.hpp
                     DEPENDS ${PATH_}
                     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
  set(${INCLUDES} ${${INCLUDES}} ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.hpp PARENT_SCOPE)
endfunction()

macro(add_bson_binds INCLUDES)
  foreach(PATH_ ${ARGN})
    add_bson_bind(${INCLUDES} ${PATH_})
  endforeach()
endmacro()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BSON_BIND DEFAULT_MSG BSON_BIND_EXECUTABLE)
mark_as_advanced(BSON_BIND_EXECUTABLE)
