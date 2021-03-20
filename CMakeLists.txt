cmake_minimum_required(VERSION 3.10)
project(LabelStudio)

find_package(glfw3 REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BGFX_BUILD_TOOLS ON)
set(BGFX_BUILD_EXAMPLES OFF)

include_directories(include)
add_subdirectory(submodules/bgfx)

file(GLOB SOURCE_FILES src/**/*.cc src/*.cc)
add_executable(main ${SOURCE_FILES})
target_link_libraries(main bgfx bx bimg glfw)

