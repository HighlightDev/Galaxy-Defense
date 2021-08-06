
set(HEADERS
  Engine.h
)

set(SOURCE
  Engine.cpp
)

add_subdirectory(Core)

set(APPEND ENGINE_CORE_MODULE EngineCoreModule)

add_library(PHEngine STATIC ${HEADERS} ${SOURCE})

target_include_directories(PHEngine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/STB/include/
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/Assimp/include/
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/BulletPhysics/include/
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/cereal/include/
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/GLEW/include/
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/GLM/include/
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/GLFW/include/
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/Lua54/include/)

link_directories(${CMAKE_BINARY_DIR}/Dependencies/Assimp/lib/Debug/)

if (WIN32)
  target_link_libraries(PHEngine ${ENGINE_CORE_MODULE} assimp-vc141-mtd)
endif()