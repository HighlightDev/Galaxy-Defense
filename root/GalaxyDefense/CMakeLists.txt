set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/PHEngine/cmake)
include(Utility)
include(Flags)
include(Dependencies)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

set(EXECUTABLE_NAME GalaxyDefense)
add_executable(${EXECUTABLE_NAME} ${HEADERS} ${SOURCES})

# Engine-wide definitions (DEBUG, BT_USE_DOUBLE_PRECISION, ...) come from
# PHEngine
applybuildoptions(${EXECUTABLE_NAME})

add_custom_command(
  TARGET ${EXECUTABLE_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E echo
    "Start copy of resources to build directory: $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/res"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${PROJECT_SOURCE_DIR}/PHEngine/res/
          $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/res)

if(WIN32)
  add_custom_command(
    TARGET ${EXECUTABLE_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo
            "Copy GLEW and OpenAL to $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:libglew_shared>"
            "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:OpenAL>"
            "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>")
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINKER_LANGUAGE CXX)

set(GAME_SRC_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(LOGGER_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/Logger/)

target_include_directories(
  ${EXECUTABLE_NAME}
  PUBLIC ${GAME_SRC_INCLUDE}
  PUBLIC ${ENGINE_CORE_INCLUDE}
  PUBLIC ${LOGGER_DEPENDENCE_INCLUDE})

if(UNIX)
  target_link_libraries(${EXECUTABLE_NAME} PRIVATE X11)
  target_link_libraries(${EXECUTABLE_NAME} PRIVATE Threads::Threads)

  if(NOT CMAKE_DL_LIBS)
    message(FATAL_ERROR "DL library wasn't found!")
  else()
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${CMAKE_DL_LIBS})
  endif()
endif()

target_link_libraries(${EXECUTABLE_NAME} PRIVATE PHEngine)

message(STATUS "LibUnwind enabled: ${ENABLE_LIBUNWIND}")
if(ENABLE_LIBUNWIND)
  target_link_libraries(${EXECUTABLE_NAME} PRIVATE UnwindTool)
endif(ENABLE_LIBUNWIND)

file(GLOB_RECURSE FORMAT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

if(NOT WIN32)
  add_custom_target(
    galaxy-defense-codeformat
    COMMAND clang-format -i ${FORMAT_SRC}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting galaxy-defense source files...")
endif()
