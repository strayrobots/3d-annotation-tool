
include(CTest)
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIR})

file(GLOB TEST_FILES *.cc)
file(GLOB LIB_FILES ${CMAKE_SOURCE_DIR}/src/**/*.cc ${CMAKE_SOURCE_DIR}/src/*.cc)

add_library(shared_lib ${LIB_FILES})
target_link_libraries(shared_lib bgfx bx bimg glfw ${eigen3_LIBRARIES})

add_custom_target(build_tests)
add_dependencies(build_tests shaders)

foreach(_test_file ${TEST_FILES})
  get_filename_component(_test_name ${_test_file} NAME_WE)
  add_executable(${_test_name} ${_test_file})
  target_link_libraries(${_test_name} ${GTEST_LIBRARIES} shared_lib)
  add_test(NAME ${_test_name} COMMAND ${_test_name} "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/dataset"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  add_dependencies(build_tests ${_test_name})
endforeach()

