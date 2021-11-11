
set(HEADERS
  Engine.h
)

set(SOURCE
  Engine.cpp
)

add_subdirectory(Core)

set(APPEND ENGINE_CORE_MODULE EngineCoreModule)

add_library(PHEngine STATIC ${HEADERS} ${SOURCE})

target_include_directories(PHEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

link_directories(${CMAKE_BINARY_DIR}/Dependencies/Assimp/lib/Debug/)

if (WIN32)
  target_link_libraries(PHEngine ${ENGINE_CORE_MODULE} assimp-vc141-mtd)
endif()