cmake_minimum_required(VERSION 3.16.3)

message(STATUS "Available CMake version: ${CMAKE_VERSION}")

set(PROJECT_NAME GalaxyDefenseProject)

project(
  ${PROJECT_NAME}
  VERSION 1.0
  LANGUAGES CXX)

option(ENABLE_ASAN "Enable Address Sanitizer." OFF)
option(ENABLE_TSAN "Enable Thread Sanitizer." OFF)
option(PROFILER_BUILD "Build with debug symbols and no optimizations." OFF)
option(BUILD_GALAXY_DEFENSE "Build GalaxyDefense game." ON)
option(BUILD_ENGINE_TEST "Build test scene." ON)
option(ENABLE_LIBUNWIND "Enables compilation of libunwind dependent code" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(UNIX)
  add_compile_options(-Wno-interference-size)
elseif(WIN32)
  if(MSVC)
    add_compile_options(/bigobj)
    string(APPEND CMAKE_C_FLAGS " /utf-8")
    string(APPEND CMAKE_CXX_FLAGS " /utf-8")
    add_compile_options(/MP)
  else()
    add_compile_options(-Wa,-mbig-obj)
  endif()
endif()

add_subdirectory(PHEngine)
add_subdirectory(Logger/TinyLogger)

if(BUILD_GALAXY_DEFENSE)
  add_subdirectory(GalaxyDefense)
endif(BUILD_GALAXY_DEFENSE)

if(ENABLE_LIBUNWIND)
  add_subdirectory(UnwindTool)
endif(ENABLE_LIBUNWIND)

if(BUILD_ENGINE_TEST)
  add_subdirectory(EngineTest)
endif(BUILD_ENGINE_TEST)
