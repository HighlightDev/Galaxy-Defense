set(SOURCES 
Core.cpp
LogInterface.cpp
)

set(HEADERS
Core.h
LoggerClient.h
LogInterface.h
)

add_library(TinyLogger SHARED ${HEADERS} ${SOURCES})