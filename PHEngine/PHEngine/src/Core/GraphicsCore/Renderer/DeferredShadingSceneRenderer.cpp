#include "DeferredShadingSceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/CommonCore/FolderManager.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"
#include "Core/GameCore/ICamera.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GameCore/GlobalInputController.h"
#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SkyboxSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"
#include "Core/UtilityCore/EngineMath.h"

#include <gl/glew.h>

using namespace Resources;
using namespace Common; 
using namespace Graphics;
using namespace Graphics::Renderer;
using namespace Graphics::Proxy;
using namespace Graphics::OpenGL;
using namespace EngineUtility;

namespace Graphics
{
   namespace Renderer
   {

      DeferredShadingSceneRenderer::DeferredShadingSceneRenderer(InterThreadCommunicationMgr& interThreadMgr, std::weak_ptr<Level> level)
         : m_interThreadMgr(interThreadMgr)
         , mLevel(level)
         , m_gbuffer(std::make_unique<DeferredShadingGBuffer>(GlobalInputController::GetInstance()->GetWindowWidth(), GlobalInputController::GetInstance()->GetWindowHeight()))
      {
         const auto& folderManager = FolderManager::GetInstance();

         const float aspectRatio = 16.0f / 9.0f;
         ProjectionMatrix = glm::perspective<float>(DEG_TO_RAD(60), aspectRatio, 1, 1000);

         ShaderParams shaderParams3("DeferredLight Shader", folderManager->GetShadersPath() + "deferredLightPassVS.glsl", folderManager->GetShadersPath() + "deferredLightPassFS.glsl", "", "", "", "");
         ShaderParams shaderParams4("DepthSkeletal Shader", folderManager->GetShadersPath() + "basicShadowSkeletalVS.glsl", folderManager->GetShadersPath() + "basicShadowFS.glsl", "", "", "", "");
         ShaderParams shaderParams5("DepthNonSkeletal Shader", folderManager->GetShadersPath() + "basicShadowNonSkeletalVS.glsl", folderManager->GetShadersPath() + "basicShadowFS.glsl", "", "", "", "");
         ShaderParams shaderParams6("CubemapDepthSkeletal Shader", folderManager->GetShadersPath() + "cubemapShadowSkeletalVS.glsl", folderManager->GetShadersPath() + "cubemapShadowFS.glsl", folderManager->GetShadersPath() + "cubemapShadowGS.glsl", "", "", "");
         ShaderParams shaderParams7("CubemapDepthNonSkeletal Shader", folderManager->GetShadersPath() + "cubemapShadowNonSkeletalVS.glsl", folderManager->GetShadersPath() + "cubemapShadowFS.glsl", folderManager->GetShadersPath() + "cubemapShadowGS.glsl", "", "", "");

         m_deferredLightShader = std::static_pointer_cast<DeferredLightShader>(ShaderPool::GetInstance()-> template GetOrAllocateResource<DeferredLightShader>(shaderParams3));
         m_depthShaderSkeletal = std::static_pointer_cast<DepthShader<true>>(ShaderPool::GetInstance()->template GetOrAllocateResource<DepthShader<true>>(shaderParams4));
         m_depthShaderNonSkeletal = std::static_pointer_cast<DepthShader<false>>(ShaderPool::GetInstance()->template GetOrAllocateResource<DepthShader<false>>(shaderParams5));
         m_depthCubemapShaderSkeletal = std::static_pointer_cast<CubemapDepthShader<true>>(ShaderPool::GetInstance()->template GetOrAllocateResource< CubemapDepthShader<true>>(shaderParams6));
         m_depthCubemapShaderNonSkeletal = std::static_pointer_cast<CubemapDepthShader<false>>(ShaderPool::GetInstance()->template GetOrAllocateResource< CubemapDepthShader<true>>(shaderParams7));

         mCompareShadowMapDescriptors = std::bind([](const std::shared_ptr<DirectionalLightSceneProxy>& firstProxy,
            const std::shared_ptr<DirectionalLightSceneProxy>& secondProxy) -> bool {
            bool result = false;
            const auto& shadowInfo1 = firstProxy->GetProjectedDirShadowInfo();
            const auto& shadowInfo2 = secondProxy->GetProjectedDirShadowInfo();
            if (shadowInfo1 && shadowInfo2)
            {
               const uint32_t desc1 = shadowInfo1->GetAtlasResource()->GetTextureDescriptor();
               const uint32_t desc2 = shadowInfo2->GetAtlasResource()->GetTextureDescriptor();
               result = desc1 > desc2;
            }
            else if (shadowInfo1 && !shadowInfo2)
            {
               result = true;
            }

            return result;
         }, std::placeholders::_1, std::placeholders::_2);
      }

      DeferredShadingSceneRenderer::~DeferredShadingSceneRenderer()
      {
      }

      void DeferredShadingSceneRenderer::PostConstructorInitialize()
      {
         
      }

      std::vector<std::shared_ptr<DirectionalLightSceneProxy>> DeferredShadingSceneRenderer::RetrieveDirectionalLightProxies(const std::vector<std::shared_ptr<LightSceneProxy>>& lightSourcesProxy) const
      {
         std::vector<std::shared_ptr<DirectionalLightSceneProxy>> directionalLights;

         for (const auto& lightProxy : lightSourcesProxy)
         {
            if (LightSceneProxyType::DIR_LIGHT == lightProxy->GetLightProxyType())
               directionalLights.push_back(std::static_pointer_cast<DirectionalLightSceneProxy>(lightProxy));
         }
         return directionalLights;
      }

      std::vector<std::shared_ptr<PointLightSceneProxy>> DeferredShadingSceneRenderer::RetrievePointLightProxies(const std::vector<std::shared_ptr<LightSceneProxy>>& lightSourcesProxy) const
      {
         std::vector<std::shared_ptr<PointLightSceneProxy>> pointLights;

         for (const auto& lightProxy : lightSourcesProxy)
         {
            if (LightSceneProxyType::POINT_LIGHT == lightProxy->GetLightProxyType())
               pointLights.push_back(std::static_pointer_cast<PointLightSceneProxy>(lightProxy));
         }
         return pointLights;
      }

      void DeferredShadingSceneRenderer::DepthPass(std::vector<PrimitiveSceneProxy*>& shadowNonSkeletalMeshProxies, std::vector<PrimitiveSceneProxy*>& shadowSkeletalMeshProxies, const std::vector<std::shared_ptr<LightSceneProxy>>& lightSourcesProxy)
      {
         auto dirLightProxies = RetrieveDirectionalLightProxies(lightSourcesProxy);
         auto pointLightProxies = RetrievePointLightProxies(lightSourcesProxy);

         std::sort(dirLightProxies.begin(), dirLightProxies.end(), mCompareShadowMapDescriptors);

         const auto firstDirLightProxyWithShadowInfo = std::find_if(dirLightProxies.begin(), dirLightProxies.end(), [](const std::shared_ptr<DirectionalLightSceneProxy>& proxy) { return proxy->GetProjectedDirShadowInfo() != nullptr; });
         uint32_t lastDirLightFramebufferDesc = std::numeric_limits<uint32_t>::max();

         for (auto& dirLightProxy : dirLightProxies)
         {
            DirectionalLightSceneProxy* lightPtr = dirLightProxy.get();

            ProjectedShadowInfo* const shadowInfo = lightPtr->GetProjectedDirShadowInfo();
            if (shadowInfo && shadowInfo->IsShadowMapDirty())
            {
               shadowInfo->BindShadowFramebuffer(shadowInfo->GetAtlasResource()->GetTextureDescriptor() != lastDirLightFramebufferDesc);
               lastDirLightFramebufferDesc = shadowInfo->GetAtlasResource()->GetTextureDescriptor();

               DirectionalLightSceneProxy* dirLightPtr = static_cast<DirectionalLightSceneProxy*>(lightPtr);

               if (shadowNonSkeletalMeshProxies.size() > 0) // Non - skeletal proxies
               {
                  m_depthShaderNonSkeletal->ExecuteShader();
                  for (auto& proxy : shadowNonSkeletalMeshProxies)
                  {
                     if (proxy->IsVisible())
                     {
                        const auto& worldMatrix = proxy->GetMatrix();
                        const auto& viewMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowViewMatrix();
                        const auto& projectionMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowProjectionMatrix();

                        m_depthShaderNonSkeletal->SetTransformationMatrices(worldMatrix, viewMatrix, projectionMatrix);

                        proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                     }
                  }
                  m_depthShaderNonSkeletal->StopShader();
               }

               if (shadowSkeletalMeshProxies.size() > 0) // Skeletal proxies
               {
                  m_depthShaderSkeletal->ExecuteShader();
                  for (auto& proxy : shadowSkeletalMeshProxies)
                  {
                     if (proxy->IsVisible())
                     {
                        SkeletalMeshSceneProxy* skeletalProxy = static_cast<SkeletalMeshSceneProxy*>(proxy);

                        const auto& worldMatrix = proxy->GetMatrix();
                        const auto& viewMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowViewMatrix();
                        const auto& projectionMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowProjectionMatrix();
                        m_depthShaderSkeletal->SetTransformationMatrices(worldMatrix, viewMatrix, projectionMatrix);
                        m_depthShaderSkeletal->SetSkinningMatrices(skeletalProxy->GetSkinningMatrices());

                        proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                     }
                  }
                  m_depthShaderSkeletal->StopShader();
               }

               glBindFramebuffer(GL_FRAMEBUFFER, 0);

               // Next frame shadow map will not be updated unless position of objects in the level are changed
               shadowInfo->SetIsShadowMapDirty(false);
            }
         }

         for (auto& pointLightProxy : pointLightProxies)
         {
            PointLightSceneProxy* pointLightPtr = static_cast<PointLightSceneProxy*>(pointLightProxy.get());

            const auto& shadowInfo = pointLightPtr->GetProjectedPointShadowInfo();
            if (shadowInfo)
            {
               shadowInfo->BindShadowFramebuffer(true); // every point light has it's own texture atlas 
               
               if (shadowNonSkeletalMeshProxies.size() > 0) // Non - skeletal proxies
               {
                  m_depthCubemapShaderNonSkeletal->ExecuteShader();
                  for (auto& proxy : shadowNonSkeletalMeshProxies)
                  {
                     if (proxy->IsVisible())
                     {
                        const auto& worldMatrix = proxy->GetMatrix();
                        const auto& viewMatrices = shadowInfo->GetShadowViewMatrices();
                        const auto& projectionMatrices = shadowInfo->GetShadowProjectionMatrices();

                        m_depthCubemapShaderNonSkeletal->SetTransformationMatrices(worldMatrix, viewMatrices, projectionMatrices);
                        m_depthCubemapShaderNonSkeletal->SetFarPlane(std::sqrtf(pointLightPtr->GetRadianceSqrRadius()));
                        m_depthCubemapShaderNonSkeletal->SetPointLightPosition(pointLightPtr->GetPosition());

                        proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                     }
                  }
                  m_depthCubemapShaderNonSkeletal->StopShader();
               }
               if (shadowSkeletalMeshProxies.size() > 0) // Skeletal proxies
               {
                  m_depthCubemapShaderSkeletal->ExecuteShader();
                  for (auto& proxy : shadowSkeletalMeshProxies)
                  {
                     if (proxy->IsVisible())
                     {
                        SkeletalMeshSceneProxy* skeletalProxy = static_cast<SkeletalMeshSceneProxy*>(proxy);

                        const auto& worldMatrix = skeletalProxy->GetMatrix();
                        const auto& viewMatrices = shadowInfo->GetShadowViewMatrices();
                        const auto& projectionMatrices = shadowInfo->GetShadowProjectionMatrices();

                        m_depthCubemapShaderSkeletal->SetTransformationMatrices(worldMatrix, viewMatrices, projectionMatrices);
                        m_depthCubemapShaderSkeletal->SetFarPlane(std::sqrtf(pointLightPtr->GetRadianceSqrRadius()));
                        m_depthCubemapShaderSkeletal->SetPointLightPosition(pointLightPtr->GetPosition());
                        m_depthCubemapShaderSkeletal->SetSkinningMatrices(skeletalProxy->GetSkinningMatrices());

                        skeletalProxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                     }
                  }
                  m_depthCubemapShaderSkeletal->StopShader();
               }
               glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
         }
      }

      void DeferredShadingSceneRenderer::DeferredBasePass_RenderThread(std::vector<PrimitiveSceneProxy*>& nonSkeletalMeshProxies, std::vector<PrimitiveSceneProxy*>& skeletalMeshProxies, glm::mat4& viewMatrix)
      {
         if (std::shared_ptr<Level> level = mLevel.lock())
         {
            // Deferred shading collect info
            m_gbuffer->BindDeferredGBuffer();

            glm::mat4 projectionMatrix = ProjectionMatrix;

            if (skeletalMeshProxies.size() > 0)
            {
               for (auto& proxy : skeletalMeshProxies)
               {
                  if (proxy->IsVisible())
                     proxy->Render(viewMatrix, projectionMatrix);
               }
            }

            if (nonSkeletalMeshProxies.size() > 0)
            {
               for (auto& proxy : nonSkeletalMeshProxies)
               {
                  if (proxy->IsVisible())
                     proxy->Render(viewMatrix, projectionMatrix);
               }
            }

            m_gbuffer->UnbindDeferredGBuffer();
         }
      }

      void DeferredShadingSceneRenderer::DeferredLightPass_RenderThread(const std::vector<std::shared_ptr<LightSceneProxy>>& lightSourcesProxy)
      {
         if (std::shared_ptr<Level> level = mLevel.lock())
         {
            // TODO: Make some check if light source (point or spot light) is too far from current view position
            m_deferredLightShader->ExecuteShader();

            // ************************** SHADOWS ************************** //
            size_t pointLightIndex = 0, dirLightIndex = 0, shadowMapSlot = 3, dirShadowMapCount = 0, pointShadowMapCount = 0;
            for (auto& lightProxy : lightSourcesProxy)
            {
               if (LightSceneProxyType::DIR_LIGHT == lightProxy->GetLightProxyType())
               {
                  DirectionalLightSceneProxy* lightPtr = static_cast<DirectionalLightSceneProxy*>(lightProxy.get());
                  ProjectedDirShadowInfo* shadowInfo = lightPtr->GetProjectedDirShadowInfo();
                  if (shadowInfo)
                  {
                     shadowInfo->GetAtlasResource()->BindTexture(shadowMapSlot);
                     m_deferredLightShader->SetDirectionalLightShadowMapSlot(dirLightIndex, shadowMapSlot, shadowInfo->GetPosOffsetShadowMapAtlas());
                     m_deferredLightShader->SetDirectionalLightShadowMatrix(dirLightIndex, shadowInfo->GetShadowMatrix());

                     dirShadowMapCount++;
                     dirLightIndex++;
                     shadowMapSlot++;
                  }
               }
               else if (LightSceneProxyType::POINT_LIGHT == lightProxy->GetLightProxyType())
               {
                  PointLightSceneProxy* lightPtr = static_cast<PointLightSceneProxy*>(lightProxy.get());
                  ProjectedPointShadowInfo* shadowInfo = lightPtr->GetProjectedPointShadowInfo();
                  if (shadowInfo)
                  {
                     shadowInfo->GetAtlasResource()->BindTexture(shadowMapSlot);
                     m_deferredLightShader->SetPointLightShadowMapSlot(pointLightIndex, shadowMapSlot);
                     m_deferredLightShader->SetPointLightShadowProjectionFarPlane(pointLightIndex, std::sqrtf(lightPtr->GetRadianceSqrRadius()));
                     shadowMapSlot++;
                     pointShadowMapCount++;
                     pointLightIndex++;
                  }
               }
            }
            m_deferredLightShader->SetCameraWorldPosition(level->GetCamera()->GetEyeVector());
            m_deferredLightShader->SetDirectionalLightShadowMapCount(dirShadowMapCount);
            m_deferredLightShader->SetPointLightShadowMapCount(pointShadowMapCount);
            // ************************** SHADOWS ************************** //

            m_gbuffer->BindPositionTexture(0);
            m_gbuffer->BindAlbedoWithSpecularTexture(1);
            m_gbuffer->BindNormalTexture(2);

            m_deferredLightShader->SetGBufferPosition(0);
            m_deferredLightShader->SetGBufferAlbedoNSpecular(1);
            m_deferredLightShader->SetGBufferNormal(2);

            m_deferredLightShader->SetLightsInfo(level->GetLightProxies());
            ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
            m_deferredLightShader->StopShader();
         }
      }

      void DeferredShadingSceneRenderer::ForwardBasePass_RenderThread(std::vector<PrimitiveSceneProxy*>& forwardedProxies, const glm::mat4& viewMatrix)
      { 
         // Resolve depth buffer from gBuffer to default frame buffer

         int32_t windowWidth = GlobalInputController::GetInstance()->GetWindowWidth();
         int32_t windowHeight = GlobalInputController::GetInstance()->GetWindowHeight();

         glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbuffer->GetFramebufferDesc());
         glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
         glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
         glBindFramebuffer(GL_FRAMEBUFFER, 0);

         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
         for (auto& proxy : forwardedProxies)
         {
            if (proxy->IsVisible())
               proxy->Render(const_cast<glm::mat4&>(viewMatrix), ProjectionMatrix); // TODO: remove from scene projection matrix and camera to render thread (I think)
         }
         glDisable(GL_BLEND);
      }

      void DeferredShadingSceneRenderer::RenderScene_RenderThread()
      {

         // TODO: fill deferred & nondeferred arrays and so on only once!

         if (std::shared_ptr<Level> level = mLevel.lock())
         {

            glEnable(GL_DEPTH_TEST);

            /***** Access view matrix from GAME THREAD  *****/
            glm::mat4 viewMatrix = level->GetCamera()->GetViewMatrix();

            std::vector<PrimitiveSceneProxy*> drawDeferredShadedProxies;
            std::vector<PrimitiveSceneProxy*> drawForwardShadedProxies;

            for (auto& proxy : level->GetSceneProxies())
            {
               if (proxy->IsDeferred())
                  drawDeferredShadedProxies.push_back(proxy.get());
               else
                  drawForwardShadedProxies.push_back(proxy.get());
            }

            const bool bIsForwardShadedProxies = drawForwardShadedProxies.size() > 0;

            std::vector<PrimitiveSceneProxy*> skeletalProxies, nonSkeletalProxies;
            for (auto& proxy : drawDeferredShadedProxies)
            {
               if (proxy->GetComponentType() == SKELETAL_MESH_COMPONENT)
               {
                  skeletalProxies.push_back(proxy);
               }
               else
               {
                  nonSkeletalProxies.push_back(proxy);
               }
            }

            DepthPass(nonSkeletalProxies, skeletalProxies, level->GetLightProxies());

            DeferredBasePass_RenderThread(nonSkeletalProxies, skeletalProxies, viewMatrix);

            DeferredLightPass_RenderThread(level->GetLightProxies());

            if (bIsForwardShadedProxies)
            {
               ForwardBasePass_RenderThread(drawForwardShadedProxies, viewMatrix);
            }

            DebugFramePanelsPass();
         }
      }

      void DeferredShadingSceneRenderer::DebugFramePanelsPass()
      {
         glDisable(GL_DEPTH_TEST);
         m_textureRenderer.RenderFrames(m_gbuffer);
      }

      void DeferredShadingSceneRenderer::PushRenderTargetToTextureRenderer()
      {
         m_textureRenderer.PushDebugRenderTarget();
      }
	}
}