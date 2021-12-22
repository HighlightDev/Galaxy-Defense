
set(HEADERS
  Engine.h
)

set(SOURCE
  Engine.cpp
)

add_subdirectory(Core)

set(APPEND ENGINE_CORE_MODULE EngineCoreModule)

add_library(PHEngine STATIC ${HEADERS} ${SOURCE})

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(STB_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/STB/include/)
set(ASSIMP_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/include/)
set(GLM_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)
set(CEREAL_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/cereal/include/)
set(GLEW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)

target_include_directories(PHEngine 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${ENGINE_CORE_INCLUDE}
  PUBLIC ${STB_DEPENDENCE_INCLUDE}
  PUBLIC ${ASSIMP_DEPENDENCE_INCLUDE}
  PUBLIC ${GLM_DEPENDENCE_INCLUDE}
  PUBLIC ${CEREAL_DEPENDENCE_INCLUDE}
  PUBLIC ${GLEW_DEPENDENCE_INCLUDE})

link_directories(${CMAKE_BINARY_DIR}/Dependencies/Assimp/lib/Debug/)

if (WIN32)
  target_link_libraries(PHEngine ${ENGINE_CORE_MODULE} assimp-vc141-mtd)
endif()