cmake_minimum_required(VERSION 3.10)
project(LabelStudio)

find_package(glfw3 REQUIRED)
find_package(eigen3 REQUIRED)
find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BGFX_BUILD_TOOLS ON)
set(BGFX_BUILD_EXAMPLES OFF)

include_directories(include ${eigen3_INCLUDE_DIRS})
add_subdirectory(submodules/bgfx)
include(cmake/shaders.cmake)

file(GLOB SOURCE_FILES src/**/*.cc src/*.cc)
add_executable(main ${SOURCE_FILES} apps/desktop.cc)
target_link_libraries(main bgfx bx bimg glfw ${eigen3_LIBRARIES} OpenMP::OpenMP_CXX)

add_shaders(main SHADERS mesh/vs_mesh.sc mesh/fs_mesh.sc)

enable_testing()
add_subdirectory(test EXCLUDE_FROM_ALL)


