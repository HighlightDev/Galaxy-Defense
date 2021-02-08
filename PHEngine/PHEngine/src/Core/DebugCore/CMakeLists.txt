
set(HEADERS
  InterThreadCommunicationMgr.h
  Job.h
)

set(SOURCE
  InterThreadCommunicationMgr.cpp
  Job.cpp
)

#add_subdirectory(Core)

add_library(EngineCoreModule STATIC ${HEADERS} ${SOURCE})

target_include_directories(EngineCoreModule INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

#target_link_libraries(PHEngine ${CORE_MODULE})