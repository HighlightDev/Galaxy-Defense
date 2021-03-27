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
#include "Core/GraphicsCore/SceneProxy/PlanarReflectionProxy.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/MaterialShader.h"
#include "Core/GraphicsCore/SceneViewInfo/SceneView.h"
#include "Core/GraphicsCore/Renderer/RenderState.h"

#include "Core/GameCore/ShaderImplementation/DeferredLightShader.h"
#include "Core/GameCore/ShaderImplementation/DirectionalLightDepthShader.h"
#include "Core/GameCore/ShaderImplementation/PointLightDepthShader.h"
#include "Core/GameCore/ShaderImplementation/SpotlightDepthShader.h"
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

         std::unordered_map<size_t /*camera proxy id*/, std::shared_ptr<SceneView>> SceneViewsMap;

         std::unordered_map<size_t /*proxy id*/, std::shared_ptr<PrimitiveSceneProxy>> SceneProxiesMap;

         std::unordered_map<size_t /*proxy id*/, std::shared_ptr<LightSceneProxy>> LightProxiesMap;

         std::unordered_map<size_t /*material proxy id*/, std::shared_ptr<MaterialProxy>> MaterialProxiesMap;

         std::unordered_map<size_t /*planar reflection proxy id*/, std::shared_ptr<PlanarReflectionProxy>> PlanarReflectionProxiesMap;

      private:

         InterThreadCommunicationMgr& m_interThreadMgr;

         /* G-buffers */
         std::unique_ptr<DeferredShadingGBuffer> m_gbuffer;

         // Shaders
         std::shared_ptr<DeferredLightShader> m_deferredLightShader;
         std::shared_ptr<DirectionalLightDepthShader<eShaderMeshType::SKELETAL>> mDLDepthShaderSkeletal;
         std::shared_ptr<DirectionalLightDepthShader<eShaderMeshType::NON_SKELETAL>> mDLDepthShaderNonSkeletal;
         std::shared_ptr<PointLightDepthShader<eShaderMeshType::SKELETAL>> mPLDepthShaderSkeletal;
         std::shared_ptr<PointLightDepthShader<eShaderMeshType::NON_SKELETAL>> mPLDepthShaderNonSkeletal;
         std::shared_ptr<SpotlightDepthShader<eShaderMeshType::SKELETAL>> mSLDepthShaderSkeletal;
         std::shared_ptr<SpotlightDepthShader<eShaderMeshType::NON_SKELETAL>> mSLDepthShaderNonSkeletal;

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

         std::unordered_map<uint32_t, std::vector<LightSceneProxy*>> mGroupedByShadowAtlasLights;

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

         void PushRenderTargetToTextureRenderer();

         void SetProxiesAreDirty(const bool bDirty);

         void SetLightProxiesAreDirty(const bool bDirty);

         void SetPlanarReflectionProxiesAreDirty(const bool bDirty);

#if DEBUG
         void SetDebugPhysicsRenderData(const DebugPhysicsRenderData& debugPhysicsRenderData);

         private:

         void DebugRenderPhysics(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

         void DebugFramePanelsPass();
#endif
		};

	}
}

