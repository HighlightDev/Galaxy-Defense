set(HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/DeferredResources/DeferredResource.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DeferredResources/DeferredResourceController.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DeferredResources/DeferredResourceCreator.h

  ${CMAKE_CURRENT_SOURCE_DIR}/Policy/CompositeShaderAllocationPolicy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Policy/ShaderAllocationPolicy.h
  
  ${CMAKE_CURRENT_SOURCE_DIR}/Policy/MeshAllocationPolicy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Policy/RenderTargetAllocationPolicy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Policy/SimplePrimitiveAllocationPolicy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Policy/TextureAllocationPolicy.h

  ${CMAKE_CURRENT_SOURCE_DIR}/Pool/CompositeShaderPool.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Pool/MeshPool.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Pool/RenderTargetPool.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Pool/ShaderPool.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Pool/SimplePrimitivePool.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Pool/TexturePool.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Pool/PoolBase.h

  ${CMAKE_CURRENT_SOURCE_DIR}/RenderTargetStorage.h
)

set(SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/DeferredResources/DeferredResource.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DeferredResources/DeferredResourceController.cpp
      
  ${CMAKE_CURRENT_SOURCE_DIR}/Policy/MeshAllocationPolicy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Policy/RenderTargetAllocationPolicy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Policy/SimplePrimitiveAllocationPolicy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Policy/TextureAllocationPolicy.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/Pool/CompositeShaderPool.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Pool/MeshPool.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Pool/RenderTargetPool.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Pool/ShaderPool.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Pool/SimplePrimitivePool.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Pool/TexturePool.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/RenderTargetStorage.cpp
)

add_library(EngineResourceManagerCoreModule STATIC ${HEADERS} ${SOURCE})

set_target_properties(EngineResourceManagerCoreModule PROPERTIES LINKER_LANGUAGE CXX)

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(STB_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/STB/include/)
set(ASSIMP_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/include/)
set(GLM_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)
set(CEREAL_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/cereal/include/)
set(GLEW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)

target_include_directories(EngineResourceManagerCoreModule
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
PUBLIC ${ENGINE_CORE_INCLUDE}
PUBLIC ${STB_DEPENDENCE_INCLUDE}
PUBLIC ${ASSIMP_DEPENDENCE_INCLUDE}
PUBLIC ${GLM_DEPENDENCE_INCLUDE}
PUBLIC ${CEREAL_DEPENDENCE_INCLUDE}
PUBLIC ${GLEW_DEPENDENCE_INCLUDE})
