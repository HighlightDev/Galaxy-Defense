set(SOURCES Core.cpp LogInterface.cpp LoggerClient.cpp)

set(HEADERS Core.h LoggerClient.h LogInterface.h)

add_library(TinyLogger STATIC ${HEADERS} ${SOURCES})

target_compile_definitions(TinyLogger PRIVATE TINYLOGGER_EXPORTS)
