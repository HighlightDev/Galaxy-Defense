cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME GalaxyDefenseProject)

project(${PROJECT_NAME} VERSION 1.0 LANGUAGES CXX)

option(BUILD_GALAXY_DEFENSE "Build GalaxyDefense game." ON)
option(BUILD_ENGINE_TEST "Build test scene." OFF)
option(BUILD_UBO_TEST "Build test UBO." OFF)

if(WIN32)
    if(MSVC)
        add_compile_options(/bigobj)
    else()
        add_compile_options(-Wa,-mbig-obj)
    endif()
endif(WIN32)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_x64 ON)
    message("Compile for x64 architecture.")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ARCH_x64 OFF)
    message("Compile for x86 architecture.")
endif()

if(UNIX)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++20" COMPILER_SUPPORTS_CXX20)
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX20)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
    elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++20 support. Please use a different C++ compiler.")
    endif()

    find_package(X11 REQUIRED)
    find_package(Threads REQUIRED)
    find_package(OpenAL REQUIRED)

elseif(WIN32)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

find_package(OpenGL REQUIRED)

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

if(BUILD_UBO_TEST)
    add_subdirectory(TestUBO)
endif(BUILD_UBO_TEST)
