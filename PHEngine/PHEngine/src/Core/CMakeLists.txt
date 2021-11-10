
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

target_include_directories(EngineCoreModule PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(EngineCoreModule
 ${ENGINE_COMMON_CORE_MODULE}
 ${ENGINE_DEBUG_CORE_MODULE}
 ${ENGINE_GAME_CORE_MODULE} 
 ${ENGINE_GRAPHICS_CORE_MODULE}
 ${ENGINE_IO_CORE_MODULE}
 ${ENGINE_RESOURCE_MANAGER_CORE_MODULE}
 ${ENGINE_UTILITY_CORE_MODULE})