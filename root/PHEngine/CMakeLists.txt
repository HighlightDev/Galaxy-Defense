set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(Dependencies)
include(Utility)
include(Flags)

set(LIB_NAME PHEngine)

set(BUILD_SHARED_LIBS
    OFF
    CACHE BOOL "Build dependencies as static libraries" FORCE)

# No debug postfix for dependency libraries
set(ASSIMP_INJECT_DEBUG_POSTFIX
    OFF
    CACHE BOOL "Overwrite assimp option" FORCE)
set(CMAKE_DEBUG_POSTFIX
    ""
    CACHE STRING "Debug postfix for libraries" FORCE)

message(STATUS "Configuring Assimp submodule")
set(ASSIMP_BUILD_TESTS
    OFF
    CACHE BOOL "Overwrite assimp option" FORCE)
set(ASSIMP_INSTALL
    OFF
    CACHE BOOL "Overwrite assimp option" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/assimp/)

message(STATUS "Configuring Bullet3 submodule")
set(USE_GRAPHICAL_BENCHMARK
    OFF
    CACHE BOOL "Overwrite bullet3 option" FORCE)
set(BUILD_CPU_DEMOS
    OFF
    CACHE BOOL "Overwrite bullet3 option" FORCE)
set(BUILD_UNIT_TESTS
    OFF
    CACHE BOOL "Overwrite bullet3 option" FORCE)
set(BUILD_BULLET2_DEMOS
    OFF
    CACHE BOOL "Overwrite bullet3 option" FORCE)
set(BUILD_OPENGL3_DEMOS
    OFF
    CACHE BOOL "Overwrite bullet3 option" FORCE)
# Engine and games are compiled with BT_USE_DOUBLE_PRECISION, bullet must match
set(USE_DOUBLE_PRECISION
    ON
    CACHE BOOL "Overwrite bullet3 option" FORCE)
# Use /MD(d) like the rest of the project instead of static CRT (/MT(d))
set(USE_MSVC_RUNTIME_LIBRARY_DLL
    ON
    CACHE BOOL "Overwrite bullet3 option" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/bullet3/)

message(STATUS "Configuring GLEW submodule")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/glew/)

message(STATUS "Configuring FreeType submodule")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/freetype/)

message(STATUS "Configuring NlohmannJson submodule")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/nlohmann_json/)

message(STATUS "Configuring Lua submodule")
set(LUA_ENABLE_TESTING
    OFF
    CACHE BOOL "Overwrite Lua option" FORCE)
set(LUA_ENABLE_SHARED
    OFF
    CACHE BOOL "Overwrite Lua option" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/lua/)

message(STATUS "Configuring ogg submodule")
set(INSTALL_DOCS
    OFF
    CACHE BOOL "Overwrite Ogg option" FORCE)
set(INSTALL_PKG_CONFIG_MODULE
    OFF
    CACHE BOOL "Overwrite Ogg option" FORCE)
set(INSTALL_CMAKE_PACKAGE_MODULE
    OFF
    CACHE BOOL "Overwrite Ogg option" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/ogg/)

message(STATUS "Configuring vorbis submodule")
set(INSTALL_CMAKE_PACKAGE_MODULE
    OFF
    CACHE BOOL "Overwrite vorbis option" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/vorbis/)

message(STATUS "Configuring flac submodule")
set(INSTALL_CMAKE_CONFIG_MODULE
    OFF
    CACHE BOOL "Overwrite flac option" FORCE)
set(INSTALL_PKGCONFIG_MODULES
    OFF
    CACHE BOOL "Overwrite flac option" FORCE)
set(BUILD_CXXLIBS
    OFF
    CACHE BOOL "Overwrite flac option" FORCE)
set(BUILD_PROGRAMS
    OFF
    CACHE BOOL "Overwrite flac option" FORCE)
set(BUILD_EXAMPLES
    OFF
    CACHE BOOL "Overwrite flac option" FORCE)
set(BUILD_TESTING
    OFF
    CACHE BOOL "Overwrite flac option" FORCE)
set(BUILD_DOCS
    OFF
    CACHE BOOL "Overwrite flac option" FORCE)
set(INSTALL_MANPAGES
    OFF
    CACHE BOOL "Overwrite flac option" FORCE)
set(WITH_OGG
    ON
    CACHE BOOL "Overwrite flac option" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/flac/)

message(STATUS "Configuring opus submodule")
set(OPUS_INSTALL_PKG_CONFIG_MODULE
    OFF
    CACHE BOOL "Overwrite opus option" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opus/)
# libsndfile includes <opus/opus.h> (installed layout), but in the opus source
# tree the headers lie directly in include/. Mirror them into an opus/
# subfolder.
set(OPUS_INCLUDE_MIRROR ${CMAKE_CURRENT_BINARY_DIR}/opus_include)
file(GLOB OPUS_PUBLIC_HEADERS
     ${CMAKE_CURRENT_SOURCE_DIR}/submodules/opus/include/*.h)
file(COPY ${OPUS_PUBLIC_HEADERS} DESTINATION ${OPUS_INCLUDE_MIRROR}/opus)
target_include_directories(opus
                           INTERFACE $<BUILD_INTERFACE:${OPUS_INCLUDE_MIRROR}>)

# OGG
set(OGG_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/submodules/ogg/include
    CACHE PATH "Overwrite libsndfile FindOgg result" FORCE)
set(OGG_LIBRARY
    Ogg::ogg
    CACHE STRING "Overwrite libsndfile FindOgg result" FORCE)

# VORBIS libsndfile requires the Enc component: find_package(Vorbis COMPONENTS
# Enc)
set(VORBIS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/submodules/vorbis/include)
set(Vorbis_Vorbis_INCLUDE_DIR
    ${VORBIS_INCLUDE}
    CACHE PATH "Overwrite libsndfile FindVorbis result" FORCE)
set(Vorbis_Vorbis_LIBRARY
    Vorbis::vorbis
    CACHE STRING "Overwrite libsndfile FindVorbis result" FORCE)
set(Vorbis_Enc_INCLUDE_DIR
    ${VORBIS_INCLUDE}
    CACHE PATH "Overwrite libsndfile FindVorbis result" FORCE)
set(Vorbis_Enc_LIBRARY
    Vorbis::vorbisenc
    CACHE STRING "Overwrite libsndfile FindVorbis result" FORCE)
set(Vorbis_File_INCLUDE_DIR
    ${VORBIS_INCLUDE}
    CACHE PATH "Overwrite libsndfile FindVorbis result" FORCE)
set(Vorbis_File_LIBRARY
    Vorbis::vorbisfile
    CACHE STRING "Overwrite libsndfile FindVorbis result" FORCE)

# FLAC
set(FLAC_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/submodules/flac/include
    CACHE PATH "Overwrite libsndfile FindFlac result" FORCE)
set(FLAC_LIBRARY
    FLAC::FLAC
    CACHE STRING "Overwrite libsndfile FindFlac result" FORCE)

# OPUS
set(OPUS_INCLUDE_DIR
    ${OPUS_INCLUDE_MIRROR}
    CACHE PATH "Overwrite libsndfile FindOpus result" FORCE)
set(OPUS_LIBRARY
    Opus::opus
    CACHE STRING "Overwrite libsndfile FindOpus result" FORCE)

message(STATUS "Configuring libsndfile submodule")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/libsndfile/)

include(CheckCXXSourceCompiles)

# Accepting -std=c++20 is not enough (e.g. GCC 10 accepts it but lacks
# std::bit_cast and <source_location>), so check the library features OpenAL
# Soft actually uses.
if(MSVC)
  set(CMAKE_REQUIRED_FLAGS "/std:c++20")
else()
  set(CMAKE_REQUIRED_FLAGS "-std=c++20")
endif()

check_cxx_source_compiles(
  "
  #include <algorithm>
  #include <bit>
  #include <concepts>
  #include <numbers>
  #include <ranges>
  #include <source_location>
  #include <span>
  int main() {
    int a[4]{};
    std::ranges::fill(a, 1);
    unsigned u = std::bit_cast<unsigned>(1.0f);
    for (int i : std::views::iota(0, 4) | std::views::take(2)) u += a[i];
    std::span<int> s{a};
    auto loc = std::source_location::current();
    return static_cast<int>(u + s.size() + loc.line() + std::numbers::pi) * 0;
  }
  "
  OPENAL_CXX20_FEATURES_SUPPORTED)

unset(CMAKE_REQUIRED_FLAGS)

if(OPENAL_CXX20_FEATURES_SUPPORTED)
  message(
    STATUS
      "Compiler supports C++20 - build OpenAL package instead of finding package."
  )
  message(STATUS "Configuring OpenAL submdule")
  set(ALSOFT_EXAMPLES
      OFF
      CACHE BOOL "Overwrite openal result" FORCE)
  set(ALSOFT_INSTALL
      OFF
      CACHE BOOL "Overwrite openal result" FORCE)
  set(ALSOFT_INSTALL_CONFIG
      OFF
      CACHE BOOL "Overwrite openal result" FORCE)
  set(ALSOFT_INSTALL_HRTF_DATA
      OFF
      CACHE BOOL "Overwrite openal result" FORCE)
  set(ALSOFT_INSTALL_AMBDEC_PRESETS
      OFF
      CACHE BOOL "Overwrite openal result" FORCE)
  set(ALSOFT_INSTALL_EXAMPLES
      OFF
      CACHE BOOL "Overwrite openal result" FORCE)
  set(ALSOFT_INSTALL_UTILS
      OFF
      CACHE BOOL "Overwrite openal result" FORCE)
  set(ALSOFT_UPDATE_BUILD_VERSION
      OFF
      CACHE BOOL "Overwrite openal result" FORCE)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/openal/)
  set(OPENAL_LIBRARIES OpenAL)
else()
  message(
    STATUS
      "Compiler doesn't support C++20 - try to find OpenAL package instead of compiling."
  )
  find_package(OpenAL REQUIRED)
  set(OPENAL_LIBRARIES OpenAL::OpenAL)
endif()

# get rid of debug postfix for glew libraries
set_target_properties(libglew_shared
                      PROPERTIES OUTPUT_NAME "glew-shared" DEBUG_POSTFIX "")

message(STATUS "Configuring GLFW submodule")
set(GLFW_BUILD_DOCS
    OFF
    CACHE BOOL "Overwrite build the GLFW documentation" FORCE)
set(GLFW_INSTALL
    OFF
    CACHE BOOL "Overwrite generate glfw installation target" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/glfw)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

add_library(${LIB_NAME} STATIC ${SOURCES} ${HEADERS})

# PUBLIC: these change types/class layouts in engine headers, so every consumer
# must be compiled with exactly the same set
target_compile_definitions(
  ${LIB_NAME}
  PUBLIC GLM_ENABLE_EXPERIMENTAL
  PUBLIC BT_USE_DOUBLE_PRECISION
  PUBLIC SHADING_MODEL_PBR
  PUBLIC $<$<CONFIG:Debug>:DEBUG>)

if(ENABLE_LIBUNWIND)
  target_compile_definitions(${LIB_NAME} PUBLIC USE_LIBUNWIND)
endif(ENABLE_LIBUNWIND)

ApplyBuildOptions(${LIB_NAME})

set(SUBMODULES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/)

# glew, glfw and OpenAL targets export their include dirs themselves
set(ENGINE_CORE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(ASSIMP_DEPENDENCE_INCLUDE ${SUBMODULES_DIR}/assimp/include/)
set(GLM_DEPENDENCE_INCLUDE ${SUBMODULES_DIR}/glm/)
set(FREE_TYPE_INCLUDE ${SUBMODULES_DIR}/freetype/include/)
set(NLOHMANN_JSON_INCLUDE ${SUBMODULES_DIR}/nlohmann_json/include/)
set(LUA_DEPENDENCE_INCLUDE ${SUBMODULES_DIR}/lua/lua-5.4.7/include/)
set(STB_DEPENDENCE_INCLUDE ${SUBMODULES_DIR}/stb/)
set(BULLET_DEPENDENCE_INCLUDE ${SUBMODULES_DIR}/bullet3/src/)
set(SNDFILE_INCLUDE ${SUBMODULES_DIR}/libsndfile/include/)
set(LOGER_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/Logger/)

target_include_directories(
  ${LIB_NAME}
  PUBLIC ${ENGINE_CORE_INCLUDE}
  PUBLIC ${LOGER_DEPENDENCE_INCLUDE}
  PUBLIC ${STB_DEPENDENCE_INCLUDE}
  PUBLIC ${ASSIMP_DEPENDENCE_INCLUDE}
  PUBLIC ${GLM_DEPENDENCE_INCLUDE}
  PUBLIC ${BULLET_DEPENDENCE_INCLUDE}
  PUBLIC ${LUA_DEPENDENCE_INCLUDE}
  PUBLIC ${SNDFILE_INCLUDE}
  PUBLIC ${NLOHMANN_JSON_INCLUDE}
  PUBLIC ${FREE_TYPE_INCLUDE})

if(UNIX)
  target_link_libraries(${LIB_NAME} PRIVATE X11)
  target_link_libraries(${LIB_NAME} PRIVATE Threads::Threads)

  if(NOT CMAKE_DL_LIBS)
    message(FATAL_ERROR "DL library wasn't found!")
  endif()

  target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_DL_LIBS})
endif()

target_link_libraries(
  ${LIB_NAME}
  PUBLIC ${OPENGL_LIBRARIES}
  PUBLIC assimp
  # Bullet declares inter-library deps only for shared builds, so static libs
  # must be listed dependents first (ld resolves archives left to right)
  PUBLIC ConvexDecomposition
  PUBLIC BulletDynamics
  PUBLIC BulletCollision
  PUBLIC Bullet3Dynamics
  PUBLIC Bullet3Collision
  PUBLIC Bullet3Geometry
  PUBLIC Bullet3Common
  PUBLIC LinearMath
  PUBLIC TinyLogger
  PUBLIC libglew_shared
  PUBLIC glfw
  PUBLIC freetype
  PUBLIC nlohmann_json
  PUBLIC lua_static
  PUBLIC sndfile
  PUBLIC ${OPENAL_LIBRARIES})

if(ENABLE_LIBUNWIND)
  target_link_libraries(${LIB_NAME} PUBLIC UnwindTool)
endif()

file(GLOB_RECURSE FORMAT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
     ${CMAKE_CURRENT_SOURCE_DIR}/res/shaders/*.glsl)

file(GLOB_RECURSE LUA_FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/scripts/*.lua)

if(NOT WIN32)
  add_custom_target(
    phengine-codeformat
    COMMAND clang-format -i ${FORMAT_SRC}
    COMMAND lua-format ${LUA_FILES} -i --align-args
            --config=${CMAKE_SOURCE_DIR}/lua_format_config
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting phengine source files...")
endif()
