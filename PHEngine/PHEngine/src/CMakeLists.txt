
set(HEADERS
  Engine.h
)

set(SOURCE
  Engine.cpp
)

add_subdirectory(Core)

set(APPEND ENGINE_CORE_MODULE EngineCoreModule)

add_library(PHEngine STATIC ${HEADERS} ${SOURCE})

set(EXTERNAL_DEPENDENCIES_INCLUDE ${PROJECT_SOURCE_DIR}/External/)
set(DEPEND_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include)

target_include_directories(PHEngine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${DEPEND_INCLUDE}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/STB/include/
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/Assimp/include/
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/BulletPhysics/include/
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/cereal/include/
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/GLEW/include/
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/GLM/include/
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/GLFW/include/
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/Lua54/include/
  PUBLIC ${EXTERNAL_DEPENDENCIES_INCLUDE})

link_directories(${CMAKE_BINARY_DIR}/Dependencies/Assimp/lib/Debug/)

if (WIN32)
  target_link_libraries(PHEngine ${ENGINE_CORE_MODULE} assimp-vc141-mtd)
endif()