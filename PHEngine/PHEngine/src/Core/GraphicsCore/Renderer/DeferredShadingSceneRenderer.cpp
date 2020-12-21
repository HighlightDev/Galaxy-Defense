#include "DeferredShadingSceneRenderer.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GlobalInputController.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SkyboxSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Scene.h"

#include <gl/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <limits>

using namespace Resources;
using namespace Common;
using namespace Graphics;
using namespace Graphics::Renderer;
using namespace Graphics::Proxy;
using namespace Graphics::OpenGL;
using namespace EngineUtility;
using namespace Game;

namespace Graphics
{
   namespace Renderer
   {

      DeferredShadingSceneRenderer::DeferredShadingSceneRenderer(InterThreadCommunicationMgr& interThreadMgr)
         : SceneViews()
         , SceneProxies()
         , LightProxies()
         , m_interThreadMgr(interThreadMgr)
         , m_gbuffer(std::make_unique<DeferredShadingGBuffer>(GlobalInputController::GetInstance()->GetWindowWidth(), GlobalInputController::GetInstance()->GetWindowHeight()))
      {
         const auto& folderManager = FolderManager::GetInstance();

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

         mCompareShadowMapDescriptors = std::bind([](DirectionalLightSceneProxy* firstProxy,
            DirectionalLightSceneProxy* secondProxy) -> bool {
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

      void DeferredShadingSceneRenderer::PostLevelInit()
      {
         ENQUEUE_RENDER_THREAD_JOB(m_interThreadMgr, EnqueueJobPolicy::PUSH_ANYWAY,
            Job(0, 0, [=]()
         {
            for (auto& lightProxy : LightProxies)
            {
               lightProxy.second->PostLevelInit();
            }
         }));
      }

      void DeferredShadingSceneRenderer::DepthPass(std::shared_ptr<SceneView> sceneView, std::vector<PrimitiveSceneProxy*>& shadowNonSkeletalMeshProxies,
         std::vector<SkeletalMeshSceneProxy*>& shadowSkeletalMeshProxies,
         std::vector<DirectionalLightSceneProxy*>& dirLightProxies, std::vector<PointLightSceneProxy*>& pointLightProxies)
      {
         std::sort(dirLightProxies.begin(), dirLightProxies.end(), mCompareShadowMapDescriptors);

         size_t lastDirLightFramebufferDesc = UINT_MAX;

         for (auto& dirLightProxy : dirLightProxies)
         {
            DirectionalLightSceneProxy* lightPtr = dirLightProxy;

            if (lightPtr->IsEnabled())
            {
               ProjectedShadowInfo* const shadowInfo = lightPtr->GetProjectedDirShadowInfo();
               if (shadowInfo && shadowInfo->IsShadowMapDirty())
               {
                  shadowInfo->BindShadowFramebuffer(shadowInfo->GetAtlasResource()->GetTextureDescriptor() != lastDirLightFramebufferDesc);
                  lastDirLightFramebufferDesc = shadowInfo->GetAtlasResource()->GetTextureDescriptor();

                  DirectionalLightSceneProxy* dirLightPtr = lightPtr;

                  if (shadowNonSkeletalMeshProxies.size() > 0) // Non - skeletal proxies
                  {
                     m_depthShaderNonSkeletal->ExecuteShader();
                     for (auto& proxy : shadowNonSkeletalMeshProxies)
                     {
                        if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
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
                        if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
                        {
                           const auto& worldMatrix = proxy->GetMatrix();
                           const auto& viewMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowViewMatrix();
                           const auto& projectionMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowProjectionMatrix();
                           m_depthShaderSkeletal->SetTransformationMatrices(worldMatrix, viewMatrix, projectionMatrix);
                           m_depthShaderSkeletal->SetSkinningMatrices(proxy->GetSkinningMatrices());

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
         }

         for (auto& pointLightProxy : pointLightProxies)
         {
            PointLightSceneProxy* pointLightPtr = pointLightProxy;

            if (pointLightProxy->IsEnabled())
            {
               const auto& shadowInfo = pointLightPtr->GetProjectedPointShadowInfo();
               if (shadowInfo)
               {
                  shadowInfo->BindShadowFramebuffer(true); // every point light has it's own texture atlas 

                  if (shadowNonSkeletalMeshProxies.size() > 0) // Non - skeletal proxies
                  {
                     m_depthCubemapShaderNonSkeletal->ExecuteShader();
                     for (auto& proxy : shadowNonSkeletalMeshProxies)
                     {
                        if (proxy->IsEnabled()  && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
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
                        if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
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
      }

      void DeferredShadingSceneRenderer::DeferredBasePass_RenderThread(std::vector<PrimitiveSceneProxy*>& nonSkeletalMeshProxies,
         std::vector<SkeletalMeshSceneProxy*>& skeletalMeshProxies, std::shared_ptr<SceneView> sceneView)
      {
         auto cameraProxy = sceneView->GetCameraProxy();

         // Deferred shading collect info
         m_gbuffer->BindDeferredGBuffer();

         if (skeletalMeshProxies.size() > 0)
         {
            for (auto& proxy : skeletalMeshProxies)
            {
               if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
                  proxy->Render(cameraProxy->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
            }
         }

         if (nonSkeletalMeshProxies.size() > 0)
         {
            for (auto& proxy : nonSkeletalMeshProxies)
            {
               if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
                  proxy->Render(cameraProxy->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
            }
         }

         m_gbuffer->UnbindDeferredGBuffer();
      }

      void DeferredShadingSceneRenderer::DeferredLightPass_RenderThread(std::shared_ptr<CameraSceneProxy> cameraProxy,
         const std::vector<DirectionalLightSceneProxy*>& dirLightSourcesProxies, const std::vector<PointLightSceneProxy*>& pointLightSourcesProxies)
      {
         if (std::shared_ptr<Scene> scene = m_interThreadMgr.TryGetSceneWP().lock())
         {
            // TODO: Make some check if light source (point or spot light) is too far from current view position
            m_deferredLightShader->ExecuteShader();

#ifndef NO_LIT
            // ************************** SHADOWS ************************** //
            size_t pointLightIndex = 0, dirLightIndex = 0, shadowMapSlot = 3, dirShadowMapCount = 0, pointShadowMapCount = 0;
            for (auto& dirLightProxy : dirLightSourcesProxies)
            {
               if (dirLightProxy->IsEnabled())
               {
                  ProjectedDirShadowInfo* shadowInfo = dirLightProxy->GetProjectedDirShadowInfo();
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
            }

            for (auto& pointLightProxy : pointLightSourcesProxies)
            {
               if (pointLightProxy->IsEnabled())
               {
                  ProjectedPointShadowInfo* shadowInfo = pointLightProxy->GetProjectedPointShadowInfo();
                  if (shadowInfo)
                  {
                     shadowInfo->GetAtlasResource()->BindTexture(shadowMapSlot);
                     m_deferredLightShader->SetPointLightShadowMapSlot(pointLightIndex, shadowMapSlot);
                     m_deferredLightShader->SetPointLightShadowProjectionFarPlane(pointLightIndex, std::sqrtf(pointLightProxy->GetRadianceSqrRadius()));
                     shadowMapSlot++;
                     pointShadowMapCount++;
                     pointLightIndex++;
                  }
               }
            }

            m_deferredLightShader->SetCameraWorldPosition(cameraProxy->GetEyeVector());
            m_deferredLightShader->SetDirectionalLightShadowMapCount(dirShadowMapCount);
            m_deferredLightShader->SetPointLightShadowMapCount(pointShadowMapCount);
            // ************************** SHADOWS ************************** //
#endif

            m_gbuffer->BindPositionTexture(0);
            m_gbuffer->BindAlbedoWithSpecularTexture(1);
            m_gbuffer->BindNormalTexture(2);

            m_deferredLightShader->SetGBufferPosition(0);
            m_deferredLightShader->SetGBufferAlbedoNSpecular(1);
            m_deferredLightShader->SetGBufferNormal(2);

#ifndef NO_LIT
            m_deferredLightShader->SetLightsInfo(LightProxies);
#endif
            ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
            m_deferredLightShader->StopShader();
         }
      }

      void DeferredShadingSceneRenderer::ForwardBasePass_RenderThread(std::vector<PrimitiveSceneProxy*>& forwardedProxies,
         std::shared_ptr<SceneView> sceneView)
      {
         // Resolve depth buffer from gBuffer to default frame buffer

         int32_t windowWidth = GlobalInputController::GetInstance()->GetWindowWidth();
         int32_t windowHeight = GlobalInputController::GetInstance()->GetWindowHeight();

         auto cameraProxy = sceneView->GetCameraProxy();

         m_gbuffer->CopyFramebufferData(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT);

         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
         for (auto& proxy : forwardedProxies)
         {
            if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
               proxy->Render(sceneView->GetCameraProxy()->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
         }
         glDisable(GL_BLEND);


         DebugRenderPhysics(sceneView->GetCameraProxy()->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
      }

#if DEBUG
      void DeferredShadingSceneRenderer::DebugRenderPhysics(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         // todo: delete this crap and use buffers =\

         const auto& physicsRenderData = mDebugPhysicsRenderData.GetDebugLines();
         if (physicsRenderData.size())
         {
            float viewMatVec[16]{ 0.0f };
            const float *pSource = (const float*)glm::value_ptr(viewMatrix);
            for (int i = 0; i < 16; ++i)
               viewMatVec[i] = pSource[i];


            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(pSource);

            float projMatrix[16]{ 0.0f };
            pSource = (const float*)glm::value_ptr(projectionMatrix);
            for (int i = 0; i < 16; ++i)
               projMatrix[i] = pSource[i];

            glMatrixMode(GL_PROJECTION);
            glLoadMatrixf(projMatrix);

            glBegin(GL_LINES);
            for (size_t i = 0; i < physicsRenderData.size(); ++i)
            {
               glm::vec3 vert1 = physicsRenderData[i].first;
               glm::vec3 vert2 = physicsRenderData[i].second;

               glColor3f(1, 0, 0);
               glVertex3f(vert1.x, vert1.y, vert1.z);
               glVertex3f(vert2.x, vert2.y, vert2.z);
            }
            glEnd();

#if 0
            for (auto& proxy : SceneProxies)
            {
               if (proxy.second->GetPrimitiveProxyType() == PrimitiveProxyType::SKELETAL_MESH_PROXY || proxy.second->GetPrimitiveProxyType() == PrimitiveProxyType::STATIC_MESH_PROXY)
               {
                  const auto bb = proxy.second->GetTransformedBoundingBox();
                  const auto& positions = bb.GetBoundPositions();

                  std::vector<glm::vec3> points =
                  {
                     // Front
                     positions[0], positions[1], positions[2],
                     positions[2], positions[0], positions[3],

                     // Back
                     positions[4], positions[5], positions[6],
                     positions[6], positions[7], positions[4],

                     // Right
                     positions[3], positions[2], positions[6],
                     positions[6], positions[7], positions[3],

                     // Left
                     positions[0], positions[1], positions[5],
                     positions[5], positions[0], positions[4],

                     // Top
                     positions[4], positions[0], positions[3],
                     positions[3], positions[7], positions[4],

                     // Bottom
                     positions[5], positions[1], positions[2],
                     positions[2], positions[6], positions[5],
                  };

                  glBegin(GL_TRIANGLES);
                  for (size_t i = 0; i < points.size(); i += 3)
                  {
                     glm::vec3 vertex1 = points[i];
                     glm::vec3 vertex2 = points[i + 1];
                     glm::vec3 vertex3 = points[i + 2];

                     glColor3f(0.6f, 0.6f, 0.6f);
                     glVertex3f(vertex1.x, vertex1.y, vertex1.z);
                     glVertex3f(vertex2.x, vertex2.y, vertex2.z);
                     glVertex3f(vertex3.x, vertex3.y, vertex3.z);
                  }
                  glEnd();
         }
      }
#endif
   }
}
#endif

      void DeferredShadingSceneRenderer::PrepareSceneProxiesForRender()
      {
         if (bProxiesDirty)
         {
            forwardRenderingProxies.clear();
            skeletalProxies.clear();
            nonSkeletalProxies.clear();

            for (auto& proxy : SceneProxies)
            {
               PrimitiveSceneProxy* proxyPtr = proxy.second.get();

               if (proxyPtr->IsDeferred())
               {
                  if (proxyPtr->GetPrimitiveProxyType() == PrimitiveProxyType::SKELETAL_MESH_PROXY)
                     skeletalProxies.push_back(static_cast<SkeletalMeshSceneProxy*>(proxyPtr));
                  else
                     nonSkeletalProxies.push_back(proxyPtr);
               }
               else
               {
                  forwardRenderingProxies.push_back(proxyPtr);
               }
            }
            SetProxiesAreDirty(false);
         }

         if (bLightProxiesDirty)
         {
            dirLightProxies.clear();
            pointLightProxies.clear();

            for (auto& proxy : LightProxies)
            {
               LightSceneProxy* proxyPtr = proxy.second.get();

               if (proxyPtr->GetLightProxyType() == LightSceneProxyType::DIR_LIGHT)
               {
                  dirLightProxies.push_back(static_cast<DirectionalLightSceneProxy*>(proxyPtr));
               }
               else if (proxyPtr->GetLightProxyType() == LightSceneProxyType::POINT_LIGHT)
               {
                  pointLightProxies.push_back(static_cast<PointLightSceneProxy*>(proxyPtr));
               }
            }
         }
      }

      void DeferredShadingSceneRenderer::RenderScene_RenderThread()
      {
         if (std::shared_ptr<Scene> scene = m_interThreadMgr.TryGetSceneWP().lock())
         {
            PrepareSceneProxiesForRender();

            for (auto sceneViewPair : SceneViews)
            {
               auto sceneView = sceneViewPair.second;
               auto cameraProxy = sceneView->GetCameraProxy();

               sceneView->DoVisibilityTest();

               glEnable(GL_DEPTH_TEST);

               DepthPass(sceneView, nonSkeletalProxies, skeletalProxies, dirLightProxies, pointLightProxies);

               DeferredBasePass_RenderThread(nonSkeletalProxies, skeletalProxies, sceneView);

               DeferredLightPass_RenderThread(cameraProxy, dirLightProxies, pointLightProxies);

               if (forwardRenderingProxies.size())
                  ForwardBasePass_RenderThread(forwardRenderingProxies, sceneView);

               DebugFramePanelsPass();
            }
         }
      }

#if DEBUG
      void DeferredShadingSceneRenderer::SetDebugPhysicsRenderData(const DebugPhysicsRenderData& debugPhysicsRenderData)
      {
         mDebugPhysicsRenderData = debugPhysicsRenderData;
      }
#endif

      void DeferredShadingSceneRenderer::SetProxiesAreDirty(const bool bDirty)
      {
         bProxiesDirty = bDirty;
      }

      void DeferredShadingSceneRenderer::SetLightProxiesAreDirty(const bool bDirty)
      {
         bLightProxiesDirty = bDirty;
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