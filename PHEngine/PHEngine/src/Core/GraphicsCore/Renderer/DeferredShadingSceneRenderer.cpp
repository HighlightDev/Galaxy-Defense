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
         : SceneViewsMap()
         , bLightProxiesDirty(false)
         , bProxiesDirty(false)
         , bPlanarReflectionProxiesDirty(false)
         , SceneProxiesMap()
         , LightProxiesMap()
         , m_interThreadMgr(interThreadMgr)
         , m_gbuffer(
            std::make_unique<DeferredShadingGBuffer>(ViewPortInfo(0, 0,
               GlobalInputController::GetInstance()->GetWindowWidth(),
               GlobalInputController::GetInstance()->GetWindowHeight())))
      {
         const auto& folderManager = FolderManager::GetInstance();

         ShaderParams shaderParams3("DeferredLight Shader", folderManager->GetShadersPath() + "deferredLightPassVS.glsl", folderManager->GetShadersPath() + "deferredLightPassFS.glsl", "", "", "", "");
         ShaderParams shaderParams4("DirectionalLightDepthShader<eShaderMeshType::SKELETAL>", folderManager->GetShadersPath() + "directLightDepthCollectSkeletalVS.glsl", folderManager->GetShadersPath() + "directLightDepthCollectFS.glsl", "", "", "", "");
         ShaderParams shaderParams5("DirectionalLightDepthShader<eShaderMeshType::NON_SKELETAL>", folderManager->GetShadersPath() + "directLightDepthCollectNonSkeletalVS.glsl", folderManager->GetShadersPath() + "directLightDepthCollectFS.glsl", "", "", "", "");
         ShaderParams shaderParams6("PointLightDepthShader<eShaderMeshType::SKELETAL>", folderManager->GetShadersPath() + "pointLightDepthCollectSkeletalVS.glsl", folderManager->GetShadersPath() + "pointLightDepthCollectFS.glsl", folderManager->GetShadersPath() + "pointLightDepthCollectGS.glsl", "", "", "");
         ShaderParams shaderParams7("PointLightDepthShader<eShaderMeshType::NON_SKELETAL>", folderManager->GetShadersPath() + "pointLightDepthCollectNonSkeletalVS.glsl", folderManager->GetShadersPath() + "pointLightDepthCollectFS.glsl", folderManager->GetShadersPath() + "pointLightDepthCollectGS.glsl", "", "", "");

         ShaderParams shaderParams8("SpotlightDepthShader<eShaderMeshType::SKELETAL>", folderManager->GetShadersPath() + "spotlightDepthCollectSkeletalVS.glsl", folderManager->GetShadersPath() + "spotlightDepthCollectFS.glsl", "", "", "", "");
         ShaderParams shaderParams9("SpotlightDepthShader<eShaderMeshType::NON_SKELETAL>", folderManager->GetShadersPath() + "spotlightDepthCollectNonSkeletalVS.glsl", folderManager->GetShadersPath() + "spotlightDepthCollectFS.glsl", "", "", "", "");

         m_deferredLightShader = std::static_pointer_cast<DeferredLightShader>(ShaderPool::GetInstance()-> template GetOrAllocateResource<DeferredLightShader>(shaderParams3));
         mDLDepthShaderSkeletal = std::static_pointer_cast<DirectionalLightDepthShader<eShaderMeshType::SKELETAL>>(ShaderPool::GetInstance()->template GetOrAllocateResource<DirectionalLightDepthShader<eShaderMeshType::SKELETAL>>(shaderParams4));
         mDLDepthShaderNonSkeletal = std::static_pointer_cast<DirectionalLightDepthShader<eShaderMeshType::NON_SKELETAL>>(ShaderPool::GetInstance()->template GetOrAllocateResource<DirectionalLightDepthShader<eShaderMeshType::NON_SKELETAL>>(shaderParams5));
         mPLDepthShaderSkeletal = std::static_pointer_cast<PointLightDepthShader<eShaderMeshType::SKELETAL>>(ShaderPool::GetInstance()->template GetOrAllocateResource<PointLightDepthShader<eShaderMeshType::SKELETAL>>(shaderParams6));
         mPLDepthShaderNonSkeletal = std::static_pointer_cast<PointLightDepthShader<eShaderMeshType::NON_SKELETAL>>(ShaderPool::GetInstance()->template GetOrAllocateResource<PointLightDepthShader<eShaderMeshType::NON_SKELETAL>>(shaderParams7));
         mSLDepthShaderSkeletal = std::static_pointer_cast<SpotlightDepthShader<eShaderMeshType::SKELETAL>>(ShaderPool::GetInstance()->template GetOrAllocateResource<SpotlightDepthShader<eShaderMeshType::SKELETAL>>(shaderParams8));
         mSLDepthShaderNonSkeletal = std::static_pointer_cast<SpotlightDepthShader<eShaderMeshType::NON_SKELETAL>>(ShaderPool::GetInstance()->template GetOrAllocateResource<SpotlightDepthShader<eShaderMeshType::NON_SKELETAL>>(shaderParams9));
      }

      DeferredShadingSceneRenderer::~DeferredShadingSceneRenderer()
      {
      }

      void DeferredShadingSceneRenderer::PostLevelInit()
      {
         m_interThreadMgr.EmplaceRenderThreadJob(EnqueueJobPolicy::PUSH_ANYWAY,
            Job(0, 0, [=]()
         {
            for (auto& lightProxy : LightProxiesMap)
            {
               lightProxy.second->PostLevelInit();
            }
         }));
      }

      void DeferredShadingSceneRenderer::DepthPass(std::shared_ptr<SceneView> sceneView)
      {
         glEnable(GL_CULL_FACE);
         glFrontFace(GL_CCW);
         glCullFace(GL_BACK);
         RenderState renderState(std::make_shared<DepthStencilState<true>>(),
            std::make_shared<BlendingState<false>>());
         renderState.BindRenderState();

         for (auto& atlasLightGroup : mGroupedByShadowAtlasLights)
         {
            bool bNewDepthShadowAtlas = true;

            for (auto& lightProxyPtr : atlasLightGroup.second)
            {
               auto lightProxyType = lightProxyPtr->GetLightProxyType();

               if (lightProxyType == LightSceneProxyType::DIR_LIGHT)
               {
                  DirectionalLightSceneProxy* lightPtr = static_cast<DirectionalLightSceneProxy*>(lightProxyPtr);

                  if (lightPtr->IsEnabled())
                  {
                     ProjectedShadowInfo* const shadowInfo = lightPtr->GetProjectedDirShadowInfo();
                     if (shadowInfo && shadowInfo->IsShadowMapDirty())
                     {
                        shadowInfo->BindShadowFramebuffer(true, bNewDepthShadowAtlas);

                        DirectionalLightSceneProxy* dirLightPtr = lightPtr;

                        if (mNonSkeletalProxiesVec.size() > 0) // Non - skeletal proxies
                        {
                           mDLDepthShaderNonSkeletal->ExecuteShader();
                           for (auto& proxy : mNonSkeletalProxiesVec)
                           {
                              if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
                              {
                                 const auto& worldMatrix = proxy->GetMatrix();
                                 const auto& viewMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowViewMatrix();
                                 const auto& projectionMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowProjectionMatrix();

                                 mDLDepthShaderNonSkeletal->SetTransformationMatrices(worldMatrix, viewMatrix, projectionMatrix);

                                 proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                              }
                           }
                           mDLDepthShaderNonSkeletal->StopShader();
                        }

                        if (mSkeletalProxiesVec.size() > 0) // Skeletal proxies
                        {
                           mDLDepthShaderSkeletal->ExecuteShader();
                           for (auto& proxy : mSkeletalProxiesVec)
                           {
                              if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
                              {
                                 const auto& worldMatrix = proxy->GetMatrix();
                                 const auto& viewMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowViewMatrix();
                                 const auto& projectionMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowProjectionMatrix();
                                 mDLDepthShaderSkeletal->SetTransformationMatrices(worldMatrix, viewMatrix, projectionMatrix);
                                 mDLDepthShaderSkeletal->SetSkinningMatrices(proxy->GetSkinningMatrices());

                                 proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                              }
                           }
                           mDLDepthShaderSkeletal->StopShader();
                        }

                        // Next frame shadow map will not be updated unless position of objects in the level are changed
                        shadowInfo->SetIsShadowMapDirty(false);
                     }
                  }
               }
               else if (lightProxyType == LightSceneProxyType::SPOT_LIGHT)
               {
                  SpotlightSceneProxy* spotlightPtr = static_cast<SpotlightSceneProxy*>(lightProxyPtr);

                  if (spotlightPtr->IsEnabled())
                  {
                     const auto& shadowInfo = spotlightPtr->GetProjectedSpotLightShadowInfo();
                     if (shadowInfo && shadowInfo->IsShadowMapDirty())
                     {
                        shadowInfo->BindShadowFramebuffer(true, bNewDepthShadowAtlas);

                        if (mNonSkeletalProxiesVec.size() > 0) // Non - skeletal proxies
                        {
                           mSLDepthShaderNonSkeletal->ExecuteShader();
                           for (auto& proxy : mNonSkeletalProxiesVec)
                           {
                              if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
                              {
                                 const auto& worldMatrix = proxy->GetMatrix();
                                 const auto& viewMatrix = shadowInfo->GetShadowViewMatrix();
                                 const auto& projectionMatrix = shadowInfo->GetShadowProjectionMatrix();

                                 mSLDepthShaderNonSkeletal->SetTransformationMatrices(worldMatrix, viewMatrix, projectionMatrix);
                                 mSLDepthShaderNonSkeletal->SetFarPlane(spotlightPtr->GetRadianceRadius());
                                 mSLDepthShaderNonSkeletal->SetSpotlightPosition(spotlightPtr->GetPosition());

                                 proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                              }
                           }
                           mSLDepthShaderNonSkeletal->StopShader();
                        }
                        if (mSkeletalProxiesVec.size() > 0) // Skeletal proxies
                        {
                           mSLDepthShaderSkeletal->ExecuteShader();
                           for (auto& proxy : mSkeletalProxiesVec)
                           {
                              if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
                              {
                                 SkeletalMeshSceneProxy* skeletalProxy = static_cast<SkeletalMeshSceneProxy*>(proxy);

                                 const auto& worldMatrix = skeletalProxy->GetMatrix();
                                 const auto& viewMatrices = shadowInfo->GetShadowViewMatrix();
                                 const auto& projectionMatrices = shadowInfo->GetShadowProjectionMatrix();

                                 mSLDepthShaderSkeletal->SetTransformationMatrices(worldMatrix, viewMatrices, projectionMatrices);
                                 mSLDepthShaderSkeletal->SetFarPlane(spotlightPtr->GetRadianceRadius());
                                 mSLDepthShaderSkeletal->SetSpotlightPosition(spotlightPtr->GetPosition());
                                 mSLDepthShaderSkeletal->SetSkinningMatrices(skeletalProxy->GetSkinningMatrices());

                                 skeletalProxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                              }
                           }
                           mSLDepthShaderSkeletal->StopShader();
                        }
                        glBindFramebuffer(GL_FRAMEBUFFER, 0);

                        // Next frame shadow map will not be updated unless position of objects in the level are changed
                        shadowInfo->SetIsShadowMapDirty(false);
                     }
                  }
               }
               else if (lightProxyType == LightSceneProxyType::POINT_LIGHT)
               {
                  PointLightSceneProxy* pointLightPtr = static_cast<PointLightSceneProxy*>(lightProxyPtr);

                  if (pointLightPtr->IsEnabled())
                  {
                     const auto& shadowInfo = pointLightPtr->GetProjectedPointShadowInfo();
                     if (shadowInfo && shadowInfo->IsShadowMapDirty())
                     {
                        shadowInfo->BindShadowFramebuffer(true, true); // every point light has it's own texture atlas 

                        if (mNonSkeletalProxiesVec.size() > 0) // Non - skeletal proxies
                        {
                           mPLDepthShaderNonSkeletal->ExecuteShader();
                           for (auto& proxy : mNonSkeletalProxiesVec)
                           {
                              if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
                              {
                                 const auto& worldMatrix = proxy->GetMatrix();
                                 const auto& viewMatrices = shadowInfo->GetShadowViewMatrices();
                                 const auto& projectionMatrices = shadowInfo->GetShadowProjectionMatrices();

                                 mPLDepthShaderNonSkeletal->SetTransformationMatrices(worldMatrix, viewMatrices, projectionMatrices);
                                 mPLDepthShaderNonSkeletal->SetFarPlane(pointLightPtr->GetRadianceRadius());
                                 mPLDepthShaderNonSkeletal->SetPointLightPosition(pointLightPtr->GetPosition());

                                 proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                              }
                           }
                           mPLDepthShaderNonSkeletal->StopShader();
                        }
                        if (mSkeletalProxiesVec.size() > 0) // Skeletal proxies
                        {
                           mPLDepthShaderSkeletal->ExecuteShader();
                           for (auto& proxy : mSkeletalProxiesVec)
                           {
                              if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
                              {
                                 SkeletalMeshSceneProxy* skeletalProxy = static_cast<SkeletalMeshSceneProxy*>(proxy);

                                 const auto& worldMatrix = skeletalProxy->GetMatrix();
                                 const auto& viewMatrices = shadowInfo->GetShadowViewMatrices();
                                 const auto& projectionMatrices = shadowInfo->GetShadowProjectionMatrices();

                                 mPLDepthShaderSkeletal->SetTransformationMatrices(worldMatrix, viewMatrices, projectionMatrices);
                                 mPLDepthShaderSkeletal->SetFarPlane(pointLightPtr->GetRadianceRadius());
                                 mPLDepthShaderSkeletal->SetPointLightPosition(pointLightPtr->GetPosition());
                                 mPLDepthShaderSkeletal->SetSkinningMatrices(skeletalProxy->GetSkinningMatrices());

                                 skeletalProxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                              }
                           }
                           mPLDepthShaderSkeletal->StopShader();
                        }
                        glBindFramebuffer(GL_FRAMEBUFFER, 0);

                        // Next frame shadow map will not be updated unless position of objects in the level are changed
                        shadowInfo->SetIsShadowMapDirty(false);
                     }
                  }
               }

               bNewDepthShadowAtlas = false;
            }
         }

         glDisable(GL_CULL_FACE);
      }

      void DeferredShadingSceneRenderer::DeferredBasePass_RenderThread(std::shared_ptr<SceneView> sceneView)
      {
         auto cameraProxy = sceneView->GetCameraProxy();

         glEnable(GL_CULL_FACE);
         glFrontFace(GL_CCW);
         glCullFace(GL_BACK);

         // Deferred shading collect info
         m_gbuffer->BindDeferredGBuffer();

         if (mSkeletalProxiesVec.size() > 0)
         {
            for (auto& proxy : mSkeletalProxiesVec)
            {
               if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
                  proxy->Render(cameraProxy->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
            }
         }

         if (mNonSkeletalProxiesVec.size() > 0)
         {
            for (auto& proxy : mNonSkeletalProxiesVec)
            {
               if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
                  proxy->Render(cameraProxy->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
            }
         }

         m_gbuffer->UnbindDeferredGBuffer();
      }

      void DeferredShadingSceneRenderer::DeferredLightPass_RenderThread(std::shared_ptr<CameraSceneProxy> cameraProxy)
      {
         // TODO: Make some check if light source (point or spot light) is too far from current view position
         m_deferredLightShader->ExecuteShader();

#ifndef NO_LIT
         // ************************** SHADOWS ************************** //
         size_t pointLightIndex = 0, dirLightIndex = 0, spotlightIndex = 0;
         size_t shadowMapSlot = 3, dirShadowMapCount = 0, pointShadowMapCount = 0, spotlightShadowMapCount = 0;
         for (auto& dirLightProxy : mDirLightProxiesVec)
         {
            if (dirLightProxy->IsEnabled())
            {
               ProjectedDirectionalLightShadowInfo* shadowInfo = dirLightProxy->GetProjectedDirShadowInfo();
               if (shadowInfo)
               {
                  shadowInfo->GetAtlasResource()->BindTexture(shadowMapSlot);
                  m_deferredLightShader->SetDirectionalLightShadowMapSlot(dirLightIndex, shadowMapSlot, shadowInfo->GetTextureAtlasOffset());
                  m_deferredLightShader->SetDirectionalLightShadowMatrix(dirLightIndex, shadowInfo->GetShadowMatrix());

                  dirShadowMapCount++;
                  dirLightIndex++;
                  shadowMapSlot++;
               }
            }
         }

         for (auto& pointLightProxy : mPointLightProxiesVec)
         {
            if (pointLightProxy->IsEnabled())
            {
               ProjectedPointLightShadowInfo* shadowInfo = pointLightProxy->GetProjectedPointShadowInfo();
               if (shadowInfo)
               {
                  shadowInfo->GetAtlasResource()->BindTexture(shadowMapSlot);
                  m_deferredLightShader->SetPointLightShadowMapSlot(pointLightIndex, shadowMapSlot);
                  m_deferredLightShader->SetPointLightShadowProjectionFarPlane(pointLightIndex, pointLightProxy->GetRadianceRadius());
                  shadowMapSlot++;
                  pointShadowMapCount++;
                  pointLightIndex++;
               }
            }
         }

         for (const auto& spotLightProxy : mSpotlightProxiesVec)
         {
            if (spotLightProxy->IsEnabled())
            {
               ProjectedSpotlightShadowInfo* shadowInfo = spotLightProxy->GetProjectedSpotLightShadowInfo();
               if (shadowInfo)
               {
                  shadowInfo->GetAtlasResource()->BindTexture(shadowMapSlot);
                  m_deferredLightShader->SetSpotlightShadowMapSlot(spotlightIndex, shadowMapSlot, shadowInfo->GetTextureAtlasOffset());
                  m_deferredLightShader->SetSpotlightShadowProjectionFarPlane(spotlightIndex, spotLightProxy->GetRadianceRadius());
                  m_deferredLightShader->SetSpotlightShadowMatrix(spotlightIndex, shadowInfo->GetShadowMatrix());
                  shadowMapSlot++;
                  spotlightShadowMapCount++;
                  spotlightIndex++;
               }
            }
         }

         m_deferredLightShader->SetCameraWorldPosition(cameraProxy->GetEyeVector());
         m_deferredLightShader->SetDirectionalLightShadowMapCount(dirShadowMapCount);
         m_deferredLightShader->SetPointLightShadowMapCount(pointShadowMapCount);
         m_deferredLightShader->SetSpotlightShadowMapCount(spotlightShadowMapCount);
         // ************************** SHADOWS ************************** //
#endif

         m_gbuffer->BindPositionTexture(0);
         m_gbuffer->BindAlbedoWithSpecularTexture(1);
         m_gbuffer->BindNormalTexture(2);

         m_deferredLightShader->SetGBufferPosition(0);
         m_deferredLightShader->SetGBufferAlbedoNSpecular(1);
         m_deferredLightShader->SetGBufferNormal(2);

#ifndef NO_LIT
         m_deferredLightShader->SetLightsInfo(LightProxiesMap);
#endif
         ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
         m_deferredLightShader->StopShader();

         glDisable(GL_CULL_FACE);
      }

      void DeferredShadingSceneRenderer::ForwardBasePass_RenderThread(std::shared_ptr<SceneView> sceneView)
      {
         glEnable(GL_CULL_FACE);
         glFrontFace(GL_CCW);
         glCullFace(GL_BACK);

         // Resolve depth buffer from gBuffer to default frame buffer
         auto cameraProxy = sceneView->GetCameraProxy();
         auto cameraViewPort = cameraProxy->GetViewPort();

         m_gbuffer->CopyFramebufferData(cameraViewPort.OriginX, cameraViewPort.OriginY, cameraViewPort.Width, cameraViewPort.Height,
            cameraViewPort.OriginX, cameraViewPort.OriginY, cameraViewPort.Width, cameraViewPort.Height, GL_DEPTH_BUFFER_BIT);

         RenderState renderState(std::make_shared<DepthStencilState<true>>(),
            std::make_shared<BlendingState<true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA>>());

         renderState.BindRenderState();

         for (const auto& proxy : mForwardRenderingProxiesVec)
         {
            if (proxy->IsEnabled() && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId()))
               proxy->Render(sceneView->GetCameraProxy()->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
         }

         glDisable(GL_BLEND);
         glDisable(GL_CULL_FACE);
      }

      void DeferredShadingSceneRenderer::PlanarReflectionPass()
      {
         glEnable(GL_CULL_FACE);
         glFrontFace(GL_CCW);
         glCullFace(GL_BACK);

         // todo: set clip distance N equal to index of current planar reflection scene proxy
         // in shader also!

         glEnable(GL_CLIP_DISTANCE0);

         //// todo: REMOVE DYNAMIC ALLOCATION of RENDER STATE!!!!!
         RenderState renderState(std::make_shared<DepthStencilState<true>>(),
            std::make_shared<BlendingState<false>>());
         renderState.BindRenderState();

         for (const auto& planarReflectionProxy : mPlanarReflectionProxiesVec) 
         {
            auto wp =  planarReflectionProxy->GetSceneViewWeakPtr();
            if (auto scenViewSp = wp.lock()) 
            {
               const auto& viewMatrix = scenViewSp->GetCameraProxy()->GetViewMatrix();
               const auto& projectionMatrix = scenViewSp->GetCameraProxy()->GetProjectionMatrix();
               const glm::mat4& mirrorMatrix = planarReflectionProxy->GetMirrorMatrix();
               const glm::vec4& mirrorPlane = planarReflectionProxy->GetReflectionPlane();
               planarReflectionProxy->RenderToPlanarReflectionFBO();

               if (mForwardRenderingProxiesVec.size() > 0)
               {
                  for (auto& proxy : mForwardRenderingProxiesVec)
                  {
                     if (proxy->IsEnabled() && proxy->IsVisible())
                        proxy->RenderPlanarReflection(mirrorPlane, mirrorMatrix, viewMatrix, projectionMatrix);
                  }
               }

               if (mSkeletalProxiesVec.size() > 0)
               {
                  for (auto& proxy : mSkeletalProxiesVec)
                  {
                     if (proxy->IsEnabled() && proxy->IsVisible())
                        proxy->RenderPlanarReflection(mirrorPlane, mirrorMatrix, viewMatrix, projectionMatrix);
                  }
               }

               if (mNonSkeletalProxiesVec.size() > 0)
               {
                  for (auto& proxy : mNonSkeletalProxiesVec)
                  {
                     if (proxy->IsEnabled() && proxy->IsVisible())
                        proxy->RenderPlanarReflection(mirrorPlane, mirrorMatrix, viewMatrix, projectionMatrix);
                  }
               }

               planarReflectionProxy->StopRenderingToPlanarReflectionFBO();
            }
         }


         glDisable(GL_CULL_FACE);
         glDisable(GL_CLIP_DISTANCE0);
      }

      void DeferredShadingSceneRenderer::PrepareSceneProxiesForRender()
      {
         if (bProxiesDirty)
         {
            mForwardRenderingProxiesVec.clear();
            mSkeletalProxiesVec.clear();
            mNonSkeletalProxiesVec.clear();

            for (auto& proxy : SceneProxiesMap)
            {
               PrimitiveSceneProxy* proxyPtr = proxy.second.get();

               if (proxyPtr->IsDeferred())
               {
                  if (proxyPtr->GetPrimitiveProxyType() == PrimitiveProxyType::SKELETAL_MESH_PROXY)
                     mSkeletalProxiesVec.push_back(static_cast<SkeletalMeshSceneProxy*>(proxyPtr));
                  else
                     mNonSkeletalProxiesVec.push_back(proxyPtr);
               }
               else
               {
                  mForwardRenderingProxiesVec.push_back(proxyPtr);
               }
            }
            SetProxiesAreDirty(false);
         }

         if (bLightProxiesDirty)
         {
            mDirLightProxiesVec.clear();
            mPointLightProxiesVec.clear();
            mSpotlightProxiesVec.clear();

            for (auto& proxy : LightProxiesMap)
            {
               LightSceneProxy* proxyPtr = proxy.second.get();

               const LightSceneProxyType& lightType = proxyPtr->GetLightProxyType();

               if (lightType == LightSceneProxyType::DIR_LIGHT)
               {
                  mDirLightProxiesVec.push_back(static_cast<DirectionalLightSceneProxy*>(proxyPtr));
               }
               else if (lightType == LightSceneProxyType::POINT_LIGHT)
               {
                  mPointLightProxiesVec.push_back(static_cast<PointLightSceneProxy*>(proxyPtr));
               }
               else if (lightType == LightSceneProxyType::SPOT_LIGHT)
               {
                  mSpotlightProxiesVec.push_back(static_cast<SpotlightSceneProxy*>(proxyPtr));
               }
            }

            GroupLightsByShadowMap();

            SetLightProxiesAreDirty(false);
         }

         if (bPlanarReflectionProxiesDirty)
         {
            for (auto& proxy : PlanarReflectionProxiesMap)
            {
               PlanarReflectionProxy* proxyPtr = proxy.second.get();
               auto findIt = std::find_if(mPlanarReflectionProxiesVec.begin(), mPlanarReflectionProxiesVec.end(),
                  [=](const auto& existingProxy) { return proxyPtr->GetSceneProxyId() == existingProxy->GetSceneProxyId(); });

               if (mPlanarReflectionProxiesVec.end() == findIt) {
                  mPlanarReflectionProxiesVec.push_back(proxyPtr);
               }
            }

            SetPlanarReflectionProxiesAreDirty(false);
         }
      }

      void DeferredShadingSceneRenderer::GroupLightsByShadowMap()
      {
         mGroupedByShadowAtlasLights.clear();
         for (auto& lProxy : LightProxiesMap)
         {
            if (auto shadowInfo = lProxy.second->GetShadowInfo())
            {
               if (auto atlasResource = shadowInfo->GetAtlasResource())
               {
                  auto lastDesc = atlasResource->GetTextureDescriptor();
                  if (mGroupedByShadowAtlasLights.count(lastDesc) == 0)
                  {
                     std::vector<LightSceneProxy*> result;

                     for (const auto& seekLProxy : LightProxiesMap)
                     {
                        if (seekLProxy.second->GetShadowInfo() && seekLProxy.second->GetShadowInfo()->GetAtlasResource())
                        {
                           if (seekLProxy.second->GetShadowInfo()->GetAtlasResource()->GetTextureDescriptor() == lastDesc)
                              result.emplace_back(seekLProxy.second.get());
                        }
                     }
                     mGroupedByShadowAtlasLights[lastDesc] = result;
                  }
               }
            }
         }
      }

      void DeferredShadingSceneRenderer::RenderScene_RenderThread()
      {
         PrepareSceneProxiesForRender();

         for (const auto& sceneViewPair : SceneViewsMap)
         {
            auto sceneView = sceneViewPair.second;
            auto cameraProxy = sceneView->GetCameraProxy();

            sceneView->DoVisibilityTest();

            // Deferred shading is done with main camera
            if (cameraProxy->GetCameraSceneType() == eCameraSceneProxyType::MAIN_SCENE_CAMERA)
            {
               PlanarReflectionPass();

               DepthPass(sceneView);

               DeferredBasePass_RenderThread(sceneView);

               DeferredLightPass_RenderThread(cameraProxy);

               if (mForwardRenderingProxiesVec.size())
                  ForwardBasePass_RenderThread(sceneView);
            }
            else
            {
               // TODO: rendering to render texture later....
            }

#if DEBUG
            DebugRenderPhysics(sceneView->GetCameraProxy()->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
         }

         DebugFramePanelsPass();


#endif
      }

      void DeferredShadingSceneRenderer::SetProxiesAreDirty(const bool bDirty)
      {
         bProxiesDirty = bDirty;
      }

      void DeferredShadingSceneRenderer::SetLightProxiesAreDirty(const bool bDirty)
      {
         bLightProxiesDirty = bDirty;
      }

      void DeferredShadingSceneRenderer::SetPlanarReflectionProxiesAreDirty(const bool bDirty)
      {
         bPlanarReflectionProxiesDirty = bDirty;
      }

      void DeferredShadingSceneRenderer::PushRenderTargetToTextureRenderer()
      {
         m_textureRenderer.PushDebugRenderTarget();
      }

#if DEBUG

      void DeferredShadingSceneRenderer::DebugFramePanelsPass()
      {
         RenderState renderState(std::make_shared<DepthStencilState<false>>(),
            std::make_shared<BlendingState<false>>());

         renderState.BindRenderState();
         m_textureRenderer.RenderFrames(m_gbuffer);
      }

      void DeferredShadingSceneRenderer::SetDebugPhysicsRenderData(const DebugPhysicsRenderData& debugPhysicsRenderData)
      {
         mDebugPhysicsRenderData = debugPhysicsRenderData;
      }

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
            for (auto& proxy : SceneProxiesMap)
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

}
   }