
set(HEADERS
  TextureRenderer.h
)

set(SOURCE
  TextureRenderer.cpp
)

set(GLM_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)

add_library(EngineDebugCoreModule STATIC ${HEADERS} ${SOURCE})

set_target_properties(EngineDebugCoreModule PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(EngineDebugCoreModule 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} 
  PUBLIC ${GLM_DEPENDENCE_INCLUDE}
)