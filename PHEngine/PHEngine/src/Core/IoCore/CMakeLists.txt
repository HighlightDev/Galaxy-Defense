set(HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/AsyncLoaderCore/AsyncDataProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/AsyncLoaderCore/AsyncJob.h
  ${CMAKE_CURRENT_SOURCE_DIR}/AsyncLoaderCore/ResourceLoader.h
  ${CMAKE_CURRENT_SOURCE_DIR}/AsyncLoaderCore/ResourceMap.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MeshLoaderCore/AssimpLoader/AssimpLoader.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MeshLoaderCore/AssimpLoader/MeshDataCollector.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MeshLoaderCore/AnimationSequenceData.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MeshLoaderCore/MeshBoneInfo.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MeshLoaderCore/MeshNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MeshLoaderCore/MeshResourceInfo.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TextureLoaderCore/StbLoader/StbLoader.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TextureLoaderCore/TextureResourceInfo.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RawResource.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DisplayDeviceDataProvider.h
  ${CMAKE_CURRENT_SOURCE_DIR}/FileFacade.h
  ${CMAKE_CURRENT_SOURCE_DIR}/FolderManager.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ResourceExtensionsInfo.h
)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/AsyncLoaderCore/ResourceLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AsyncLoaderCore/ResourceMap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MeshLoaderCore/AssimpLoader/AssimpLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MeshLoaderCore/AssimpLoader/MeshDataCollector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TextureLoaderCore/StbLoader/StbLoader.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/DisplayDeviceDataProvider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FileFacade.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FolderManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ResourceExtensionsInfo.cpp
)

add_library(EngineIoCoreModule STATIC ${HEADERS} ${SOURCE})

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(STB_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/STB/include/)

message(STATUS "STB_DEPENDENCE_INCLUDE path=${STB_DEPENDENCE_INCLUDE}")

set_target_properties(EngineIoCoreModule PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(EngineIoCoreModule
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${ENGINE_CORE_INCLUDE}
  PUBLIC ${STB_DEPENDENCE_INCLUDE})
