cmake_minimum_required(VERSION 3.12)
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
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BGFX_BUILD_TOOLS ON)
set(BGFX_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WIN32 OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )

add_subdirectory(submodules/bgfx)
add_subdirectory(submodules/glfw)

find_package(PkgConfig)
pkg_search_module(Eigen3 REQUIRED eigen3)
find_package(OpenMP REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)


if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

if (APPLE)
  find_library(COCOA_LIBRARY Cocoa)
  find_library(CF_LIBRARY CoreFoundation)
  find_library(IOKIT_LIBRARY IOKit)
  set(EXTRA_LIBS ${COCOA_LIBRARY} ${CF_LIBRARY} ${IOKIT_LIBRARY})
endif(APPLE)

include_directories(include ${GLFW_INCLUDE_DIRS} ${eigen3_INCLUDE_DIRS} ${boost_INCLUDE_DIRS})
include(cmake/shaders.cmake)

include_directories("submodules/bgfx/bgfx/examples")
include_directories("submodules/bgfx/bgfx/3rdparty")
link_directories(${Boost_LIBRARY_DIRS})

file(GLOB_RECURSE SOURCE_FILES src/*.cc)
file(GLOB_RECURSE BGFX_COMMON submodules/bgfx/bgfx/examples/common/font/*.cpp
  submodules/bgfx/bgfx/examples/common/cube_atlas.cpp)
add_executable(studio ${SOURCE_FILES} ${BGFX_COMMON} apps/desktop.cc)
target_compile_options(studio PRIVATE -Wall)
target_link_libraries(studio PRIVATE bgfx bx bimg glfw ${eigen3_LIBRARIES} ${OpenMP_CXX_LIBRARY} ${Boost_LIBRARIES} ${EXTRA_LIBS})

add_executable(preview ${SOURCE_FILES} ${BGFX_COMMON} apps/preview.cc)
target_compile_options(preview PRIVATE -Wall)
target_link_libraries(preview PRIVATE bgfx bx bimg glfw ${eigen3_LIBRARIES} ${OpenMP_CXX_LIBRARY} ${Boost_LIBRARIES} ${EXTRA_LIBS})

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

