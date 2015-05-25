find_package(GTest REQUIRED)
include_directories("${GTEST_INCLUDE_DIR}")

enable_testing()

function(make_test SOURCE)
  get_filename_component(NAME_ "${SOURCE}" NAME)
  add_executable("${NAME_}.test" "${SOURCE}")
  
  target_link_libraries("${NAME_}.test" "${GTEST_LIBRARY}" daylite)
  add_test(NAME "${NAME_}" COMMAND "${NAME_}.test")
endfunction()

file(GLOB TESTS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
foreach(TEST_ ${TESTS})
  make_test("${TEST_}")
endforeach()
