
set(HEADERS
  InterThreadCommunicationMgr.h
  Job.h
)

set(SOURCE
  InterThreadCommunicationMgr.cpp
  Job.cpp
)

#add_subdirectory(CommonCore)
add_subdirectory(DebugCore)
add_subdirectory(GameCore)

#set(APPEND ENGINE_COMMON_CORE_MODULE EngineCommonCoreModule)
set(APPEND ENGINE_DEBUG_CORE_MODULE EngineDebugCoreModule)
set(APPEND ENGINE_GAME_CORE_MODULE EngineGameCoreModule)

add_library(EngineCoreModule STATIC ${HEADERS} ${SOURCE})

target_include_directories(EngineCoreModule INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

#target_link_libraries(EngineCoreModule ${ENGINE_COMMON_CORE_MODULE} ${ENGINE_DEBUG_CORE_MODULE} ${ENGINE_GAME_CORE_MODULE})
target_link_libraries(EngineCoreModule ${ENGINE_DEBUG_CORE_MODULE} ${ENGINE_GAME_CORE_MODULE})