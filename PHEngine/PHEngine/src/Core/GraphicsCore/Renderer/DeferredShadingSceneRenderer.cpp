#include "DeferredShadingSceneRenderer.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Scene.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
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
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/GameCore/Scene.h"

#include "Core/GameCore/GUI/Text/GUIText.h"
#include "Core/GameCore/GUI/Text/FontType.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

#include <gl/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <limits>
#include <algorithm>
#include <TinyLogger/LogInterface.h>

using namespace Resources;
using namespace Common;
using namespace Graphics;
using namespace Graphics::Renderer;
using namespace Graphics::Proxy;
using namespace Graphics::OpenGL;
using namespace EngineUtility;
using namespace EngineCore;
using namespace IO;
using namespace TinyLogger;

namespace Graphics
{
   namespace Renderer
   {

      DeferredShadingSceneRenderer::DeferredShadingSceneRenderer(InterThreadCommunicationMgr &interThreadMgr)
          : SceneViewsVector(),
            bLightProxiesDirty(false),
            bProxiesDirty(false),
            bPlanarReflectionProxiesDirty(false),
            PrimitiveProxiesVector(),
            LightProxiesVector(),
            m_interThreadMgr(interThreadMgr),
            m_gbuffer(
                std::make_unique<DeferredShadingGBuffer>(ViewPortInfo(0, 0,
                                                                      DisplayDeviceDataProvider::GetInstance()->GetWindowWidth(),
                                                                      DisplayDeviceDataProvider::GetInstance()->GetWindowHeight())))
      {
         Logger::Out("DeferredShadingSceneRenderer::ctor");

         const auto &folderManager = FolderManager::GetInstance();

         const ShaderParams depthCollectShaderParams("DepthCollectShader",
                                                     FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "depthCollectVS.glsl",
                                                     FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "depthCollectFS.glsl");

         const ShaderParams plDepthCollectShaderParams("PointLightDepthCollectShader",
                                                       FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "depthCollectPointLightVS.glsl",
                                                       FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "depthCollectPointLightFS.glsl",
                                                       FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "depthCollectPointLightGS.glsl");

         TemplatedCompositeShaderParams staticMeshParams(
             "StaticMeshVertexFactory_DepthCollectShader", depthCollectShaderParams);
         TemplatedCompositeShaderParams skeletalMeshParams(
             "SkeletalMeshVertexFactory<4>_DepthCollectShader", depthCollectShaderParams);

         TemplatedCompositeShaderParams staticMeshCompositeParams(
             "StaticMeshVertexFactory_PointLightDepthCollectShader", plDepthCollectShaderParams);
         TemplatedCompositeShaderParams skeletalMeshCompositeParams(
             "SkeletalMeshVertexFactory<4>_PointLightDepthCollectShader", plDepthCollectShaderParams);

         mDepthCollectShaderNonSkeletal = Resources::CompositeShaderPool::GetInstance()->template GetOrAllocateResource<VertexFactoryCompositeShader<StaticMeshVertexFactory, DepthCollectShader>>(staticMeshParams);
         mDepthCollectShaderSkeletal = Resources::CompositeShaderPool::GetInstance()->template GetOrAllocateResource<VertexFactoryCompositeShader<SkeletalMeshVertexFactory<4>, DepthCollectShader>>(skeletalMeshParams);

         mDepthCollectPointLightShaderSkeletal = Resources::CompositeShaderPool::GetInstance()->template GetOrAllocateResource<VertexFactoryCompositeShader<SkeletalMeshVertexFactory<4>, PointLightDepthCollectShader>>(skeletalMeshCompositeParams);
         mDepthCollectPointLightShaderNonSkeletal = Resources::CompositeShaderPool::GetInstance()->template GetOrAllocateResource<VertexFactoryCompositeShader<StaticMeshVertexFactory, PointLightDepthCollectShader>>(staticMeshCompositeParams);

         ShaderParams deferredLightShaderParams(
             "DeferredLight Shader", folderManager->GetShadersPath() + "deferredLightPassVS.glsl", folderManager->GetShadersPath() + "deferredLightPassFS.glsl");
         m_deferredLightShader = ShaderPool::GetInstance()->template GetOrAllocateResource<DeferredLightShader>(deferredLightShaderParams);
      }

      DeferredShadingSceneRenderer::~DeferredShadingSceneRenderer()
      {
      }

      void DeferredShadingSceneRenderer::PostLevelInit()
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(0, 0, [=]()
                                                     {
            for (auto& lightProxy : LightProxiesVector)
            {
               lightProxy->PostLevelInit();
            } }));

         RegisterFonts();
      }

      void DeferredShadingSceneRenderer::RegisterFonts()
      {
         const auto &fonts = EngineConfigHolder::GetInstance()->GetEngineConfig().FontsVector;

         for (const auto &font : fonts)
         {
            FontParams fontParams(font, font + ".fnt", font + ".png");
            mFontHandler.RegisterFont(fontParams);
         }
      }

      void DeferredShadingSceneRenderer::DepthPass(std::shared_ptr<SceneView> sceneView)
      {
         if (mGroupedByShadowAtlasLights.size())
         {
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            RenderState<DepthStencilState<true, GL_LEQUAL, false, 0, 0, 0>, BlendingState<false>> renderState;
            renderState.BindRenderState();

            for (auto &atlasLightGroup : mGroupedByShadowAtlasLights)
            {
               bool bNewDepthShadowAtlas = true;

               for (auto &lightProxyPtr : atlasLightGroup.second)
               {
                  auto lightProxyType = lightProxyPtr->GetLightProxyType();

                  if (lightProxyType == LightSceneProxyType::DIR_LIGHT)
                  {
                     DirectionalLightSceneProxy *lightPtr = static_cast<DirectionalLightSceneProxy *>(lightProxyPtr);

                     if (lightPtr->IsEnabled())
                     {
                        ProjectedShadowInfo *const shadowInfo = lightPtr->GetProjectedDirShadowInfo();
                        if (shadowInfo && shadowInfo->IsShadowMapDirty())
                        {
                           shadowInfo->BindShadowFramebuffer(true, bNewDepthShadowAtlas);

                           DirectionalLightSceneProxy *dirLightPtr = lightPtr;
                           const BoundingBox &dirLightShadowOrthoBound = dirLightPtr->GetShadowOrthographicProjectionBound();

                           if (mNonSkeletalProxiesVec.size() > 0) // Non - skeletal proxies
                           {
                              mDepthCollectShaderNonSkeletal->ExecuteShader();
                              mDepthCollectShaderNonSkeletal->GetShader()->SetWriteDepthLinearly(false);
                              for (auto &proxy : mNonSkeletalProxiesVec)
                              {
                                 if (proxy->IsEnabled() && proxy->IsVisible() && dirLightShadowOrthoBound.IsIntersectionWithBox(proxy->GetTransformedBoundingBox()))
                                 {
                                    const auto &worldMatrix = proxy->GetMatrix();
                                    const auto &viewMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowViewMatrix();
                                    const auto &projectionMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowProjectionMatrix();
                                    mDepthCollectShaderNonSkeletal->GetVertexFactoryShader()->SetMatrices(worldMatrix, viewMatrix, projectionMatrix);

                                    proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                                 }
                              }
                              mDepthCollectShaderNonSkeletal->StopShader();
                           }

                           if (mSkeletalProxiesVec.size() > 0) // Skeletal proxies
                           {
                              mDepthCollectShaderSkeletal->ExecuteShader();
                              for (auto &proxy : mSkeletalProxiesVec)
                              {
                                 const bool bShouldRender = proxy->IsEnabled() &&
                                                            proxy->IsVisible() &&
                                                            dirLightShadowOrthoBound.IsIntersectionWithBox(proxy->GetTransformedBoundingBox());
                                 if (bShouldRender)
                                 {
                                    const auto &worldMatrix = proxy->GetMatrix();
                                    const auto &viewMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowViewMatrix();
                                    const auto &projectionMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowProjectionMatrix();
                                    mDepthCollectShaderSkeletal->GetVertexFactoryShader()->SetMatrices(worldMatrix, viewMatrix, projectionMatrix);
                                    mDepthCollectShaderSkeletal->GetVertexFactoryShader()->SetSkinningMatrices(proxy->GetSkinningMatrices());

                                    proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                                 }
                              }
                              mDepthCollectShaderSkeletal->StopShader();
                           }

                           // Next frame shadow map will not be updated unless position of objects on scene were changed
                           shadowInfo->SetIsShadowMapDirty(false);
                        }
                     }
                  }
                  else if (lightProxyType == LightSceneProxyType::SPOT_LIGHT)
                  {
                     SpotlightSceneProxy *spotlightPtr = static_cast<SpotlightSceneProxy *>(lightProxyPtr);

                     if (spotlightPtr->IsEnabled())
                     {
                        const auto &shadowInfo = spotlightPtr->GetProjectedSpotLightShadowInfo();
                        if (shadowInfo && shadowInfo->IsShadowMapDirty())
                        {
                           shadowInfo->BindShadowFramebuffer(true, bNewDepthShadowAtlas);

                           if (mNonSkeletalProxiesVec.size() > 0) // Non - skeletal proxies
                           {
                              mDepthCollectShaderNonSkeletal->ExecuteShader();
                              mDepthCollectShaderNonSkeletal->GetShader()->SetWriteDepthLinearly(true);
                              for (auto &proxy : mNonSkeletalProxiesVec)
                              {
                                 const bool bShouldRender = proxy->IsTransformIntialized() &&
                                                            proxy->IsEnabled() &&
                                                            proxy->IsVisible() &&
                                                            sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
                                 if (bShouldRender)
                                 {
                                    const auto &worldMatrix = proxy->GetMatrix();
                                    const auto &viewMatrix = shadowInfo->GetShadowViewMatrix();
                                    const auto &projectionMatrix = shadowInfo->GetShadowProjectionMatrix();

                                    mDepthCollectShaderNonSkeletal->GetVertexFactoryShader()->SetMatrices(worldMatrix, viewMatrix, projectionMatrix);
                                    mDepthCollectShaderNonSkeletal->GetShader()->SetShadowDistance(spotlightPtr->GetRadianceRadius());
                                    mDepthCollectShaderNonSkeletal->GetShader()->SetLightWorldPosition(spotlightPtr->GetPosition());

                                    proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                                 }
                              }
                              mDepthCollectShaderNonSkeletal->StopShader();
                           }
                           if (mSkeletalProxiesVec.size() > 0) // Skeletal proxies
                           {
                              mDepthCollectShaderSkeletal->ExecuteShader();
                              mDepthCollectShaderSkeletal->GetShader()->SetWriteDepthLinearly(true);

                              for (auto &proxy : mSkeletalProxiesVec)
                              {
                                 const bool bShouldRender = proxy->IsTransformIntialized() &&
                                                            proxy->IsEnabled() &&
                                                            proxy->IsVisible() &&
                                                            sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());

                                 if (bShouldRender)
                                 {
                                    SkeletalMeshSceneProxy *skeletalProxy = static_cast<SkeletalMeshSceneProxy *>(proxy);

                                    const auto &worldMatrix = skeletalProxy->GetMatrix();
                                    const auto &viewMatrices = shadowInfo->GetShadowViewMatrix();
                                    const auto &projectionMatrices = shadowInfo->GetShadowProjectionMatrix();

                                    mDepthCollectShaderSkeletal->GetVertexFactoryShader()->SetMatrices(worldMatrix, viewMatrices, projectionMatrices);
                                    mDepthCollectShaderSkeletal->GetVertexFactoryShader()->SetSkinningMatrices(skeletalProxy->GetSkinningMatrices());
                                    mDepthCollectShaderSkeletal->GetShader()->SetShadowDistance(spotlightPtr->GetRadianceRadius());
                                    mDepthCollectShaderSkeletal->GetShader()->SetLightWorldPosition(spotlightPtr->GetPosition());

                                    skeletalProxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                                 }
                              }
                              mDepthCollectShaderSkeletal->StopShader();
                           }
                           glBindFramebuffer(GL_FRAMEBUFFER, 0);

                           // Next frame shadow map will not be updated unless position of objects on scene are changed
                           shadowInfo->SetIsShadowMapDirty(false);
                        }
                     }
                  }
                  else if (lightProxyType == LightSceneProxyType::POINT_LIGHT)
                  {
                     PointLightSceneProxy *pointLightPtr = static_cast<PointLightSceneProxy *>(lightProxyPtr);

                     if (pointLightPtr->IsEnabled())
                     {
                        const auto &shadowInfo = pointLightPtr->GetProjectedPointShadowInfo();
                        if (shadowInfo && shadowInfo->IsShadowMapDirty())
                        {
                           shadowInfo->BindShadowFramebuffer(true, true); // every point light has it's own depth texture atlas

                           if (mNonSkeletalProxiesVec.size() > 0) // Non - skeletal proxies
                           {
                              mDepthCollectPointLightShaderNonSkeletal->ExecuteShader();
                              for (auto &proxy : mNonSkeletalProxiesVec)
                              {
                                 const bool bShouldRender = proxy->IsTransformIntialized() &&
                                                            proxy->IsEnabled() &&
                                                            proxy->IsVisible() &&
                                                            sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
                                 if (bShouldRender)
                                 {
                                    const auto &worldMatrix = proxy->GetMatrix();
                                    const auto &viewMatrices = shadowInfo->GetShadowViewMatrices();
                                    const auto &projectionMatrices = shadowInfo->GetShadowProjectionMatrices();

                                    mDepthCollectPointLightShaderNonSkeletal->GetShader()->SetTransformationMatrices(viewMatrices, projectionMatrices);
                                    mDepthCollectPointLightShaderNonSkeletal->GetVertexFactoryShader()->SetMatrices(worldMatrix, glm::mat4(), glm::mat4());
                                    mDepthCollectPointLightShaderNonSkeletal->GetShader()->SetFarPlane(pointLightPtr->GetRadianceRadius());
                                    mDepthCollectPointLightShaderNonSkeletal->GetShader()->SetPointLightPosition(pointLightPtr->GetPosition());

                                    proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                                 }
                              }
                              mDepthCollectPointLightShaderNonSkeletal->StopShader();
                           }
                           if (mSkeletalProxiesVec.size() > 0) // Skeletal proxies
                           {
                              mDepthCollectPointLightShaderSkeletal->ExecuteShader();
                              for (auto &proxy : mSkeletalProxiesVec)
                              {
                                 const bool bShouldRender = proxy->IsTransformIntialized() &&
                                                            proxy->IsEnabled() &&
                                                            proxy->IsVisible() &&
                                                            sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
                                 if (bShouldRender)
                                 {
                                    SkeletalMeshSceneProxy *skeletalProxy = static_cast<SkeletalMeshSceneProxy *>(proxy);

                                    const auto &worldMatrix = skeletalProxy->GetMatrix();
                                    const auto &viewMatrices = shadowInfo->GetShadowViewMatrices();
                                    const auto &projectionMatrices = shadowInfo->GetShadowProjectionMatrices();

                                    mDepthCollectPointLightShaderSkeletal->GetVertexFactoryShader()->SetMatrices(worldMatrix, glm::mat4(), glm::mat4());
                                    mDepthCollectPointLightShaderSkeletal->GetVertexFactoryShader()->SetSkinningMatrices(skeletalProxy->GetSkinningMatrices());
                                    mDepthCollectPointLightShaderSkeletal->GetShader()->SetTransformationMatrices(viewMatrices, projectionMatrices);
                                    mDepthCollectPointLightShaderSkeletal->GetShader()->SetFarPlane(pointLightPtr->GetRadianceRadius());
                                    mDepthCollectPointLightShaderSkeletal->GetShader()->SetPointLightPosition(pointLightPtr->GetPosition());

                                    skeletalProxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                                 }
                              }
                              mDepthCollectPointLightShaderSkeletal->StopShader();
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
         }

         glDisable(GL_CULL_FACE);
      }

      void DeferredShadingSceneRenderer::DeferredBasePass_RenderThread(std::shared_ptr<SceneView> sceneView)
      {
         auto cameraProxy = sceneView->GetCameraProxy();

         glEnable(GL_CULL_FACE);
         glFrontFace(GL_CCW);
         glCullFace(GL_BACK);

         RenderState<DepthStencilState<true, GL_LEQUAL, false, 0, 0, 0>, BlendingState<false>> renderState;
         renderState.BindRenderState();

         // Deferred shading collect info
         m_gbuffer->BindDeferredGBuffer();

         if (mSkeletalProxiesVec.size() > 0)
         {
            for (auto &proxy : mSkeletalProxiesVec)
            {
               const bool bShouldRender = proxy->IsTransformIntialized() &&
                                          proxy->IsEnabled() &&
                                          proxy->IsVisible() &&
                                          sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
               if (bShouldRender)
               {
                  proxy->Render(cameraProxy->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
               }
            }
         }

         if (mNonSkeletalProxiesVec.size() > 0)
         {
            for (auto &proxy : mNonSkeletalProxiesVec)
            {
               const bool bShouldRender = proxy->IsTransformIntialized() &&
                                          proxy->IsEnabled() &&
                                          proxy->IsVisible() &&
                                          sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
               if (bShouldRender)
               {
                  proxy->Render(cameraProxy->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
               }
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
         size_t shadowMapSlot = 4, dirShadowMapCount = 0, pointShadowMapCount = 0, spotlightShadowMapCount = 0;
         for (auto &dirLightProxy : mDirLightProxiesVec)
         {
            if (dirLightProxy->IsEnabled())
            {
               ProjectedDirectionalLightShadowInfo *shadowInfo = dirLightProxy->GetProjectedDirShadowInfo();
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

         for (auto &pointLightProxy : mPointLightProxiesVec)
         {
            if (pointLightProxy->IsEnabled())
            {
               ProjectedPointLightShadowInfo *shadowInfo = pointLightProxy->GetProjectedPointShadowInfo();
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

         for (const auto &spotLightProxy : mSpotlightProxiesVec)
         {
            if (spotLightProxy->IsEnabled())
            {
               ProjectedSpotlightShadowInfo *shadowInfo = spotLightProxy->GetProjectedSpotLightShadowInfo();
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
         m_gbuffer->BindAlbedoTexture(1);
         m_gbuffer->BindNormalTexture(2);
         m_gbuffer->BindMetallicRoughnessTexture(3);

         m_deferredLightShader->SetGBufferPosition(0);
         m_deferredLightShader->SetGBufferAlbedo(1);
         m_deferredLightShader->SetGBufferNormal(2);

#ifdef SHADING_MODEL_PBR
         m_deferredLightShader->SetGBufferMetallicRoughness(3);
#endif

#ifndef NO_LIT
         m_deferredLightShader->SetLightsInfo(LightProxiesVector);
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

         RenderState<DepthStencilState<true, GL_LEQUAL, false, 0, 0, 0>,
                     BlendingState<true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA>>
             renderState;
         renderState.BindRenderState();

         for (const auto &proxy : mForwardRenderingProxiesVec)
         {
            const bool bShouldRender = proxy->IsTransformIntialized() &&
                                       proxy->IsEnabled() &&
                                       proxy->IsVisible() &&
                                       sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
            if (bShouldRender)
            {
               proxy->Render(sceneView->GetCameraProxy()->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
            }
         }

         glDisable(GL_BLEND);
         glDisable(GL_CULL_FACE);
      }

      void DeferredShadingSceneRenderer::PlanarReflectionPass()
      {
         if (mPlanarReflectionProxiesVec.size() <= 0)
            return;

         glEnable(GL_CULL_FACE);
         glFrontFace(GL_CW);
         glCullFace(GL_BACK);
         glEnable(GL_CLIP_DISTANCE0);

         RenderState<DepthStencilState<true, GL_LEQUAL, false, 0, 0, 0>,
                     BlendingState<false>>
             renderState;

         renderState.BindRenderState();

         for (const auto &planarReflectionProxy : mPlanarReflectionProxiesVec)
         {
            auto wp = planarReflectionProxy->GetSceneViewWeakPtr();
            if (auto scenViewSp = wp.lock())
            {
               const auto &viewMatrix = scenViewSp->GetCameraProxy()->GetViewMatrix();
               const auto &projectionMatrix = scenViewSp->GetCameraProxy()->GetProjectionMatrix();
               const glm::mat4 &mirrorMatrix = planarReflectionProxy->GetMirrorMatrix();
               const glm::vec4 &mirrorPlane = planarReflectionProxy->GetReflectionPlane();

               const CameraFrustum &mirroredCameraFrustum =
                   CameraFrustum::GetConstructedFromViewProjectionMatrices(viewMatrix * mirrorMatrix, projectionMatrix);

               planarReflectionProxy->RenderToPlanarReflectionFBO();

               if (mSkeletalProxiesVec.size() > 0)
               {
                  for (auto &proxy : mSkeletalProxiesVec)
                  {
                     if (proxy->IsTransformIntialized() &&
                         proxy->IsEnabled() &&
                         proxy->IsVisible())
                     {
                        bool bDraw = proxy->IsFrustumCullTestNeeded() ? mirroredCameraFrustum.CollidesWithBoundingBox(proxy->GetTransformedBoundingBox()) : true;
                        if (bDraw)
                           proxy->RenderPlanarReflection(mirrorPlane, mirrorMatrix, viewMatrix, projectionMatrix);
                     }
                  }
               }

               if (mNonSkeletalProxiesVec.size() > 0)
               {
                  for (auto &proxy : mNonSkeletalProxiesVec)
                  {
                     if (proxy->IsTransformIntialized() &&
                         proxy->IsEnabled() &&
                         proxy->IsVisible())
                     {
                        bool bDraw = proxy->IsFrustumCullTestNeeded() ? mirroredCameraFrustum.CollidesWithBoundingBox(proxy->GetTransformedBoundingBox()) : true;
                        if (bDraw)
                           proxy->RenderPlanarReflection(mirrorPlane, mirrorMatrix, viewMatrix, projectionMatrix);
                     }
                  }
               }

               if (mForwardRenderingProxiesVec.size() > 0)
               {
                  for (auto &proxy : mForwardRenderingProxiesVec)
                  {
                     if (proxy->IsTransformIntialized() &&
                         proxy->IsEnabled() &&
                         proxy->IsVisible())
                     {
                        bool bDraw = proxy->IsFrustumCullTestNeeded() ? mirroredCameraFrustum.CollidesWithBoundingBox(proxy->GetTransformedBoundingBox()) : true;
                        if (bDraw)
                           proxy->RenderPlanarReflection(mirrorPlane, mirrorMatrix, viewMatrix, projectionMatrix);
                     }
                  }
               }

               planarReflectionProxy->StopRenderingToPlanarReflectionFBO();
               planarReflectionProxy->ResolveReflectionRenderTargetSurfaceData();
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

            for (auto &proxy : PrimitiveProxiesVector)
            {
               PrimitiveSceneProxy *proxyPtr = proxy.get();

               if (proxyPtr->IsDeferred())
               {
                  if (proxyPtr->GetPrimitiveProxyType() == ePrimitiveProxyType::SKELETAL_MESH_PROXY)
                     mSkeletalProxiesVec.push_back(static_cast<SkeletalMeshSceneProxy *>(proxyPtr));
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

            for (auto &proxy : LightProxiesVector)
            {
               const LightSceneProxyType &lightType = proxy->GetLightProxyType();

               if (lightType == LightSceneProxyType::DIR_LIGHT)
               {
                  mDirLightProxiesVec.push_back(static_cast<DirectionalLightSceneProxy *>(proxy.get()));
               }
               else if (lightType == LightSceneProxyType::POINT_LIGHT)
               {
                  mPointLightProxiesVec.push_back(static_cast<PointLightSceneProxy *>(proxy.get()));
               }
               else if (lightType == LightSceneProxyType::SPOT_LIGHT)
               {
                  mSpotlightProxiesVec.push_back(static_cast<SpotlightSceneProxy *>(proxy.get()));
               }
            }

            GroupLightsByShadowMap();

            SetLightProxiesAreDirty(false);
         }

         if (bPlanarReflectionProxiesDirty)
         {
            for (auto &proxy : PlanarReflectionProxiesVector)
            {
               PlanarReflectionProxy *proxyPtr = proxy.get();
               auto findIt = std::find_if(mPlanarReflectionProxiesVec.begin(), mPlanarReflectionProxiesVec.end(),
                                          [=](const auto &existingProxy)
                                          { return proxyPtr->GetSceneProxyId() == existingProxy->GetSceneProxyId(); });

               if (mPlanarReflectionProxiesVec.end() == findIt)
               {
                  mPlanarReflectionProxiesVec.push_back(proxyPtr);
               }
            }

            SetPlanarReflectionProxiesAreDirty(false);
         }
      }

      void DeferredShadingSceneRenderer::GroupLightsByShadowMap()
      {
         mGroupedByShadowAtlasLights.clear();
         for (auto &proxy : LightProxiesVector)
         {
            if (auto shadowInfo = proxy->GetShadowInfo())
            {
               if (auto atlasResource = shadowInfo->GetAtlasResource())
               {
                  auto lastDesc = atlasResource->GetTextureDescriptor();

                  const auto groupIt = std::find_if(mGroupedByShadowAtlasLights.begin(), mGroupedByShadowAtlasLights.end(), [=](const auto &lightGroupPair)
                                                    { return lightGroupPair.first == lastDesc; });

                  if (groupIt == mGroupedByShadowAtlasLights.end())
                  {
                     std::vector<LightSceneProxy *> result;

                     for (const auto &proxy : LightProxiesVector)
                     {
                        if (proxy->GetShadowInfo() && proxy->GetShadowInfo()->GetAtlasResource())
                        {
                           if (proxy->GetShadowInfo()->GetAtlasResource()->GetTextureDescriptor() == lastDesc)
                              result.emplace_back(proxy.get());
                        }
                     }
                     mGroupedByShadowAtlasLights.emplace_back(std::make_pair(lastDesc, result));
                  }
               }
            }
         }
      }

      void DeferredShadingSceneRenderer::RenderScene_RenderThread()
      {
         PrepareSceneProxiesForRender();

         for (const auto &sceneView : SceneViewsVector)
         {
            auto cameraProxy = sceneView->GetCameraProxy();

            sceneView->DoVisibilityTest();

            // Deferred shading is done with main camera
            if (eCameraSceneProxyType::MAIN_SCENE_CAMERA == cameraProxy->GetCameraSceneType())
            {
               PlanarReflectionPass();

               DepthPass(sceneView);

               DeferredBasePass_RenderThread(sceneView);

               DeferredLightPass_RenderThread(cameraProxy);

               if (mForwardRenderingProxiesVec.size())
                  ForwardBasePass_RenderThread(sceneView);

               DebugRenderText();
            }
            else
            {
               // TODO: rendering to render texture later....
            }

#if DEBUG
            DebugRenderPhysics(sceneView->GetCameraProxy()->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
#endif
         }

#if DEBUG
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

      std::shared_ptr<SceneView> DeferredShadingSceneRenderer::GetSceneViewByProxyId(const size_t proxyId) const
      {
         std::shared_ptr<SceneView> result = nullptr;

         const auto foundSceneIt = std::find_if(SceneViewsVector.begin(), SceneViewsVector.end(), [=](const auto &sceneView)
                                                { return proxyId == sceneView->GetCameraProxy()->GetSceneProxyId(); });

         if (foundSceneIt != SceneViewsVector.end())
         {
            result = *foundSceneIt;
         }

         return result;
      }

      std::shared_ptr<PrimitiveSceneProxy> DeferredShadingSceneRenderer::GetPrimitiveProxyByProxyId(const size_t proxyId) const
      {
         std::shared_ptr<PrimitiveSceneProxy> result = nullptr;

         const auto foundPrimitiveProxyIt = std::find_if(PrimitiveProxiesVector.begin(), PrimitiveProxiesVector.end(), [=](const auto &primitiveProxy)
                                                         { return proxyId == primitiveProxy->GetSceneProxyId(); });

         if (foundPrimitiveProxyIt != PrimitiveProxiesVector.end())
         {
            result = *foundPrimitiveProxyIt;
         }

         return result;
      }

      std::shared_ptr<LightSceneProxy> DeferredShadingSceneRenderer::GetLightProxyByProxyId(const size_t proxyId) const
      {
         std::shared_ptr<LightSceneProxy> result = nullptr;

         const auto foundLightProxyIt = std::find_if(LightProxiesVector.begin(), LightProxiesVector.end(), [=](const auto &lightProxy)
                                                     { return proxyId == lightProxy->GetSceneProxyId(); });

         if (foundLightProxyIt != LightProxiesVector.end())
         {
            result = *foundLightProxyIt;
         }

         return result;
      }

      std::shared_ptr<MaterialProxy> DeferredShadingSceneRenderer::GetMaterialProxyByProxyId(const size_t proxyId) const
      {
         std::shared_ptr<MaterialProxy> result = nullptr;

         const auto foundMaterialProxyIt = std::find_if(MaterialProxiesVector.begin(), MaterialProxiesVector.end(), [=](const auto &materialProxy)
                                                        { return proxyId == materialProxy->GetSceneProxyId(); });

         if (foundMaterialProxyIt != MaterialProxiesVector.end())
         {
            result = *foundMaterialProxyIt;
         }

         return result;
      }

      std::shared_ptr<PlanarReflectionProxy> DeferredShadingSceneRenderer::GetPlanarReflectionProxyByProxyId(const size_t proxyId) const
      {
         std::shared_ptr<PlanarReflectionProxy> result = nullptr;

         const auto foundPlanarProxyIt = std::find_if(PlanarReflectionProxiesVector.begin(), PlanarReflectionProxiesVector.end(), [=](const auto &planarProxy)
                                                      { return proxyId == planarProxy->GetSceneProxyId(); });

         if (foundPlanarProxyIt != PlanarReflectionProxiesVector.end())
         {
            result = *foundPlanarProxyIt;
         }

         return result;
      }

      bool DeferredShadingSceneRenderer::RemovePrimitiveProxyByProxyId(const size_t proxyId)
      {
         auto removeIt = std::remove_if(PrimitiveProxiesVector.begin(), PrimitiveProxiesVector.end(), [=](const auto &primitiveProxy)
                                        { return proxyId == primitiveProxy->GetSceneProxyId(); });

         if (removeIt != PrimitiveProxiesVector.end())
         {
            PrimitiveProxiesVector.erase(removeIt);
            return true;
         }

         return false;
      }

      bool DeferredShadingSceneRenderer::RemoveLightProxyByProxyId(const size_t proxyId)
      {
         auto removeIt = std::remove_if(LightProxiesVector.begin(), LightProxiesVector.end(), [=](const auto &lightProxy)
                                        { return proxyId == lightProxy->GetSceneProxyId(); });

         if (removeIt != LightProxiesVector.end())
         {
            LightProxiesVector.erase(removeIt);
            return true;
         }

         return false;
      }

      void DeferredShadingSceneRenderer::RegisterText(const std::shared_ptr<TextFieldProxy> &textFieldProxy)
      {
         mFontHandler.RegisterText(textFieldProxy);
      }

      void DeferredShadingSceneRenderer::UnregisterText(const std::string &fontName, const size_t textFieldProxyId)
      {
         mFontHandler.UnregisterText(fontName, textFieldProxyId);
      }

      void DeferredShadingSceneRenderer::TextPositionChanged(const std::string &fontName, const size_t textFieldProxyId, const glm::vec2 &position)
      {
         mFontHandler.TextPositionChanged(fontName, textFieldProxyId, position);
      }

      void DeferredShadingSceneRenderer::TextColorChanged(const std::string &fontName, const size_t textFieldProxyId, const glm::vec3 &color)
      {
         mFontHandler.TextColorChanged(fontName, textFieldProxyId, color);
      }

      void DeferredShadingSceneRenderer::TextChanged(const std::string &fontName, const size_t textFieldProxyId, const std::string &text)
      {
         mFontHandler.TextChanged(fontName, textFieldProxyId, text);
      }

      void DeferredShadingSceneRenderer::TextVisibilityChanged(const std::string &fontName, const size_t textFieldProxyId, const bool bIsVisible)
      {
         mFontHandler.TextVisibilityChanged(fontName, textFieldProxyId, bIsVisible);
      }

#if DEBUG

      void DeferredShadingSceneRenderer::PushRenderTargetToTextureRenderer()
      {
         m_textureRenderer.PushDebugRenderTarget();
      }

      void DeferredShadingSceneRenderer::DebugFramePanelsPass()
      {
         RenderState<DepthStencilState<false, 0, false, 0, 0, 0>, BlendingState<false>> renderState;
         renderState.BindRenderState();

         m_textureRenderer.RenderFrames(m_gbuffer);
      }

      void DeferredShadingSceneRenderer::SetDebugPhysicsRenderData(const DebugPhysicsRenderData &debugPhysicsRenderData)
      {
         mDebugPhysicsRenderData = debugPhysicsRenderData;
      }

      void DeferredShadingSceneRenderer::DebugRenderText()
      {
         static struct TextShader : public Shader
         {
            Uniform u_fontAtlas;
            Uniform u_position;
            Uniform u_color;

            TextShader()
                : Shader(ShaderParams("fontShader",
                                      FolderManager::GetInstance()->GetShadersPath() + "fontVS.glsl",
                                      FolderManager::GetInstance()->GetShadersPath() + "fontFS.glsl"))
            {
               ShaderInit();
            }

            virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override
            {
               u_fontAtlas = GetUniform("fontAtlas", shaderProgramID);
               u_position = GetUniform("position", shaderProgramID);
               u_color = GetUniform("color", shaderProgramID);
            }

            virtual void SetShaderPredefine() override
            {
            }

         } shader;

         const auto &renderDataMap = mFontHandler.GetFontRenderDataMap();

         RenderState<DepthStencilState<false, 0, false, 0, 0, 0>,
                     BlendingState<true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA>>
             renderState;
         renderState.BindRenderState();

         for (const auto &renderData : renderDataMap)
         {
            const auto &renderDataSp = renderData.second;
            renderDataSp->GetFontTextureAtlas()->BindTexture(0);
            shader.ExecuteShader();
            const auto &textFields = renderDataSp->GetTexFieldProxies();
            shader.u_fontAtlas.LoadUniform(0);
            for (const auto &textField : textFields)
            {
               if (textField->mIsVisible)
               {
                  shader.u_position.LoadUniform(textField->mPosition);
                  shader.u_color.LoadUniform(textField->mColor);
                  renderDataSp->GetTextMesh()->GetBuffer()->RenderVAO(textField->mVertexStart, textField->mVerticesCount, GL_TRIANGLES);
               }
            }
            shader.StopShader();
         }

         glDisable(GL_BLEND);
      }

      void DeferredShadingSceneRenderer::DebugRenderPhysics(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         // todo: delete this crap and use buffers =\

         const auto &physicsRenderData = mDebugPhysicsRenderData.GetDebugLines();
         if (physicsRenderData.size())
         {
            float viewMatVec[16]{0.0f};
            const float *pSource = (const float *)glm::value_ptr(viewMatrix);
            for (int i = 0; i < 16; ++i)
               viewMatVec[i] = pSource[i];

            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(pSource);

            float projMatrix[16]{0.0f};
            pSource = (const float *)glm::value_ptr(projectionMatrix);
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

            for (auto& dirLProxy : this->mDirLightProxiesVec)
            {
               const auto bb = dirLProxy->GetShadowOrthographicProjectionBound();
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

            for (auto& proxy : PrimitiveProxiesVector)
            {
               if (proxy->GetPrimitiveProxyType() == ePrimitiveProxyType::SKELETAL_MESH_PROXY || proxy->GetPrimitiveProxyType() == ePrimitiveProxyType::STATIC_MESH_PROXY)
               {
                  const auto bb = proxy->GetTransformedBoundingBox();
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