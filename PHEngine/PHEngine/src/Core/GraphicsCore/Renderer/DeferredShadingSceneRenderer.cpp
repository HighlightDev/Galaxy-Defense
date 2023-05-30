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
#include "Core/GameCore/TextHandler.h"
#include "Core/GraphicsCore/SceneProxy/ParticleSystemSceneProxy.h"
#include "Core/GraphicsCore/Renderer/PrimitiveSorter.h"
#include "Core/GameCore/GUI/Common/TextFieldProxyType.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GameCore/LoggerExtension.h"

#include <gl/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <limits>
#include <algorithm>

using namespace Resources;
using namespace Common;
using namespace Graphics;
using namespace Graphics::Proxy;
using namespace Graphics::OpenGL;
using namespace EngineUtility;
using namespace EngineCore;
using namespace IO;
using namespace EngineCore::GUI;

namespace Graphics
{
   namespace Renderer
   {
      DeferredShadingSceneRenderer::DeferredShadingSceneRenderer(InterThreadCommunicationMgr &interThreadMgr)
          : m_interThreadMgr(interThreadMgr),
            m_gbuffer(
                std::make_unique<DeferredShadingGBuffer>(ViewPortInfo(0, 0,
                                                                      DisplayDeviceDataProvider::GetInstance()->GetWindowWidth(),
                                                                      DisplayDeviceDataProvider::GetInstance()->GetWindowHeight()))),
            m_resolvedSceneFramebuffer(std::make_shared<ResolvedSceneFramebuffer>(ViewPortInfo(0, 0,
                                                                                               DisplayDeviceDataProvider::GetInstance()->GetWindowWidth(),
                                                                                               DisplayDeviceDataProvider::GetInstance()->GetWindowHeight()))),
            m_deferredLightShader(),
            m_fontShader(),
            mDepthCollectShaderSkeletal(),
            mDepthCollectShaderNonSkeletal(),
            mDepthCollectPointLightShaderSkeletal(),
            mDepthCollectPointLightShaderNonSkeletal(),
            bProxiesDirty(false),
            bLightProxiesDirty(false),
            bPlanarReflectionProxiesDirty(false),
#if DEBUG
            mDebugPhysicsRenderData(),
#endif
            mForwardRenderingProxiesVec(),
            mSkeletalProxiesVec(),
            mNonSkeletalProxiesVec(),
            mDirLightProxiesVec(),
            mPointLightProxiesVec(),
            mSpotlightProxiesVec(),
            mPlanarReflectionProxiesVec(),
            mGroupedByShadowAtlasLights(),
            mFontHandler(std::make_shared<FontHandler>()),
            mPostFxRenderer(std::make_unique<PostFxRenderer>(ViewPortInfo(0, 0,
                                                                          DisplayDeviceDataProvider::GetInstance()->GetWindowWidth(),
                                                                          DisplayDeviceDataProvider::GetInstance()->GetWindowHeight()))),
            SceneViewsVector(),
            PrimitiveProxiesVector(),
            LightProxiesVector(),
            MaterialProxiesVector(),
            PlanarReflectionProxiesVector()
      {
         LogInfo("DeferredShadingSceneRenderer::ctor");

         Initialize();
      }

      void DeferredShadingSceneRenderer::Initialize()
      {
         const auto &folderManager = FolderManager::GetInstance();

         const ShaderParams depthCollectShaderParams("DepthCollectShader",
                                                     FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "depthCollectVS.glsl",
                                                     FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "depthCollectFS.glsl");

         const ShaderParams plDepthCollectShaderParams("PointLightDepthCollectShader",
                                                       FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "depthCollectPointLightVS.glsl",
                                                       FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "depthCollectPointLightFS.glsl",
                                                       FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "depthCollectPointLightGS.glsl");

         CompositeShaderParams staticMeshParams(
             "StaticMeshVertexFactory_DepthCollectShader", depthCollectShaderParams);
         CompositeShaderParams skeletalMeshParams(
             "SkeletalMeshVertexFactory<4>_DepthCollectShader", depthCollectShaderParams);

         CompositeShaderParams staticMeshCompositeParams(
             "StaticMeshVertexFactory_PointLightDepthCollectShader", plDepthCollectShaderParams);
         CompositeShaderParams skeletalMeshCompositeParams(
             "SkeletalMeshVertexFactory<4>_PointLightDepthCollectShader", plDepthCollectShaderParams);

         mDepthCollectShaderNonSkeletal = Resources::CompositeShaderPool::GetInstance()->template GetOrAllocateResource<VertexFactoryCompositeShader<StaticMeshVertexFactory, DepthCollectShader>>(staticMeshParams);
         mDepthCollectShaderSkeletal = Resources::CompositeShaderPool::GetInstance()->template GetOrAllocateResource<VertexFactoryCompositeShader<SkeletalMeshVertexFactory<4>, DepthCollectShader>>(skeletalMeshParams);

         mDepthCollectPointLightShaderSkeletal = Resources::CompositeShaderPool::GetInstance()->template GetOrAllocateResource<VertexFactoryCompositeShader<SkeletalMeshVertexFactory<4>, PointLightDepthCollectShader>>(skeletalMeshCompositeParams);
         mDepthCollectPointLightShaderNonSkeletal = Resources::CompositeShaderPool::GetInstance()->template GetOrAllocateResource<VertexFactoryCompositeShader<StaticMeshVertexFactory, PointLightDepthCollectShader>>(staticMeshCompositeParams);

         ShaderParams deferredLightShaderParams(
             "DeferredLight Shader", folderManager->GetShadersPath() + "deferredLightPassVS.glsl", folderManager->GetShadersPath() + "deferredLightPassFS.glsl");
         m_deferredLightShader = ShaderPool::GetInstance()->template GetOrAllocateResource<DeferredLightShader>(deferredLightShaderParams);

         ShaderParams fontRenderingShaderParams(
             "FontRendering Shader", folderManager->GetShadersPath() + "fontVS.glsl", folderManager->GetShadersPath() + "fontFS.glsl");
         m_fontShader = ShaderPool::GetInstance()->template GetOrAllocateResource<FontRenderingShader>(fontRenderingShaderParams);
      }

      DeferredShadingSceneRenderer::~DeferredShadingSceneRenderer()
      {
         LogInfo("DeferredShadingSceneRenderer::dctor");
      }

      void DeferredShadingSceneRenderer::PostLevelInit()
      {
         m_interThreadMgr.ExecuteOnRenderThread(
             eEnqueueJobPolicy::PUSH_ANYWAY,
             0, 0,
             [=]()
             {
                for (const auto &lightProxy : LightProxiesVector)
                {
                   lightProxy->PostLevelInit();
                }
             });

         RegisterFonts();
      }

      InterThreadCommunicationMgr &DeferredShadingSceneRenderer::GetInterThreadCommunicationManager()
      {
         return m_interThreadMgr;
      }

      void DeferredShadingSceneRenderer::RegisterFonts()
      {
         const auto &fonts = EngineConfigHolder::GetInstance()->GetEngineConfig().FontsVector;

         for (const auto &font : fonts)
         {
            FontParams fontParams(font, font + ".fnt", font + ".png");
            mFontHandler->RegisterFont(fontParams);
         }
      }

      void DeferredShadingSceneRenderer::DepthPass(const std::shared_ptr<SceneView> &sceneView)
      {
         if (mGroupedByShadowAtlasLights.size())
         {
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            RenderState<DepthState<true, GL_LEQUAL>, StencilState<false, 0, 0, 0, 0, 0, 0, 0>, BlendingState<false>> renderState;
            renderState.BindRenderState();

            for (auto &atlasLightGroup : mGroupedByShadowAtlasLights)
            {
               bool bNewDepthShadowAtlas = true;

               for (auto &lightProxyPtr : atlasLightGroup.second)
               {
                  auto lightProxyType = lightProxyPtr->GetLightProxyType();

                  if (lightProxyType == LightSceneProxyType::DIR_LIGHT)
                  {
                     const auto dirLightPtr = std::static_pointer_cast<DirectionalLightSceneProxy>(lightProxyPtr);

                     if (dirLightPtr->IsEnabled())
                     {
                        ProjectedShadowInfo *const shadowInfo = dirLightPtr->GetProjectedDirShadowInfo();
                        if (shadowInfo && shadowInfo->IsShadowMapDirty())
                        {
                           shadowInfo->BindShadowFramebuffer(true, bNewDepthShadowAtlas);

                           const BoundingBox3D &dirLightShadowOrthoBound = dirLightPtr->GetShadowOrthographicProjectionBound();

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
                     const auto spotlightPtr = std::static_pointer_cast<SpotlightSceneProxy>(lightProxyPtr);

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
                                    const auto skeletalProxy = std::static_pointer_cast<SkeletalMeshSceneProxy>(proxy);

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
                     const auto pointLightPtr = std::static_pointer_cast<PointLightSceneProxy>(lightProxyPtr);

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
                                    const auto skeletalProxy = std::static_pointer_cast<SkeletalMeshSceneProxy>(proxy);

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

      void DeferredShadingSceneRenderer::DeferredBasePass_RenderThread(const std::shared_ptr<SceneView> &sceneView)
      {
         const auto &cameraProxy = sceneView->GetCameraProxy();

         glEnable(GL_CULL_FACE);
         glFrontFace(GL_CCW);
         glCullFace(GL_BACK);

         RenderState<DepthState<true, GL_LEQUAL>, StencilState<false, 0, 0, 0, 0, 0, 0, 0>, BlendingState<false>> renderState;
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
                  proxy->Render(cameraProxy, cameraProxy->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
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
                  proxy->Render(cameraProxy, cameraProxy->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
               }
            }
         }

         m_resolvedSceneFramebuffer->BindResolvedSceneFramebuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      }

      void DeferredShadingSceneRenderer::DeferredLightPass_RenderThread(const std::shared_ptr<CameraSceneProxy> &cameraProxy)
      {
         RenderState<DepthState<false, GL_LEQUAL>, StencilState<true, GL_KEEP, GL_KEEP, GL_REPLACE, GL_ALWAYS, 0, 0xFF, 0xFF>, BlendingState<false>> renderState;
         renderState.BindRenderState();
         glDepthMask(0x00);
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
         glDepthMask(0xFF);
      }

      void DeferredShadingSceneRenderer::ForwardBasePass_RenderThread(const std::shared_ptr<SceneView> &sceneView)
      {
         glEnable(GL_CULL_FACE);
         glFrontFace(GL_CCW);
         glCullFace(GL_BACK);

         // Resolve depth buffer from gBuffer to default frame buffer
         auto cameraProxy = sceneView->GetCameraProxy();
         auto cameraViewPort = cameraProxy->GetViewPort();

         const auto originX = cameraViewPort.OriginX,
                    originY = cameraViewPort.OriginY,
                    screenWidth = cameraViewPort.Width,
                    screenHeight = cameraViewPort.Height;

         m_gbuffer->CopyFramebufferDataToDstFramebuffer(m_resolvedSceneFramebuffer->GetFramebufferObjectInstance(), originX, originY, screenWidth, screenHeight,
                                                        originX, originY, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT);

         static constexpr int NoClearFlag = 0;
         m_resolvedSceneFramebuffer->BindResolvedSceneFramebuffer(NoClearFlag);

         RenderState<DepthState<true, GL_LEQUAL>, StencilState<true, GL_KEEP, GL_KEEP, GL_REPLACE, GL_ALWAYS, 0, 0xFF, 0xFF>,
                     BlendingState<true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA>>
             renderState;
         renderState.BindRenderState();

         PrimitiveSorter sorter;
         sorter.SortPrimitivesByOrder(mForwardRenderingProxiesVec);

         for (const auto &proxy : mForwardRenderingProxiesVec)
         {
            const bool bShouldRender = proxy->IsTransformIntialized() &&
                                       proxy->IsEnabled() &&
                                       proxy->IsVisible() &&
                                       sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
            if (bShouldRender)
            {
               const int32_t stencilFuncRefValue = proxy->CanBloomBeApplied() ? 0 : 1;
               glStencilFunc(GL_ALWAYS, stencilFuncRefValue, 0xFF); // write 0 to stencil
               proxy->Render(sceneView->GetCameraProxy(), sceneView->GetCameraProxy()->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
            }
         }

         glDisable(GL_BLEND);
         glDisable(GL_CULL_FACE);
         glDisable(GL_STENCIL_TEST);

         m_resolvedSceneFramebuffer->UnbindFramebuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      }

      void DeferredShadingSceneRenderer::PlanarReflectionPass()
      {
         if (mPlanarReflectionProxiesVec.size() <= 0)
            return;

         glEnable(GL_CULL_FACE);
         glFrontFace(GL_CW);
         glCullFace(GL_BACK);
         glEnable(GL_CLIP_DISTANCE0);

         RenderState<DepthState<true, GL_LEQUAL>, StencilState<false, 0, 0, 0, 0, 0, 0, 0>,
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

      void DeferredShadingSceneRenderer::HudTextPass()
      {
         const auto &renderDataMap = mFontHandler->GetFontBatcher();

         RenderState<DepthState<false, GL_LEQUAL>, StencilState<false, 0, 0, 0, 0, 0, 0, 0>, BlendingState<true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA>> renderState;
         renderState.BindRenderState();

         for (const auto &renderData : renderDataMap)
         {
            const auto &renderDataSp = renderData.second;
            renderDataSp->GetFontTextureAtlas()->BindTexture(0);
            m_fontShader->ExecuteShader();
            const auto &textFields = renderDataSp->GetTexFieldProxies();
            m_fontShader->SetFontAtlasSlot(0);
            for (const auto &textField : textFields)
            {
               if (textField->GetIsVisible() &&
                   eTextFieldProxyType::HUD_TEXT_FIELD == textField->GetTextFieldProxyType())
               {
                  m_fontShader->SetPosition(textField->GetPosition());
                  m_fontShader->SetColor(textField->GetColor());
                  renderDataSp->GetTextMesh()->GetBuffer()->RenderVAO(textField->GetVertexStart(), textField->GetVerticesCount(), GL_TRIANGLES);
               }
            }
            m_fontShader->StopShader();
         }

         glDisable(GL_BLEND);
      }

      void DeferredShadingSceneRenderer::GuiPass(const std::shared_ptr<SceneView> &sceneView)
      {
         RenderState<DepthState<false, GL_LEQUAL>, StencilState<false, 0, 0, 0, 0, 0, 0, 0>, BlendingState<true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA>> renderState;
         renderState.BindRenderState();
         glDepthMask(false);
         for (const auto &canvas : mUiCanvasProxies)
         {
            const auto &canvasOrigin = canvas->GetAbsoluteOrigin();
            const auto &widthHeight = canvas->GetWidthHeight();
            glViewport(canvasOrigin.x, canvasOrigin.y, widthHeight.x, widthHeight.y);
            canvas->Render();
         }

         glDepthMask(true);
         const auto &viewPortInfo = sceneView->GetCameraProxy()->GetViewPort();
         glViewport(viewPortInfo.OriginX, viewPortInfo.OriginY, viewPortInfo.Width, viewPortInfo.Height);
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
               if (proxy->IsDeferred())
               {
                  if (proxy->GetPrimitiveProxyType() == ePrimitiveProxyType::SKELETAL_MESH_PROXY)
                     mSkeletalProxiesVec.emplace_back(std::static_pointer_cast<SkeletalMeshSceneProxy>(proxy));
                  else
                     mNonSkeletalProxiesVec.emplace_back(proxy);
               }
               else
               {
                  mForwardRenderingProxiesVec.emplace_back(proxy);
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
                  mDirLightProxiesVec.emplace_back(std::static_pointer_cast<DirectionalLightSceneProxy>(proxy));
               }
               else if (lightType == LightSceneProxyType::POINT_LIGHT)
               {
                  mPointLightProxiesVec.emplace_back(std::static_pointer_cast<PointLightSceneProxy>(proxy));
               }
               else if (lightType == LightSceneProxyType::SPOT_LIGHT)
               {
                  mSpotlightProxiesVec.emplace_back(std::static_pointer_cast<SpotlightSceneProxy>(proxy));
               }
            }

            GroupLightsByShadowMap();

            SetLightProxiesAreDirty(false);
         }

         if (bPlanarReflectionProxiesDirty)
         {
            for (const auto &proxy : PlanarReflectionProxiesVector)
            {
               auto findIt = std::find_if(mPlanarReflectionProxiesVec.begin(), mPlanarReflectionProxiesVec.end(),
                                          [&](const auto &existingProxy)
                                          { return proxy->GetSceneProxyId() == existingProxy->GetSceneProxyId(); });

               if (mPlanarReflectionProxiesVec.end() == findIt)
               {
                  mPlanarReflectionProxiesVec.emplace_back(proxy);
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
                     std::vector<std::shared_ptr<LightSceneProxy>> result;

                     for (const auto &proxy : LightProxiesVector)
                     {
                        if (proxy->GetShadowInfo() && proxy->GetShadowInfo()->GetAtlasResource())
                        {
                           if (proxy->GetShadowInfo()->GetAtlasResource()->GetTextureDescriptor() == lastDesc)
                              result.emplace_back(proxy);
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
            const auto &cameraProxy = sceneView->GetCameraProxy();
            if (cameraProxy->IsInitializedFirstTime())
            {
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

                  if (mPostFxRenderer)
                     mPostFxRenderer->Execute(m_resolvedSceneFramebuffer);

                  HudTextPass();

                  GuiPass(sceneView);
                  // TODO: rendering to render texture later....
               }
               else
               {
                  // TODO: rendering to render texture later....
               }

#if DEBUG
               // DebugRenderPhysics(sceneView->GetCameraProxy()->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
#endif
            }
         }
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

      std::shared_ptr<UiSceneProxyBase> DeferredShadingSceneRenderer::GetUiSceneProxyByProxyId(const size_t proxyId, const size_t canvasId) const
      {
         auto canvasIt = std::find_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [=](const auto &canvasProxy)
                                      { return canvasId == canvasProxy->GetUiItemUId(); });
         return canvasIt != mUiCanvasProxies.end() ? (*canvasIt)->GetSceneProxyById(proxyId) : nullptr;
      }

      std::shared_ptr<UiCanvasSceneProxy> DeferredShadingSceneRenderer::GetCanvasSceneProxyByProxyId(const size_t proxyId) const
      {
         auto canvasIt = std::find_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [=](const auto &canvasProxy)
                                      { return proxyId == canvasProxy->GetUiItemUId(); });
         return canvasIt != mUiCanvasProxies.end() ? *canvasIt : nullptr;
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

      void DeferredShadingSceneRenderer::MaterialProxyAdded_OnRenderThread(const std::shared_ptr<MaterialProxy> &materialProxy)
      {
         static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::MaterialProxyAdded_OnRenderThread");

         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, materialProxy->GetSceneProxyId(), functionId, [this, materialProxy]() { 
            assert(!GetMaterialProxyByProxyId(materialProxy->GetSceneProxyId()));
            LogInfo("DeferredShadingSceneRenderer::MaterialProxyAdded_OnRenderThread => material name = ", materialProxy->MaterialName, "proxyId = ", materialProxy->GetSceneProxyId());
            MaterialProxiesVector.emplace_back(materialProxy); 
         });
      }

      bool DeferredShadingSceneRenderer::UpdatePrimitiveComponentEnable_OnRenderThread(const size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const bool bEnabled)
      {
         bool updateSuccess = false;
         const auto &primitiveSp = GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
         if (primitiveSp)
         {
            updateSuccess = true;
            m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, creatorObjectId, functionId, [primitiveSp, bEnabled]() {
               primitiveSp->SetEnabled(bEnabled);
            });
         }
         else
         {
            LogInfo("DeferredShadingSceneRenderer::UpdatePrimitiveComponentEnable_OnRenderThread => "
                    "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                    primitiveSceneProxyIndex);
         }

         return updateSuccess;
      }

      bool DeferredShadingSceneRenderer::UpdatePrimitiveComponentVisibility_OnRenderThread(const size_t primitiveSceneProxyIndex,
                                                                                           const uint64_t creatorObjectId,
                                                                                           const uint64_t functionId,
                                                                                           const bool visibility)
      {
         bool updateSuccess = false;
         const auto &primitiveSp = GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
         if (primitiveSp)
         {
            updateSuccess = true;
            m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, creatorObjectId, functionId, [primitiveSp, visibility]() {
               primitiveSp->SetVisibility(visibility); 
            });
         }
         else
         {
            LogInfo("DeferredShadingSceneRenderer::UpdatePrimitiveComponentVisibility_OnRenderThread => "
                    "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                    primitiveSceneProxyIndex);
         }

         return updateSuccess;
      }

      bool DeferredShadingSceneRenderer::UpdatePrimitiveComponentSortOrderValue_OnRenderThread(const size_t primitiveSceneProxyIndex,
                                                                                               const uint64_t creatorObjectId,
                                                                                               const uint64_t functionId,
                                                                                               const int32_t sortOrderValue)
      {
         bool updateSuccess = false;
         const auto &primitiveSp = GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
         if (primitiveSp)
         {
            updateSuccess = true;
            m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, creatorObjectId, functionId, [primitiveSp, sortOrderValue]() {
               primitiveSp->SetSortOrderValue(sortOrderValue); 
            });
         }
         else
         {
            LogInfo("DeferredShadingSceneRenderer::UpdatePrimitiveComponentSortOrderValue_OnRenderThread => "
                    "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                    primitiveSceneProxyIndex);
         }

         return updateSuccess;
      }

      bool DeferredShadingSceneRenderer::UpdatePrimitiveComponentTransform_OnRenderThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId,
                                                                                          const uint64_t functionId, const glm::mat4 &newRelativeMatrix, const BoundingBox3D &newTransformedBoundingBox)
      {
         auto updateSuccess = false;
         const auto &primitiveProxySp = GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
         if (primitiveProxySp)
         {
            updateSuccess = true;
            m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, creatorObjectId, functionId, [primitiveProxySp, newRelativeMatrix, newTransformedBoundingBox]() {
               primitiveProxySp->SetTransformationMatrix(newRelativeMatrix);
               primitiveProxySp->SetTransformedBoundingBox(newTransformedBoundingBox); 
            });
         }
         else
         {
            LogInfo("DeferredShadingSceneRenderer::UpdatePrimitiveComponentTransform_OnRenderThread => "
                     "Error !Current proxy index doesn't exist on RT. Proxy index = ",
                     primitiveSceneProxyIndex);
         }
         return updateSuccess;
      }

      bool DeferredShadingSceneRenderer::UpdateLightComponentTransform_OnRenderThread(const size_t lightSceneProxyIndex,
                                                                                      const uint64_t creatorObjectId,
                                                                                      const uint64_t functionId,
                                                                                      const glm::mat4 &newRelativeMatrix)
      {
         auto updateSuccess = false;
         const auto &lightProxySp = GetLightProxyByProxyId(lightSceneProxyIndex);
         if (lightProxySp)
         {
            updateSuccess = true;
            m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, creatorObjectId, functionId, [newRelativeMatrix, lightProxySp]() {
               lightProxySp->SetTransformationMatrix(newRelativeMatrix);
            });
         }
         else
         {
            LogInfo("DeferredShadingSceneRenderer::UpdateLightComponentTransform_OnRenderThread => "
                     "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                     lightSceneProxyIndex);
         }

         return updateSuccess;
      }

      void DeferredShadingSceneRenderer::PrimitiveSceneProxyDeleted_OnRenderThread(const size_t primitiveSceneProxyIndex)
      {
         static constexpr uint64_t creatorObjectId = 0;
         static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::PrimitiveSceneProxyDeleted_OnRenderThread");

         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, creatorObjectId, functionId, [this, primitiveSceneProxyIndex]() {
            if (RemovePrimitiveProxyByProxyId(primitiveSceneProxyIndex))
            {
               SetProxiesAreDirty(true);
            }
            else 
            {
               LogInfo("DeferredShadingSceneRenderer::PrimitiveSceneProxyDeleted_OnRenderThread => "
                        "Error! Current proxy index doesn't exist on RT. Proxy index = ", primitiveSceneProxyIndex);
            }
         });
      }

      void DeferredShadingSceneRenderer::PrimitiveSceneProxiesUpdated_OnRenderThread()
      {
         static constexpr uint64_t creatorObjectId = 0;
         static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::PrimitiveSceneProxiesUpdated_OnRenderThread");

         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, creatorObjectId, functionId, [this]() {
            SetProxiesAreDirty(true); 
         });
      }

      void DeferredShadingSceneRenderer::LightSceneProxyDeleted_OnRenderThread(const size_t lightSceneProxyIndex)
      {
         static constexpr uint64_t creatorObjectId = 0;
         static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::LightSceneProxyDeleted_OnRenderThread");

         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, creatorObjectId, functionId, [this, lightSceneProxyIndex]() {
            if (RemoveLightProxyByProxyId(lightSceneProxyIndex))
            {
               SetLightProxiesAreDirty(true); 
            }
            else
            {
               LogInfo("DeferredShadingSceneRenderer::LightSceneProxyDeleted_OnRenderThread => "
                     "Error! Current proxy index doesn't exist on RT. Proxy index = ", lightSceneProxyIndex);
            } 
         });
      }

      void DeferredShadingSceneRenderer::LightSceneProxiesUpdated_OnRenderThread()
      {
         static constexpr uint64_t creatorObjectId = 0;
         static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::LightSceneProxiesUpdated_OnRenderThread");

         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, creatorObjectId, functionId, [this]() {
            SetLightProxiesAreDirty(true); 
         });
      }

      void DeferredShadingSceneRenderer::CameraSceneProxyAdded_OnRenderThread(const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy)
      {
         static constexpr uint64_t creatorObjectId = 0;
         static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::CameraSceneProxyAdded_OnRenderThread");

         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, creatorObjectId, functionId, [this, cameraSceneProxy]() {
            SceneViewsVector.emplace_back(std::make_shared<SceneView>(cameraSceneProxy, PrimitiveProxiesVector)); 
         });
      }

      void DeferredShadingSceneRenderer::PrimitiveSceneProxyAdded_OnRenderThread(const std::shared_ptr<PrimitiveSceneProxy>& primitiveSceneProxy)
      {
         static constexpr uint64_t creatorObjectId = 0;
         static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::PrimitiveSceneProxyAdded_OnRenderThread");

         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, creatorObjectId, functionId, [this, primitiveSceneProxy]() {
            assert(!GetPrimitiveProxyByProxyId(primitiveSceneProxy->GetSceneProxyId()));
            primitiveSceneProxy->PostConstructorInitialize();
            PrimitiveProxiesVector.emplace_back(primitiveSceneProxy);
            SetProxiesAreDirty(true);
         });
      }

      void DeferredShadingSceneRenderer::LightSceneProxyAdded_OnRenderThread(const std::shared_ptr<LightSceneProxy>& lightSceneProxy)
      {
         static constexpr uint64_t creatorObjectId = 0;
         static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::LightSceneProxyAdded_OnRenderThread");

         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, creatorObjectId, functionId, [this, lightSceneProxy]() {
            assert(!GetLightProxyByProxyId(lightSceneProxy->GetSceneProxyId()));
            LightProxiesVector.emplace_back(lightSceneProxy);
            SetLightProxiesAreDirty(true);
         });
      }

      void DeferredShadingSceneRenderer::RegisterText_OnRenderThread(const std::shared_ptr<HudTextField> &textField, const bool subscribeOnTextScreenSpaceSizeUpdate)
      {
         LogInfo("DeferredShadingSceneRenderer::RegisterText_OnRenderThread => font name = ", textField->GetFontName(), " textFieldId = ", textField->GetTextFieldId());

         static constexpr uint64_t creatorObjectId = 0;
         static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::RegisterText_OnRenderThread");

         const auto textFieldProxy = TextFieldProxy::CreateTextFieldProxyInstance(
             textField->GetTextFieldId(),
             eTextFieldProxyType::HUD_TEXT_FIELD,
             textField->GetIsVisible(),
             textField->GetText(),
             textField->GetFontName(),
             textField->GetPosition(),
             textField->GetColor(),
             textField->GetFontSize(),
             textField->GetTextHorizontalAlignment(),
             textField->GetLineMaxSize(),
             textField->GetNumberOfLines(),
             subscribeOnTextScreenSpaceSizeUpdate);
         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, creatorObjectId, functionId, [this, textFieldProxy]() {
            RegisterText(textFieldProxy); 
         });
      }

      void DeferredShadingSceneRenderer::UnregisterText_OnRenderThread(const std::shared_ptr<HudTextField> &textField)
      {
         LogInfo("DeferredShadingSceneRenderer::UnregisterText_OnRenderThread => font name = ", textField->GetFontName(), " textFieldId = ", textField->GetTextFieldId());

         static constexpr uint64_t creatorObjectId = 0;
         static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::UnregisterText_OnRenderThread");

         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, creatorObjectId, functionId, [this, fontName = textField->GetFontName(), textFieldId = textField->GetTextFieldId()]() {
            UnregisterText(fontName, textFieldId); 
         });
      }

      void DeferredShadingSceneRenderer::RegisterUiCanvasProxy_OnRenderThread(const std::shared_ptr<UiCanvasSceneProxy> &uiCanvasProxy)
      {
         LogInfo("DeferredShadingSceneRenderer::RegisterUiCanvasProxy_OnRenderThread => UId = ", uiCanvasProxy->GetUiItemUId());

         static constexpr uint64_t creatorObjectId = 0;
         static constexpr uint64_t functionId = Hash64_CT("DeferredShadingSceneRenderer::RegisterUiCanvasProxy_OnRenderThread");

         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, creatorObjectId, functionId, [this, uiCanvasProxy]() {
            RegisterUiCanvasProxy(uiCanvasProxy); 
         });
      }

      void DeferredShadingSceneRenderer::UnregisterUiCanvasProxy_OnRenderThread(const std::shared_ptr<UiCanvasSceneProxy> &uiCanvasProxy)
      {
         LogInfo("DeferredShadingSceneRenderer::UnregisterUiCanvasProxy_OnRenderThread => UId = ", uiCanvasProxy->GetUiItemUId());

         static constexpr uint64_t creatorObjectId = 0;
         static constexpr uint64_t functionId = Hash64_CT("DeferredShadingSceneRenderer::UnregisterUiCanvasProxy_OnRenderThread");

         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, creatorObjectId, functionId, [=]() {
            UnregisterUiCanvasProxy(uiCanvasProxy); 
         });
      }

      void DeferredShadingSceneRenderer::RegisterUiSceneProxy_OnRenderThread(const std::shared_ptr<UiItemBase>& uiItem, const std::shared_ptr<UiSceneProxyBase>& uiSceneProxy, const size_t canvasUId)
      {
         LogInfo("DeferredShadingSceneRenderer::RegisterUiSceneProxy_OnRenderThread => UId = ", uiSceneProxy->GetUiItemUId(), " canvasUId = ", canvasUId);

         static constexpr uint64_t creatorObjectId = 0;
         static constexpr uint64_t functionId = Hash64_CT("DeferredShadingSceneRenderer::RegisterUiSceneProxy_OnRenderThread");

         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, creatorObjectId, functionId, [=]() {
            RegisterUiSceneProxy(uiSceneProxy, canvasUId); 
            uiItem->SetIsSceneProxyReady(true);
         });
      }

      void DeferredShadingSceneRenderer::UnregisterUiSceneProxy_OnRenderThread(const std::shared_ptr<UiItemBase>& uiItem, const std::shared_ptr<UiSceneProxyBase>& uiSceneProxy, const size_t canvasUId)
      {
         LogInfo("DeferredShadingSceneRenderer::UnregisterUiSceneProxy_OnRenderThread => UId = ", uiSceneProxy->GetUiItemUId(), " canvasUId = ", canvasUId);

         static constexpr uint64_t creatorObjectId = 0;
         static constexpr uint64_t functionId = Hash64_CT("DeferredShadingSceneRenderer::UnregisterUiSceneProxy_OnRenderThread");

         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, creatorObjectId, functionId, [=]() {
            UnregisterUiSceneProxy(uiSceneProxy, canvasUId);
            uiItem->SetIsSceneProxyReady(false);
         });
      }

      void DeferredShadingSceneRenderer::TextDataChanged_OnRenderThread(const std::shared_ptr<HudTextField> &textField, const eTextChangedDataType textChangedDataType)
      {
         static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::TextDataChanged_OnRenderThread");

         const auto textFontName = textField->GetFontName();
         const auto textFieldId = textField->GetTextFieldId();
         if (eTextChangedDataType::OFFSET == textChangedDataType)
         {
            m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, textFieldId, functionId, [this, textFontName, textFieldId, textPosition = textField->GetPosition()]() {
               TextPositionChanged(textFontName,textFieldId, textPosition);
            });
         }
         else if (eTextChangedDataType::COLOR == textChangedDataType)
         {
            m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, textFieldId, functionId, [this, textFontName, textFieldId, textColor = textField->GetColor()]() {
               TextColorChanged(textFontName, textFieldId, textColor);
            });
         }
         else if (eTextChangedDataType::TEXT == textChangedDataType)
         {
            m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, textFieldId, functionId, [this, textFontName, textFieldId, text = textField->GetText()]() {
               TextChanged(textFontName, textFieldId, text);
            });
         }
         else if (eTextChangedDataType::VISIBILITY == textChangedDataType)
         {
             m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, textFieldId, functionId, [this, textFontName, textFieldId, isVisible = textField->GetIsVisible()]() {
               TextVisibilityChanged(textFontName, textFieldId, isVisible);
            });
         }
      }

      void DeferredShadingSceneRenderer::MaterialPropertiesUpdated_OnRenderThread(const size_t materialProxyIndex, std::vector<std::shared_ptr<MaterialProperty>> &&properties)
      {
         static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::MaterialPropertiesUpdated_OnRenderThread");
         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, 0, functionId, [this, materialProxyIndex, properties = std::move(properties)]() mutable { 
            const auto& materialProxySp = GetMaterialProxyByProxyId(materialProxyIndex);
            if (materialProxySp)
            {
               materialProxySp->UpdateProperties(std::move(properties)); 
            }
            else
            {
               LogInfo("DeferredShadingSceneRenderer::MaterialPropertiesUpdated_OnRenderThread => "
                  "Error! Current proxy index doesn't exist on RT. Proxy index = ", materialProxyIndex);
            } 
         });
      }

      void DeferredShadingSceneRenderer::PlanarReflectionSceneProxyAdded_OnRenderThread(const std::shared_ptr<PlanarReflectionProxy> &proxy)
      {
            static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::PlanarReflectionSceneProxyAdded");
            m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, proxy->GetSceneProxyId(), functionId, [proxy, this]() {
               const auto& reflectionProxySp = GetPlanarReflectionProxyByProxyId(proxy->GetSceneProxyId());
               assert(!reflectionProxySp);
               PlanarReflectionProxiesVector.emplace_back(proxy);
               SetPlanarReflectionProxiesAreDirty(true); 
            });
      }

      void DeferredShadingSceneRenderer::BindPlanarReflectionSceneProxyToSceneView_OnRenderThread(const std::shared_ptr<PlanarReflectionProxy> &planarReflectionProxy,
                                                                                                  const size_t cameraSceneProxyId)
      {
         static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::BindPlanarReflectionSceneProxyToSceneView_OnRenderThread");
         m_interThreadMgr.ExecuteOnRenderThread(eEnqueueJobPolicy::PUSH_ANYWAY, cameraSceneProxyId, functionId, [this, cameraSceneProxyId, planarReflectionProxy]() {
            const auto &sceneViewSp = GetSceneViewByProxyId(cameraSceneProxyId);
            if (sceneViewSp)
            {
               planarReflectionProxy->SetSceneViewWeakPtr(sceneViewSp);
            }
            else
            {
               LogInfo("DeferredShadingSceneRenderer::BindPlanarReflectionSceneProxyToSceneView_OnRenderThread => Error! Current proxy index doesn't exist on RT. Proxy index = ", cameraSceneProxyId);
            }
         });
      }

      void DeferredShadingSceneRenderer::RegisterText(const std::shared_ptr<TextFieldProxy> &textFieldProxy)
      {
         mFontHandler->RegisterText(textFieldProxy);
      }

      void DeferredShadingSceneRenderer::UnregisterText(const std::string &fontName, const int32_t textFieldProxyId)
      {
         mFontHandler->UnregisterText(fontName, textFieldProxyId);
      }

      void DeferredShadingSceneRenderer::TextPositionChanged(const std::string &fontName, const int32_t textFieldProxyId, const glm::vec2 &position)
      {
         mFontHandler->TextPositionChanged(fontName, textFieldProxyId, position);
      }

      void DeferredShadingSceneRenderer::TextColorChanged(const std::string &fontName, const int32_t textFieldProxyId, const glm::vec3 &color)
      {
         mFontHandler->TextColorChanged(fontName, textFieldProxyId, color);
      }

      void DeferredShadingSceneRenderer::TextChanged(const std::string &fontName, const int32_t textFieldProxyId, const std::string &text)
      {
         mFontHandler->TextChanged(fontName, textFieldProxyId, text);

         if (mFontHandler->IsTextSubscribedOnSizeChangeUpdate(fontName, textFieldProxyId))
         {
            static constexpr uint64_t creatorObjectId = 0;
            static const uint64_t functionId = Hash("DeferredShadingSceneRenderer::TextChanged");

            if (const auto &sceneSp = m_interThreadMgr.GetSceneWP().lock())
            {
               m_interThreadMgr.ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                                                    creatorObjectId, functionId, [=]()
                                                    { sceneSp->GetTextHandler()
                                                          .GetTextFieldById(textFieldProxyId)
                                                          ->SetTextScreenSpaceSize(mFontHandler
                                                                                       ->GetTextScreenSpaceSize(fontName, textFieldProxyId)); });
            }
         }
      }

      void DeferredShadingSceneRenderer::TextVisibilityChanged(const std::string &fontName, const int32_t textFieldProxyId, const bool bIsVisible)
      {
         mFontHandler->TextVisibilityChanged(fontName, textFieldProxyId, bIsVisible);
      }

      void DeferredShadingSceneRenderer::RegisterUiCanvasProxy(const std::shared_ptr<UiCanvasSceneProxy> &canvasSceneProxy)
      {
         assert(canvasSceneProxy);
         auto canvasIt = std::find_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [&](const auto &canvasProxy)
                                      { return canvasSceneProxy->GetUiItemUId() == canvasProxy->GetUiItemUId(); });
         assert(canvasIt == mUiCanvasProxies.end());
         mUiCanvasProxies.emplace_back(canvasSceneProxy);
         canvasSceneProxy->SetFontHandler(mFontHandler);
      }

      void DeferredShadingSceneRenderer::UnregisterUiCanvasProxy(const std::shared_ptr<UiCanvasSceneProxy> &canvasSceneProxy)
      {
         assert(canvasSceneProxy);
         mUiCanvasProxies.erase(std::remove_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [&](const auto &canvasProxy)
                                               { return canvasSceneProxy->GetUiItemUId() == canvasProxy->GetUiItemUId(); }));
      }

      void DeferredShadingSceneRenderer::RegisterUiSceneProxy(const std::shared_ptr<UiSceneProxyBase> &sceneProxy, const size_t canvasUId)
      {
         assert(sceneProxy);
         auto canvasIt = std::find_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [=](const auto &canvasProxy)
                                      { return canvasUId == canvasProxy->GetUiItemUId(); });
         assert(canvasIt != mUiCanvasProxies.end());
         sceneProxy->SetCanvasSceneProxy((*canvasIt));
         (*canvasIt)->AddUiSceneProxy(sceneProxy);
         sceneProxy->OnSceneProxyRegistered();
      }

      void DeferredShadingSceneRenderer::UnregisterUiSceneProxy(const std::shared_ptr<UiSceneProxyBase> &sceneProxy, const size_t canvasUId)
      {
         assert(sceneProxy);
         auto canvasIt = std::find_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [=](const auto &canvasProxy)
                                      { return canvasUId == canvasProxy->GetUiItemUId(); });
         assert(canvasIt != mUiCanvasProxies.end());
         (*canvasIt)->RemoveUiSceneProxy(sceneProxy);
      }

#if DEBUG

      void DeferredShadingSceneRenderer::SetDebugPhysicsRenderData(const DebugPhysicsRenderData &debugPhysicsRenderData)
      {
         mDebugPhysicsRenderData = debugPhysicsRenderData;
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