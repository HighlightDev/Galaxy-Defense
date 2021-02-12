
set(HEADERS
  TextureRenderer.h
)

set(SOURCE
  TextureRenderer.cpp
)

add_library(EngineDebugCoreModule STATIC ${HEADERS} ${SOURCE})

target_include_directories(EngineDebugCoreModule INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )