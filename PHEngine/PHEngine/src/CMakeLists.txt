
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

link_directories(${CMAKE_BINARY_DIR}/Dependencies/Assimp/lib/Debug/)

target_link_libraries(PHEngine ${ENGINE_CORE_MODULE} assimp-vc141-mtd)