
include(CTest)
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIR})

file(GLOB TEST_FILES *.cc)
file(GLOB_RECURSE LIB_FILES ${CMAKE_SOURCE_DIR}/src/*.cc)
file(GLOB_RECURSE BGFX_COMMON ${CMAKE_SOURCE_DIR}/submodules/bgfx/bgfx/examples/common/font/*.cpp
  ${CMAKE_SOURCE_DIR}/submodules/bgfx/bgfx/examples/common/cube_atlas.cpp)

add_library(shared_lib ${LIB_FILES} ${BGFX_COMMON})
target_link_libraries(shared_lib bgfx bx bimg glfw ${eigen3_LIBRARIES} ${Boost_LIBRARIES} ${EXTRA_LIBS})

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

