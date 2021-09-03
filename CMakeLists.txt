cmake_minimum_required(VERSION 3.10)
project(Studio)

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.3)
    message(FATAL_ERROR "GCC version must be at least 9.3")
endif()

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 2)
set(Boost_USE_STATIC_LIBS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BGFX_BUILD_TOOLS ON)
set(BGFX_BUILD_EXAMPLES OFF)

find_package(PkgConfig)
pkg_search_module(Eigen3 REQUIRED eigen3)
find_package(glfw3 REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)


include_directories(include ${eigen3_INCLUDE_DIRS} ${boost_INCLUDE_DIRS})
add_subdirectory(submodules/bgfx)
include(cmake/shaders.cmake)

include_directories("submodules/bgfx/bgfx/examples")
include_directories("submodules/bgfx/bgfx/3rdparty")
link_directories(${Boost_LIBRARY_DIRS})

file(GLOB_RECURSE SOURCE_FILES src/*.cc)
file(GLOB_RECURSE BGFX_COMMON submodules/bgfx/bgfx/examples/common/font/*.cpp
  submodules/bgfx/bgfx/examples/common/cube_atlas.cpp)
add_executable(studio ${SOURCE_FILES} ${BGFX_COMMON} apps/desktop.cc)
target_compile_options(studio PRIVATE -Wall)
target_link_libraries(studio PRIVATE bgfx bx bimg glfw ${eigen3_LIBRARIES} ${OpenMP_CXX_LIBRARY} ${Boost_LIBRARIES})

add_executable(preview ${SOURCE_FILES} ${BGFX_COMMON} apps/preview.cc)
target_compile_options(preview PRIVATE -Wall)
target_link_libraries(preview PRIVATE bgfx bx bimg glfw ${eigen3_LIBRARIES} ${OpenMP_CXX_LIBRARY} ${Boost_LIBRARIES})

file(GLOB_RECURSE SHADER_FILES shaders/fs_*.sc shaders/vs_*.sc)
add_shaders(studio SHADERS ${SHADER_FILES})

add_dependencies(studio shaders)
add_dependencies(preview shaders)

enable_testing()
add_subdirectory(test EXCLUDE_FROM_ALL)

install(TARGETS studio DESTINATION bin)
install(DIRECTORY ${CMAKE_BINARY_DIR}/compiled_shaders DESTINATION share/stray)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets DESTINATION share/stray)
install(TARGETS preview DESTINATION bin)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
include(CPack)

