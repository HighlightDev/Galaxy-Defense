#pragma once

#include <stdint.h>
#include <glm/mat4x4.hpp>

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ShaderImplementation/DeferredLightShader.h"
#include "Core/GameCore/ShaderImplementation/DepthShader.h"
#include "Core/GameCore/ShaderImplementation/CubemapDepthShader.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingGBuffer.h"
#include "Core/InterThreadCommunicationMgr.h"
#include "Core/DebugCore/TextureRenderer.h"
#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"

#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"
#include "Core/GraphicsCore/Material/PBRMaterial.h"
#include "Core/GraphicsCore/OpenGL/Shader/MaterialShader.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkeletalMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"

using namespace Game::ShaderImpl;
using namespace Game;
using namespace Thread;
using namespace Debug;

namespace Graphics
{
	namespace Renderer
	{

		class DeferredShadingSceneRenderer
		{

      public:

         std::vector<std::shared_ptr<PrimitiveSceneProxy>> SceneProxies;

         std::vector<std::shared_ptr<LightSceneProxy>> LightProxies;

      private:

         InterThreadCommunicationMgr& m_interThreadMgr;

         /* Scene to render */
         std::unique_ptr<DeferredShadingGBuffer> m_gbuffer;

         // Shaders
         std::shared_ptr<DeferredLightShader> m_deferredLightShader;
         std::shared_ptr<DepthShader<true>> m_depthShaderSkeletal;
         std::shared_ptr<DepthShader<false>> m_depthShaderNonSkeletal;
         std::shared_ptr<CubemapDepthShader<true>> m_depthCubemapShaderSkeletal;
         std::shared_ptr<CubemapDepthShader<false>> m_depthCubemapShaderNonSkeletal;

         // Texture renderer
         TextureRenderer m_textureRenderer;

         std::function<bool(const std::shared_ptr<DirectionalLightSceneProxy>&, const std::shared_ptr<DirectionalLightSceneProxy>&)> mCompareShadowMapDescriptors;

         bool bProxiesDirty;

         bool bLightProxiesDirty;

      public:

         glm::mat4 ProjectionMatrix;

         std::vector<std::pair<glm::vec3, glm::vec3>> DebugLines;

         void loadDebugRender(const std::vector<std::pair<glm::vec3, glm::vec3>>& debugLines)
         {
            std::lock_guard<std::mutex> lock(lockDebug);
            DebugLines.clear();
            DebugLines = debugLines;
         }

         std::mutex lockDebug;

      private:

         void DebugFramePanelsPass();

         void DeferredLightPass_RenderThread(const std::vector<std::shared_ptr<LightSceneProxy>>& lightSourcesProxy);

         void DeferredBasePass_RenderThread(std::vector<PrimitiveSceneProxy*>& nonSkeletalMeshProxies, std::vector<PrimitiveSceneProxy*>& skeletalMeshProxies, const glm::mat4& viewMatrix);

         void ForwardBasePass_RenderThread(std::vector<PrimitiveSceneProxy*>& forwardedProxies, const glm::mat4& viewMatrix);

         void DepthPass(std::vector<PrimitiveSceneProxy*>& shadowNonSkeletalMeshProxies, std::vector<PrimitiveSceneProxy*>& shadowSkeletalMeshProxies, const std::vector<std::shared_ptr<LightSceneProxy>>& lightSourceProxies);

         std::vector<std::shared_ptr<DirectionalLightSceneProxy>> RetrieveDirectionalLightProxies(const std::vector<std::shared_ptr<LightSceneProxy>>& lightSourceProxies) const;
         std::vector<std::shared_ptr<PointLightSceneProxy>> RetrievePointLightProxies(const std::vector<std::shared_ptr<LightSceneProxy>>& lightSourceProxies) const;

      public:

         DeferredShadingSceneRenderer(InterThreadCommunicationMgr& interThreadMgr);

			~DeferredShadingSceneRenderer();

         void PostConstructorInitialize();

         void RenderScene_RenderThread();

         void PushRenderTargetToTextureRenderer();

         void SetProxiesAreDirty(const bool bDirty);

         void SetLightProxiesAreDirty(const bool bDirty);

         private:

         void DebugRenderPhysics(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
		};

	}
}

