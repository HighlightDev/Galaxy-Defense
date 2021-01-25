#pragma once

#include <stdint.h>
#include <glm/mat4x4.hpp>

#include "Core/GraphicsCore/Renderer/DeferredShadingGBuffer.h"
#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SpotlightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/MaterialShader.h"
#include "Core/GraphicsCore/SceneViewInfo/SceneView.h"
#include "Core/GraphicsCore/Renderer/RenderState.h"

#include "Core/GameCore/ShaderImplementation/DeferredLightShader.h"
#include "Core/GameCore/ShaderImplementation/DepthShader.h"
#include "Core/GameCore/ShaderImplementation/CubemapDepthShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkeletalMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"

#include "Core/GameCore/Physics/DebugRender/DebugPhysicsRenderData.h"

#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/InterThreadCommunicationMgr.h"
#include "Core/DebugCore/TextureRenderer.h"

using namespace Game::ShaderImpl;
using namespace Game;
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

         std::unordered_map<size_t /*camera proxy id*/, std::shared_ptr<SceneView>> SceneViews;

         std::unordered_map<size_t /*proxy id*/, std::shared_ptr<PrimitiveSceneProxy>> SceneProxies;

         std::unordered_map<size_t /*proxy id*/, std::shared_ptr<LightSceneProxy>> LightProxies;

      private:

         InterThreadCommunicationMgr& m_interThreadMgr;

         /* G-buffers */
         std::unique_ptr<DeferredShadingGBuffer> m_gbuffer;

         // Shaders
         std::shared_ptr<DeferredLightShader> m_deferredLightShader;
         std::shared_ptr<DepthShader<eShaderMeshType::SKELETAL>> m_depthShaderSkeletal;
         std::shared_ptr<DepthShader<eShaderMeshType::NON_SKELETAL>> m_depthShaderNonSkeletal;
         std::shared_ptr<CubemapDepthShader<eShaderMeshType::SKELETAL>> m_depthCubemapShaderSkeletal;
         std::shared_ptr<CubemapDepthShader<eShaderMeshType::NON_SKELETAL>> m_depthCubemapShaderNonSkeletal;

         // Texture renderer
         TextureRenderer m_textureRenderer;

         std::function<bool(DirectionalLightSceneProxy*, DirectionalLightSceneProxy*)> mCompareShadowMapDescriptors;

         bool bProxiesDirty;

         bool bLightProxiesDirty;

#if DEBUG
         DebugPhysicsRenderData mDebugPhysicsRenderData;
#endif

         std::vector<PrimitiveSceneProxy*> forwardRenderingProxies;

         std::vector<SkeletalMeshSceneProxy*> skeletalProxies;

         std::vector<PrimitiveSceneProxy*> nonSkeletalProxies;

         DirectionalLightProxiesPtrVector dirLightProxies;

         PointLightProxiesPtrVector pointLightProxies;

         SpotlightProxiesPtrVector spotlightProxies;

      private:

         void PrepareSceneProxiesForRender();

         void DeferredLightPass_RenderThread(std::shared_ptr<CameraSceneProxy> cameraProxy,
            const DirectionalLightProxiesPtrVector& dirLightProxies,
            const PointLightProxiesPtrVector& pointLightProxies, 
            const SpotlightProxiesPtrVector& spotlightProxies);

         void DeferredBasePass_RenderThread(std::vector<PrimitiveSceneProxy*>& nonSkeletalMeshProxies, std::vector<SkeletalMeshSceneProxy*>& skeletalMeshProxies,
            std::shared_ptr<SceneView> sceneView);

         void ForwardBasePass_RenderThread(std::vector<PrimitiveSceneProxy*>& forwardedProxies, std::shared_ptr<SceneView> sceneView);

         void DepthPass(std::shared_ptr<SceneView> sceneView, std::vector<PrimitiveSceneProxy*>& shadowNonSkeletalMeshProxies, std::vector<SkeletalMeshSceneProxy*>& shadowSkeletalMeshProxies,
            DirectionalLightProxiesPtrVector& dirLightProxies,
            PointLightProxiesPtrVector& pointLightProxies,
            SpotlightProxiesPtrVector& spotlightProxies);

      public:

         DeferredShadingSceneRenderer(InterThreadCommunicationMgr& interThreadMgr);

			~DeferredShadingSceneRenderer();

         void PostLevelInit();

         void RenderScene_RenderThread();

         void PushRenderTargetToTextureRenderer();

         void SetProxiesAreDirty(const bool bDirty);

         void SetLightProxiesAreDirty(const bool bDirty);

#if DEBUG
         void SetDebugPhysicsRenderData(const DebugPhysicsRenderData& debugPhysicsRenderData);

         private:

         void DebugRenderPhysics(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

         void DebugFramePanelsPass();
#endif
		};

	}
}

