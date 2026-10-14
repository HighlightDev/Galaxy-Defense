set(SOURCES Core.cpp LogInterface.cpp LoggerClient.cpp)

set(HEADERS Core.h LoggerClient.h LogInterface.h)

add_library(TinyLogger STATIC ${HEADERS} ${SOURCES})

target_compile_definitions(TinyLogger PRIVATE TINYLOGGER_EXPORTS)

file(
  GLOB_RECURSE
  FORMAT_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

if(NOT WIN32)
  add_custom_target(
    tinylogger-codeformat
    COMMAND clang-format -i ${FORMAT_SRC}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting tinylogger source files...")
endif()
