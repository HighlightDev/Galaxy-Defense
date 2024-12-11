cmake_minimum_required(VERSION 3.5)

#set(PROJECT_NAME TinyGame)
set(PROJECT_NAME GalaxyDefenseProject)

project(${PROJECT_NAME} VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

if(UNIX)
    find_package(X11 REQUIRED)
    find_package(Threads REQUIRED)
endif()

find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)

option(ENABLE_LIBUNWIND "Enables compilation of libunwind dependent code" OFF)

if(ENABLE_LIBUNWIND)
    add_compile_definitions(
        USE_LIBUNWIND
    )
endif(ENABLE_LIBUNWIND)

add_compile_definitions(
    GLM_ENABLE_EXPERIMENTAL
    BT_USE_DOUBLE_PRECISION
)

add_subdirectory(PHEngine/src)
add_subdirectory(GalaxyDefense)
add_subdirectory(Logger/TinyLogger)
if(ENABLE_LIBUNWIND)
    add_subdirectory(UnwindTool)
endif(ENABLE_LIBUNWIND)
