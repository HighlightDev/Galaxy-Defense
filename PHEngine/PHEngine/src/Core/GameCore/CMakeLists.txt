
set(HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/Batcher/Batch.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Batcher/BatchJobMaster.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Components/Component.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Components/ComponentCreatorFactory.h
)

set(SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/Batcher/Batch.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Batcher/BatchJobMaster.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Components/Component.cpp
)

#add_subdirectory(Core)

add_library(EngineGameCoreModule STATIC ${HEADERS} ${SOURCE})

target_include_directories(EngineGameCoreModule INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

#target_link_libraries(PHEngine ${CORE_MODULE})