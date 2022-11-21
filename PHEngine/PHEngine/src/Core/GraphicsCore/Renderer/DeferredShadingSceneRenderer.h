#pragma once

#include <cstddef>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Core/GraphicsCore/Renderer/DeferredShadingGBuffer.h"
#include "Core/GraphicsCore/Renderer/ResolvedSceneFramebuffer.h"
#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SpotlightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PlanarReflectionProxy.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryCompositeShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/MaterialShader.h"
#include "Core/GraphicsCore/SceneViewInfo/SceneView.h"
#include "Core/GraphicsCore/Renderer/RenderState.h"
#include "Core/GraphicsCore/PostFX/PostFxRenderer.h"

#include "Core/GameCore/ShaderImplementation/DeferredLightShader.h"
#include "Core/GameCore/ShaderImplementation/PointLightDepthShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkeletalMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/DepthCollectShader.h"
#include "Core/GameCore/ShaderImplementation/FontRenderingShader.h"

#include "Core/GameCore/Physics/DebugRender/DebugPhysicsRenderData.h"

#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/InterThreadCommunicationMgr.h"
#include "Core/DebugCore/TextureRenderer.h"
#include "Core/GameCore/GUI/Text/FontHandler.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"

#include <utility>

using namespace Debug;
using namespace Thread;
using namespace EngineCore;
using namespace EngineCore::ShaderImpl;
using namespace EnginePhysics;
using namespace EngineCore::GUI;

namespace Graphics
{
   namespace Renderer
   {

      class DeferredShadingSceneRenderer
      {
         InterThreadCommunicationMgr &m_interThreadMgr;

         /* G-buffers */
         std::unique_ptr<DeferredShadingGBuffer> m_gbuffer;

         std::unique_ptr<ResolvedSceneFramebuffer> m_resolvedSceneFramebuffer;

         // Shaders
         std::shared_ptr<DeferredLightShader> m_deferredLightShader;
         std::shared_ptr<FontRenderingShader> m_fontShader;
         std::shared_ptr<VertexFactoryCompositeShader<SkeletalMeshVertexFactory<4>, DepthCollectShader>> mDepthCollectShaderSkeletal;
         std::shared_ptr<VertexFactoryCompositeShader<StaticMeshVertexFactory, DepthCollectShader>> mDepthCollectShaderNonSkeletal;
         std::shared_ptr<VertexFactoryCompositeShader<SkeletalMeshVertexFactory<4>, PointLightDepthCollectShader>> mDepthCollectPointLightShaderSkeletal;
         std::shared_ptr<VertexFactoryCompositeShader<StaticMeshVertexFactory, PointLightDepthCollectShader>> mDepthCollectPointLightShaderNonSkeletal;

         // Texture renderer
         TextureRenderer m_textureRenderer;

         bool bProxiesDirty;

         bool bLightProxiesDirty;

         bool bPlanarReflectionProxiesDirty;

#if DEBUG
         DebugPhysicsRenderData mDebugPhysicsRenderData;
#endif

         std::vector<std::shared_ptr<PrimitiveSceneProxy>> mForwardRenderingProxiesVec;
         std::vector<std::shared_ptr<SkeletalMeshSceneProxy>> mSkeletalProxiesVec;
         std::vector<std::shared_ptr<PrimitiveSceneProxy>> mNonSkeletalProxiesVec;
         std::vector<std::shared_ptr<DirectionalLightSceneProxy>> mDirLightProxiesVec;
         std::vector<std::shared_ptr<PointLightSceneProxy>> mPointLightProxiesVec;
         std::vector<std::shared_ptr<SpotlightSceneProxy>> mSpotlightProxiesVec;
         std::vector<std::shared_ptr<PlanarReflectionProxy>> mPlanarReflectionProxiesVec;
         std::vector<std::pair<size_t, std::vector<std::shared_ptr<LightSceneProxy>>>> mGroupedByShadowAtlasLights;

         FontHandler mFontHandler;

         std::unique_ptr<PostFxRenderer> mPostFxRenderer;

         // todo
         std::shared_ptr<UiCanvas> mUiCanvas;
         // todo

      public:
         std::vector<std::shared_ptr<SceneView>> SceneViewsVector;
         std::vector<std::shared_ptr<PrimitiveSceneProxy>> PrimitiveProxiesVector;
         std::vector<std::shared_ptr<LightSceneProxy>> LightProxiesVector;
         std::vector<std::shared_ptr<MaterialProxy>> MaterialProxiesVector;
         std::vector<std::shared_ptr<PlanarReflectionProxy>> PlanarReflectionProxiesVector;

      private:
         void PrepareSceneProxiesForRender();

         void GroupLightsByShadowMap();

         void DeferredLightPass_RenderThread(const std::shared_ptr<CameraSceneProxy> &cameraProxy);

         void DeferredBasePass_RenderThread(const std::shared_ptr<SceneView> &sceneView);

         void ForwardBasePass_RenderThread(const std::shared_ptr<SceneView> &sceneView);

         void DepthPass(const std::shared_ptr<SceneView> &sceneView);

         void PlanarReflectionPass();

         void GuiTextPass();

         void GuiPass(const std::shared_ptr<SceneView> &sceneView);

         void RegisterFonts();

      public:
         DeferredShadingSceneRenderer(InterThreadCommunicationMgr &interThreadMgr);

         ~DeferredShadingSceneRenderer();

         void PostLevelInit();

         void RenderScene_RenderThread();

         std::shared_ptr<SceneView> GetSceneViewByProxyId(const size_t proxyId) const;
         std::shared_ptr<PrimitiveSceneProxy> GetPrimitiveProxyByProxyId(const size_t proxyId) const;
         std::shared_ptr<LightSceneProxy> GetLightProxyByProxyId(const size_t proxyId) const;
         std::shared_ptr<MaterialProxy> GetMaterialProxyByProxyId(const size_t proxyId) const;
         std::shared_ptr<PlanarReflectionProxy> GetPlanarReflectionProxyByProxyId(const size_t proxyId) const;

         bool RemovePrimitiveProxyByProxyId(const size_t proxyId);

         bool RemoveLightProxyByProxyId(const size_t proxyId);

         void SetProxiesAreDirty(const bool bDirty);

         void SetLightProxiesAreDirty(const bool bDirty);

         void SetPlanarReflectionProxiesAreDirty(const bool bDirty);

         void RegisterText(const std::shared_ptr<TextFieldProxy> &textFieldProxy);

         void UnregisterText(const std::string &fontName, const int32_t textFieldProxyId);

         void TextPositionChanged(const std::string &fontName, const int32_t textFieldProxyId, const glm::vec2 &position);

         void TextColorChanged(const std::string &fontName, const int32_t textFieldProxyId, const glm::vec3 &color);

         void TextChanged(const std::string &fontName, const int32_t textFieldProxyId, const std::string &text);

         void TextVisibilityChanged(const std::string &fontName, const int32_t textFieldProxyId, const bool bIsVisible);

#if DEBUG
         void SetDebugPhysicsRenderData(const DebugPhysicsRenderData &debugPhysicsRenderData);

         void PushRenderTargetToTextureRenderer();

      private:
         void DebugRenderPhysics(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);

         void DebugFramePanelsPass();
#endif
      };

   }
}
