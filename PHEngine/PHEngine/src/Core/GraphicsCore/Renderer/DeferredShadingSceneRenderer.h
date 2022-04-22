#pragma once

#include <cstddef>
#include <glm/mat4x4.hpp>

#include "Core/GraphicsCore/Renderer/DeferredShadingGBuffer.h"
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

#include "Core/GameCore/ShaderImplementation/DeferredLightShader.h"
#include "Core/GameCore/ShaderImplementation/PointLightDepthShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkeletalMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/DepthCollectShader.h"

#include "Core/GameCore/Physics/DebugRender/DebugPhysicsRenderData.h"

#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/InterThreadCommunicationMgr.h"
#include "Core/DebugCore/TextureRenderer.h"

#include <utility>

using namespace EngineCore::ShaderImpl;
using namespace EngineCore;
using namespace Thread;
using namespace Debug;
using namespace EnginePhysics;

namespace Graphics
{
	namespace Renderer
	{

		class DeferredShadingSceneRenderer
		{

         using PointLightProxiesPtrVector = std::vector<PointLightSceneProxy*>;
         using DirectionalLightProxiesPtrVector = std::vector<DirectionalLightSceneProxy*>;
         using SpotlightProxiesPtrVector = std::vector<SpotlightSceneProxy*>;

      public:

         std::vector<std::shared_ptr<SceneView>> SceneViewsVector;

         std::vector<std::shared_ptr<PrimitiveSceneProxy>> PrimitiveProxiesVector;

         std::vector<std::shared_ptr<LightSceneProxy>> LightProxiesVector;

         std::vector<std::shared_ptr<MaterialProxy>> MaterialProxiesVector;

         std::vector<std::shared_ptr<PlanarReflectionProxy>> PlanarReflectionProxiesVector;

      private:

         InterThreadCommunicationMgr& m_interThreadMgr;

         /* G-buffers */
         std::unique_ptr<DeferredShadingGBuffer> m_gbuffer;

         // Shaders
         std::shared_ptr<DeferredLightShader> m_deferredLightShader;

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

         std::vector<PrimitiveSceneProxy*> mForwardRenderingProxiesVec;

         std::vector<SkeletalMeshSceneProxy*> mSkeletalProxiesVec;

         std::vector<PrimitiveSceneProxy*> mNonSkeletalProxiesVec;

         DirectionalLightProxiesPtrVector mDirLightProxiesVec;

         PointLightProxiesPtrVector mPointLightProxiesVec;

         SpotlightProxiesPtrVector mSpotlightProxiesVec;

         std::vector<PlanarReflectionProxy*> mPlanarReflectionProxiesVec;

         std::vector<std::pair<size_t, std::vector<LightSceneProxy*>>> mGroupedByShadowAtlasLights;

      private:

         void PrepareSceneProxiesForRender();

         void GroupLightsByShadowMap();

         void DeferredLightPass_RenderThread(std::shared_ptr<CameraSceneProxy> cameraProxy);

         void DeferredBasePass_RenderThread(std::shared_ptr<SceneView> sceneView);

         void ForwardBasePass_RenderThread(std::shared_ptr<SceneView> sceneView);

         void DepthPass(std::shared_ptr<SceneView> sceneView);

         void PlanarReflectionPass();

      public:

         DeferredShadingSceneRenderer(InterThreadCommunicationMgr& interThreadMgr);

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

#if DEBUG
         void SetDebugPhysicsRenderData(const DebugPhysicsRenderData& debugPhysicsRenderData);

         void PushRenderTargetToTextureRenderer();

         private:

         void DebugRenderPhysics(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

         void DebugFramePanelsPass();
#endif
		};

	}
}

