set(HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/Common/ScreenQuad.h

  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterial.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/IMaterial.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/MaterialNodeDecorator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/MaterialParser.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterial.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/MaterialProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/MaterialProcessPolicy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialBinaryAddOperationNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialBinaryDivOperationNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialBinaryMulOperationNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialBinaryOperationNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialBinarySubOperationNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialConstantFloatValueNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialEnterNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialUnaryDecrementNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialUnaryIncrementNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialUnaryOperationNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialValueNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialValuePropertyNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/MaterialProperties/MaterialPropertySetter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/MaterialProperties/BindingMaterialProperty.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/MaterialProperties/DeferredTextureMaterialProperty.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/MaterialProperties/DynamicFloatMaterialProperty.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/MaterialProperties/FloatBindingMaterialProperty.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Material/MaterialProperties/TextureMaterialProperty.h
  
  ${CMAKE_CURRENT_SOURCE_DIR}/Mesh/AnimatedMeshData.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Mesh/AnimatedSkin.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Mesh/AnimationPlayer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Mesh/MeshAttributes.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Mesh/Skin.h

  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/DataCarryFlag.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/VertexBufferObject.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/IndexBufferObject.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/VertexArrayObject.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/VertexBufferObjectBase.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Framebuffer/FramebufferBundle.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Framebuffer/FramebufferObject.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/CompositeShaderParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/ShaderPredefineUtility.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/ShaderType.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/VertexFactoryCompositeShader.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/AVertexFactoryMaterialShaderModule.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/AVertexFactoryShaderModule.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/IShader.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/MaterialShader.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/Shader.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/ShaderParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/ShaderPredefine.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/Uniform.h
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/VertexFactoryShader.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RenderData/BillboardRenderData.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RenderData/CubemapRenderData.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RenderData/DirectionalLightRenderData.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RenderData/LightRenderData.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RenderData/PointLightRenderData.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RenderData/SkeletalMeshRenderData.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RenderData/SkyboxRenderData.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RenderData/SpotlightRenderData.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RenderData/StaticMeshRenderData.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RenderData/WaterPlaneRenderData.h

  ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/RenderState.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/DeferredShadingGBuffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/DeferredShadingSceneRenderer.h

  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/BillboardSceneProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/CameraSceneProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/CubemapSceneProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/DirectionalLightSceneProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/LightSceneProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/MainCameraSceneProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/PlanarReflectionProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/PointLightSceneProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/PrimitiveSceneProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/SceneProxyBase.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/SkeletalMeshSceneProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/SkyboxSceneProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/SpotlightSceneProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/StaticMeshSceneProxy.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/WaterPlaneSceneProxy.h

  ${CMAKE_CURRENT_SOURCE_DIR}/SceneViewInfo/AProxyVisibilityController.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneViewInfo/CameraFrustum.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneViewInfo/Octree.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneViewInfo/SceneView.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneViewInfo/ViewPerspectiveInfo.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneViewInfo/ViewPortInfo.h

  ${CMAKE_CURRENT_SOURCE_DIR}/Shadow/ProjectedDirectionalLightShadowInfo.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Shadow/ProjectedPointLightShadowInfo.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Shadow/ProjectedShadowInfo.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Shadow/ProjectedSpotlightShadowInfo.h

  ${CMAKE_CURRENT_SOURCE_DIR}/Texture/CubemapTexture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Texture/ITexture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Texture/TexParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Texture/Texture2d.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Texture/TextureMipMapState.h

  ${CMAKE_CURRENT_SOURCE_DIR}/TextureAtlas/TextureAtlasCell.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TextureAtlas/TextureAtlasHandler.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TextureAtlas/TextureAtlas.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TextureAtlas/TextureAtlasFactory.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TextureAtlas/TextureAtlasSpaceRequest.h

)

set(SOURCE
${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterial.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/IMaterial.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/MaterialNodeDecorator.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/MaterialParser.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterial.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/MaterialProxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialBinaryAddOperationNode.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialBinaryDivOperationNode.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialBinaryMulOperationNode.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialBinaryOperationNode.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialBinarySubOperationNode.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialConstantFloatValueNode.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialEnterNode.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialUnaryDecrementNode.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialUnaryIncrementNode.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialUnaryOperationNode.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialValueNode.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/DynamicMaterialOperations/MaterialValuePropertyNode.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material/MaterialProperties/MaterialPropertySetter.cpp

${CMAKE_CURRENT_SOURCE_DIR}/Mesh/AnimatedMeshData.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Mesh/AnimatedSkin.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Mesh/AnimationPlayer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Mesh/MeshAttributes.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Mesh/Skin.cpp

${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/IndexBufferObject.cpp
${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/VertexArrayObject.cpp
${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/VertexBufferObjectBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Framebuffer/FramebufferBundle.cpp
${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Framebuffer/FramebufferObject.cpp

${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/AVertexFactoryMaterialShaderModule.cpp
${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/AVertexFactoryShaderModule.cpp
${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/IShader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/MaterialShader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/Shader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/ShaderParams.cpp
${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/ShaderPredefine.cpp
${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/Uniform.cpp
${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Shader/VertexFactoryShader.cpp

${CMAKE_CURRENT_SOURCE_DIR}/Common/ScreenQuad.cpp

${CMAKE_CURRENT_SOURCE_DIR}/Renderer/DeferredShadingGBuffer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Renderer/DeferredShadingSceneRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Renderer/RenderState.cpp

${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/BillboardSceneProxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/CameraSceneProxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/CubemapSceneProxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/DirectionalLightSceneProxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/LightSceneProxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/MainCameraSceneProxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/PlanarReflectionProxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/PointLightSceneProxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/PrimitiveSceneProxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/SceneProxyBase.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/SkeletalMeshSceneProxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/SkyboxSceneProxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/SpotlightSceneProxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/StaticMeshSceneProxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneProxy/WaterPlaneSceneProxy.cpp

${CMAKE_CURRENT_SOURCE_DIR}/SceneViewInfo/AProxyVisibilityController.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneViewInfo/CameraFrustum.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneViewInfo/Octree.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SceneViewInfo/SceneView.cpp

${CMAKE_CURRENT_SOURCE_DIR}/Shadow/ProjectedDirectionalLightShadowInfo.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Shadow/ProjectedPointLightShadowInfo.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Shadow/ProjectedShadowInfo.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Shadow/ProjectedSpotlightShadowInfo.cpp

${CMAKE_CURRENT_SOURCE_DIR}/Texture/CubemapTexture.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Texture/ITexture.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Texture/TexParams.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Texture/Texture2d.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Texture/TextureMipMapState.cpp

${CMAKE_CURRENT_SOURCE_DIR}/TextureAtlas/TextureAtlas.cpp
${CMAKE_CURRENT_SOURCE_DIR}/TextureAtlas/TextureAtlasFactory.cpp
${CMAKE_CURRENT_SOURCE_DIR}/TextureAtlas/TextureAtlasSpaceRequest.cpp
)

add_library(EngineGraphicsCoreModule STATIC ${HEADERS} ${SOURCE})

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(STB_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/STB/include/)
set(ASSIMP_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/include/)
set(GLM_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)
set(CEREAL_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/cereal/include/)
set(GLEW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)

target_include_directories(EngineGraphicsCoreModule
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
PUBLIC ${ENGINE_CORE_INCLUDE}
PUBLIC ${STB_DEPENDENCE_INCLUDE}
PUBLIC ${ASSIMP_DEPENDENCE_INCLUDE}
PUBLIC ${GLM_DEPENDENCE_INCLUDE}
PUBLIC ${CEREAL_DEPENDENCE_INCLUDE}
PUBLIC ${GLEW_DEPENDENCE_INCLUDE})
