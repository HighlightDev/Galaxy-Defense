
set(HEADERS
  Engine.h
)

set(SOURCE
  Engine.cpp
)

add_subdirectory(Core)

set(APPEND ENGINE_CORE_MODULE EngineCoreModule)

add_library(PHEngine STATIC ${HEADERS} ${SOURCE})

target_include_directories(PHEngine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

target_link_libraries(PHEngine ${ENGINE_CORE_MODULE})