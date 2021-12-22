
set(HEADERS
  InterThreadCommunicationMgr.h
  Job.h
)

set(SOURCE
  InterThreadCommunicationMgr.cpp
  Job.cpp
)

add_subdirectory(CommonCore/)
add_subdirectory(DebugCore/)
add_subdirectory(GameCore/)
add_subdirectory(GraphicsCore/)
add_subdirectory(IoCore/)
add_subdirectory(ResourceManagerCore/)
add_subdirectory(UtilityCore/)

set(APPEND ENGINE_COMMON_CORE_MODULE EngineCommonCoreModule)
set(APPEND ENGINE_DEBUG_CORE_MODULE EngineDebugCoreModule)
set(APPEND ENGINE_GAME_CORE_MODULE EngineGameCoreModule)
set(APPEND ENGINE_GRAPHICS_CORE_MODULE EngineGraphicsCoreModule)
set(APPEND ENGINE_IO_CORE_MODULE EngineIoCoreModule)
set(APPEND ENGINE_RESOURCE_MANAGER_CORE_MODULE EngineResourceManagerCoreModule)
set(APPEND ENGINE_UTILITY_CORE_MODULE EngineUtilityCoreModule)

add_library(EngineCoreModule STATIC ${HEADERS} ${SOURCE})

set_target_properties(EngineCoreModule PROPERTIES LINKER_LANGUAGE CXX)

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(STB_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/STB/include/)
set(ASSIMP_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/include/)
set(GLM_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)
set(CEREAL_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/cereal/include/)
set(GLEW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)

target_include_directories(EngineCoreModule 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${ENGINE_CORE_INCLUDE}
  PUBLIC ${STB_DEPENDENCE_INCLUDE}
  PUBLIC ${ASSIMP_DEPENDENCE_INCLUDE}
  PUBLIC ${GLM_DEPENDENCE_INCLUDE}
  PUBLIC ${CEREAL_DEPENDENCE_INCLUDE}
  PUBLIC ${GLEW_DEPENDENCE_INCLUDE})

target_link_libraries(EngineCoreModule PUBLIC
 ${ENGINE_COMMON_CORE_MODULE}
 ${ENGINE_DEBUG_CORE_MODULE}
 ${ENGINE_GAME_CORE_MODULE} 
 ${ENGINE_GRAPHICS_CORE_MODULE}
 ${ENGINE_IO_CORE_MODULE}
 ${ENGINE_RESOURCE_MANAGER_CORE_MODULE}
 ${ENGINE_UTILITY_CORE_MODULE})