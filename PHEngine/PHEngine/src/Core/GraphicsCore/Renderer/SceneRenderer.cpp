#include "SceneRenderer.h"

#include "Core/CommonCore/EngineConstants.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/LightComponent.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontAtlas.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontParams.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeTextFieldProxy.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GraphicsCore/Renderer/PrimitiveSorter.h"
#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SkyboxSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/StaticMeshSceneProxy.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/UtilityCore/EngineMath.h"
#include "RenderPassProxies.h"

#include <gl/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <limits>
#include <utility>

using namespace Resources;
using namespace Common;
using namespace Graphics;
using namespace Graphics::Proxy;
using namespace Graphics::OpenGL;
using namespace EngineUtility;
using namespace EngineCore;
using namespace EngineCore::DataProviders;
using namespace IO;
using namespace EngineCore::GUI;

namespace Graphics::Renderer {
SceneRenderer::SceneRenderer(InterThreadCommunicationMgr& interThreadMgr)
    : bDeferredPrimitivesDirty(false)
    , bForwardPrimitivesDirty(false)
    , bLightProxiesDirty(false)
    , bLightProxiesTransformDirty(false)
    , bPlanarReflectionProxiesDirty(false)
    , bDeferredPrimitivesMoved(false)
    , bForwardPrimitivesMoved(false)
    , mFramesSinceDistanceSortRefresh(0)
    , m_interThreadMgr(interThreadMgr)
    , m_gbuffer(
          std::make_unique<DeferredShadingGBuffer>(ViewPortInfo(
              0,
              0,
              GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth(),
              GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight())))
    , m_resolvedSceneFramebuffer(
          std::make_shared<ResolvedSceneFramebuffer>(ViewPortInfo(
              0,
              0,
              GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth(),
              GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight())))
    , m_resolvedSceneAndUiFramebuffer(
          std::make_shared<ResolvedSceneFramebuffer>(ViewPortInfo(
              0,
              0,
              GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth(),
              GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight())))
    , m_deferredLightShader()
    , mDepthCollectShaderSkeletal()
    , mDepthCollectShaderNonSkeletal()
    , mDepthCollectPointLightShaderSkeletal()
    , mDepthCollectPointLightShaderNonSkeletal()
    , mActiveBindedState()
    , mPostFxRenderer(
          std::make_shared<PostFxRenderer>(ViewPortInfo(
              0,
              0,
              GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth(),
              GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight())))
    ,
#if DEBUG
    mDebugPhysicsRenderData()
    ,
#endif
    SceneViewsVector()
    , PrimitiveProxiesVector()
    , LightProxiesVector()
    , MaterialProxiesVector()
    , PlanarReflectionProxiesVector()
    , mUiCanvasProxies()
    , mFreeTypeFontHandler(std::make_shared<FreeTypeFontHandler>())
    , mDirLightProxiesVec()
    , mPointLightProxiesVec()
    , mSpotlightProxiesVec()
    , mGroupedByShadowAtlasLights()
    , mInstancedGeometryBatchRenderer(std::make_shared<InstancedGeometryBatchRenderer>())
{
    LogInfo("SceneRenderer::ctor");

    mProxiesProviders[eRenderPassType::SHADOW_DEPTH_PASS] = std::make_shared<ShadowDepthPassProxiesProvider>();
    mProxiesProviders[eRenderPassType::PLANAR_REFLECTION_PASS] = std::make_shared<PlanarReflectionPassProxiesProvider>();
    mProxiesProviders[eRenderPassType::OUTLINE_PASS] = std::make_shared<OutlinePassProxiesProvider>();
    mProxiesProviders[eRenderPassType::DEPTH_PRE_PASS] = std::make_shared<DepthPrePassProxiesProvider>();
    mProxiesProviders[eRenderPassType::DEFERRED_BASE_PASS] = std::make_shared<DeferredBasePassProxiesProvider>();
    mProxiesProviders[eRenderPassType::FORWARD_BASE_PASS] = std::make_shared<ForwardPassProxiesProvider>();
}

void SceneRenderer::Initialize()
{
    LogInfo("SceneRenderer::Initialize");
    InitializeCoreShaders();
#ifdef DEBUG
    bRenderDebugPhysicsData = EngineConfigHolder::GetInstance()->GetEngineConfig().RenderDebugPhysicsData;
#endif
}

void SceneRenderer::InitializeCoreShaders()
{
    const auto folderManager = FolderManager::GetInstance();

    ShaderParams depthCollectShaderParams("DepthCollectShader");
    depthCollectShaderParams.SetMainShaders(
        folderManager->GetAbsolutePath("depthCollectVS.glsl"), folderManager->GetAbsolutePath("depthCollectFS.glsl"));
    ShaderParams plDepthCollectShaderParams("PointLightDepthCollectShader");
    plDepthCollectShaderParams.SetMainShaders(
        folderManager->GetAbsolutePath("depthCollectPointLightVS.glsl"),
        folderManager->GetAbsolutePath("depthCollectPointLightFS.glsl"));
    plDepthCollectShaderParams.SetGeometryShader(folderManager->GetAbsolutePath("depthCollectPointLightGS.glsl"));

    const CompositeShaderParams staticMeshParams("StaticMeshVertexFactory", depthCollectShaderParams);
    const CompositeShaderParams skeletalMeshParams("SkeletalMeshVertexFactory<4>", depthCollectShaderParams);
    const CompositeShaderParams staticMeshCompositeParams("StaticMeshVertexFactory", plDepthCollectShaderParams);
    const CompositeShaderParams skeletalMeshCompositeParams("SkeletalMeshVertexFactory<4>", plDepthCollectShaderParams);
    ShaderParams deferredLightShaderParams("DeferredLight Shader");
    deferredLightShaderParams.SetMainShaders(
        folderManager->GetAbsolutePath("deferredLightPassVS.glsl"), folderManager->GetAbsolutePath("deferredLightPassFS.glsl"));

    mDepthCollectShaderNonSkeletal
        = std::make_shared<VertexFactoryCompositeShader<StaticMeshVertexFactory, DepthCollectShader>>(staticMeshParams);
    mDepthCollectShaderSkeletal
        = std::make_shared<VertexFactoryCompositeShader<SkeletalMeshVertexFactory<4>, DepthCollectShader>>(skeletalMeshParams);
    mDepthCollectPointLightShaderSkeletal
        = std::make_shared<VertexFactoryCompositeShader<SkeletalMeshVertexFactory<4>, PointLightDepthCollectShader>>(
            skeletalMeshCompositeParams);
    mDepthCollectPointLightShaderNonSkeletal
        = std::make_shared<VertexFactoryCompositeShader<StaticMeshVertexFactory, PointLightDepthCollectShader>>(
            staticMeshCompositeParams);
    m_deferredLightShader = std::make_shared<DeferredLightShader>(deferredLightShaderParams);
}

SceneRenderer::~SceneRenderer()
{
    LogInfo("SceneRenderer::dctor");
    if (mDepthCollectPointLightShaderNonSkeletal) {
        mDepthCollectPointLightShaderNonSkeletal->CleanUp(true);
    }
    if (mDepthCollectPointLightShaderSkeletal) {
        mDepthCollectPointLightShaderSkeletal->CleanUp(true);
    }
    if (mDepthCollectShaderSkeletal) {
        mDepthCollectShaderSkeletal->CleanUp(true);
    }
    if (mDepthCollectShaderNonSkeletal) {
        mDepthCollectShaderNonSkeletal->CleanUp(true);
    }
    if (m_deferredLightShader) {
        m_deferredLightShader->CleanUp(true);
    }
}

void SceneRenderer::CleanUp()
{
    LogInfo("SceneRenderer::CleanUp");
    SceneViewsVector.clear();

    mInstancedGeometryBatchRenderer->CleanUp();

    for (const auto& planarReflectionProxy : PlanarReflectionProxiesVector) {
        planarReflectionProxy->CleanUp();
    }
    PlanarReflectionProxiesVector.clear();

    for (const auto& lightSceneProxy : LightProxiesVector) {
        lightSceneProxy->CleanUp();
    }
    LightProxiesVector.clear();

    for (const auto& primitiveProxy : PrimitiveProxiesVector) {
        primitiveProxy->CleanUp();
    }
    PrimitiveProxiesVector.clear();

    std::for_each(
        MaterialProxiesVector.begin(), MaterialProxiesVector.end(), [](const auto& materialProxy) { materialProxy->CleanUp(); });
    MaterialProxiesVector.erase(
        std::remove_if(
            MaterialProxiesVector.begin(),
            MaterialProxiesVector.end(),
            [](const auto& materialProxy) { return "OutlineMaterial" != materialProxy->MaterialName; }),
        MaterialProxiesVector.end());

    for (const auto& [renderPassType, proxyProvider] : mProxiesProviders) {
        proxyProvider->CleanUp();
    }

    mDirLightProxiesVec.clear();
    mPointLightProxiesVec.clear();
    mSpotlightProxiesVec.clear();
    mGroupedByShadowAtlasLights.clear();

    mDepthCollectShaderNonSkeletal->CleanUp(true);
    mDepthCollectShaderSkeletal->CleanUp(true);
    mDepthCollectPointLightShaderSkeletal->CleanUp(true);
    mDepthCollectPointLightShaderNonSkeletal->CleanUp(true);
    m_deferredLightShader->CleanUp(true);

    mDepthCollectShaderNonSkeletal = nullptr;
    mDepthCollectShaderSkeletal = nullptr;
    mDepthCollectPointLightShaderSkeletal = nullptr;
    mDepthCollectPointLightShaderNonSkeletal = nullptr;
    m_deferredLightShader = nullptr;
}

void SceneRenderer::PostLevelInit()
{
}

InterThreadCommunicationMgr& SceneRenderer::GetInterThreadCommunicationManager()
{
    return m_interThreadMgr;
}

std::shared_ptr<InstancedGeometryBatchRenderer> SceneRenderer::GetInstancedGeometryBatchRenderer() const
{
    return mInstancedGeometryBatchRenderer;
}

void SceneRenderer::OnWindowSizeChanged(const ViewPortInfo& viewPortInfo)
{
    m_gbuffer->ResizeRenderTargets(viewPortInfo);
    m_resolvedSceneFramebuffer->ResizeRenderTargets(viewPortInfo);
    m_resolvedSceneAndUiFramebuffer->ResizeRenderTargets(viewPortInfo);
    mPostFxRenderer->ResizeRenderTargets(viewPortInfo);
}

void SceneRenderer::DepthPrePass(const std::shared_ptr<SceneView>& sceneView)
{
    RenderState renderState;
    renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LESS).SetDepthTestWriteMask(true);
    renderState.GetStencilState().SetIsStencilTestEnabled(false);
    renderState.GetBlendingState().SetIsBlendingEnabled(false);
    renderState.GetColorState().SetColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    renderState.BindRenderState();

    const auto& cameraProxy = sceneView->GetCameraProxy();
    const auto& viewMatrix = cameraProxy->GetViewMatrix();
    const auto& projectionMatrix = cameraProxy->GetProjectionMatrix();

    mInstancedGeometryBatchRenderer->RenderAllBatches(
        cameraProxy, viewMatrix, projectionMatrix, mActiveBindedState, eInstancedGeometryBatchRenderType::DEFERRED);

    const auto depthPrePassProvider
        = std::static_pointer_cast<DepthPrePassProxiesProvider>(mProxiesProviders.at(eRenderPassType::DEPTH_PRE_PASS));
    // Candidates are already filtered (deferred, non-indirect) and sorted by distance to camera by the provider;
    // here we only skip the per-frame/per-view invisible ones.
    const auto& [nonSkeletalProxies, skeletalProxies] = depthPrePassProvider->GetPrimitives();

    if (nonSkeletalProxies.size() > 0) {
        mDepthCollectShaderNonSkeletal->ExecuteShader();
        mDepthCollectShaderNonSkeletal->GetShader()->SetWriteDepthLinearly(false);
        for (auto& proxy : nonSkeletalProxies) {
            if (proxy->IsEnabled() && proxy->IsVisible() && proxy->IsTransformIntialized()
                && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId())) {
                mDepthCollectShaderNonSkeletal->GetVertexFactoryShader()->SetMatrices(
                    proxy->GetMatrix(), viewMatrix, projectionMatrix);

                proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
            }
        }
        mDepthCollectShaderNonSkeletal->StopShader();
    }

    if (skeletalProxies.size() > 0) // Skeletal proxies
    {
        mDepthCollectShaderSkeletal->ExecuteShader();
        mDepthCollectShaderSkeletal->GetShader()->SetWriteDepthLinearly(false);
        for (auto& proxy : skeletalProxies) {
            if (proxy->IsEnabled() && proxy->IsVisible() && proxy->IsTransformIntialized()
                && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId())) {
                mDepthCollectShaderSkeletal->GetVertexFactoryShader()->SetMatrices(
                    proxy->GetMatrix(), viewMatrix, projectionMatrix);
                mDepthCollectShaderSkeletal->GetVertexFactoryShader()->SetSkinningMatrices(proxy->GetSkinningMatrices());

                proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
            }
        }
        mDepthCollectShaderSkeletal->StopShader();
    }

    renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);
    renderState.GetColorState().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    renderState.BindRenderState();
}

void SceneRenderer::ShadowDepthPass(const std::shared_ptr<SceneView>& sceneView)
{
    if (mGroupedByShadowAtlasLights.size()) {
        RenderState renderState;
        renderState.GetCullingState().SetIsCullingEnabled(true).SetCullFaceMode(GL_BACK).SetFrontFace(GL_CCW);
        renderState.GetBlendingState().SetIsBlendingEnabled(false);
        renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);
        renderState.GetStencilState()
            .SetIsStencilTestEnabled(false)
            .SetStencilOperation(0, 0, 0)
            .SetStencilFunction(GL_NOTEQUAL, EngineConstants::eStencilValues::SCENE_DEFAULT, 0xFF)
            .SetStencilMask(0x00);

        renderState.BindRenderState();

        const auto& proxiesProvider
            = std::static_pointer_cast<ShadowDepthPassProxiesProvider>(mProxiesProviders.at(eRenderPassType::SHADOW_DEPTH_PASS));

        for (auto& atlasLightGroup : mGroupedByShadowAtlasLights) {
            bool bNewDepthShadowAtlas = true;

            for (auto& lightProxyPtr : atlasLightGroup.second) {
                auto lightProxyType = lightProxyPtr->GetLightProxyType();

                if (lightProxyType == LightSceneProxyType::DIR_LIGHT) {
                    const auto dirLightPtr = std::static_pointer_cast<DirectionalLightSceneProxy>(lightProxyPtr);

                    if (dirLightPtr->IsEnabled()) {
                        const auto& shadowInfo = dirLightPtr->GetProjectedDirShadowInfo();
                        if (shadowInfo && shadowInfo->IsShadowMapDirty()) {
                            shadowInfo->BindShadowFramebuffer(true, bNewDepthShadowAtlas);

                            const BoundingBox3D& dirLightShadowOrthoBound = dirLightPtr->GetShadowOrthographicProjectionBound();

                            const auto& [nonSkeletalProxies, skeletalProxies]
                                = proxiesProvider->GetPrimitivesForShadowByDescriptorId(
                                    shadowInfo->GetAtlasResource()->GetTextureDescriptor());

                            if (nonSkeletalProxies.size() > 0) // Non - skeletal proxies
                            {
                                mDepthCollectShaderNonSkeletal->ExecuteShader();
                                mDepthCollectShaderNonSkeletal->GetShader()->SetWriteDepthLinearly(false);
                                for (auto& proxy : nonSkeletalProxies) {
                                    const bool bRender = proxy->IsEnabled() && proxy->IsVisible()
                                        && proxy->IsTransformIntialized()
                                        && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
                                    if (bRender
                                        && dirLightShadowOrthoBound.IsIntersectionWithBox(proxy->GetTransformedBoundingBox())) {
                                        const auto& worldMatrix = proxy->GetMatrix();
                                        const auto& viewMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowViewMatrix();
                                        const auto& projectionMatrix
                                            = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowProjectionMatrix();
                                        mDepthCollectShaderNonSkeletal->GetVertexFactoryShader()->SetMatrices(
                                            worldMatrix, viewMatrix, projectionMatrix);

                                        proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                                    }
                                }
                                mDepthCollectShaderNonSkeletal->StopShader();
                            }

                            if (skeletalProxies.size() > 0) // Skeletal proxies
                            {
                                mDepthCollectShaderSkeletal->ExecuteShader();
                                for (auto& proxy : skeletalProxies) {
                                    if (dirLightShadowOrthoBound.IsIntersectionWithBox(proxy->GetTransformedBoundingBox())) {
                                        const auto& worldMatrix = proxy->GetMatrix();
                                        const auto& viewMatrix = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowViewMatrix();
                                        const auto& projectionMatrix
                                            = dirLightPtr->GetProjectedDirShadowInfo()->GetShadowProjectionMatrix();
                                        mDepthCollectShaderSkeletal->GetVertexFactoryShader()->SetMatrices(
                                            worldMatrix, viewMatrix, projectionMatrix);
                                        mDepthCollectShaderSkeletal->GetVertexFactoryShader()->SetSkinningMatrices(
                                            proxy->GetSkinningMatrices());

                                        proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                                    }
                                }
                                mDepthCollectShaderSkeletal->StopShader();
                            }

                            // Next frame shadow map will not be updated unless position of objects
                            // on scene were changed
                            shadowInfo->SetIsShadowMapDirty(false);
                        }
                    }
                } else if (lightProxyType == LightSceneProxyType::SPOT_LIGHT) {
                    const auto spotlightPtr = std::static_pointer_cast<SpotlightSceneProxy>(lightProxyPtr);

                    if (spotlightPtr->IsEnabled()) {
                        const auto& shadowInfo = spotlightPtr->GetProjectedSpotLightShadowInfo();
                        if (shadowInfo && shadowInfo->IsShadowMapDirty()) {
                            shadowInfo->BindShadowFramebuffer(true, bNewDepthShadowAtlas);

                            const auto& [nonSkeletalProxies, skeletalProxies]
                                = proxiesProvider->GetPrimitivesForShadowByDescriptorId(
                                    shadowInfo->GetAtlasResource()->GetTextureDescriptor());

                            if (nonSkeletalProxies.size() > 0) // Non - skeletal proxies
                            {
                                mDepthCollectShaderNonSkeletal->ExecuteShader();
                                mDepthCollectShaderNonSkeletal->GetShader()->SetWriteDepthLinearly(true);
                                for (auto& proxy : nonSkeletalProxies) {
                                    const auto& worldMatrix = proxy->GetMatrix();
                                    const auto& viewMatrix = shadowInfo->GetShadowViewMatrix();
                                    const auto& projectionMatrix = shadowInfo->GetShadowProjectionMatrix();

                                    mDepthCollectShaderNonSkeletal->GetVertexFactoryShader()->SetMatrices(
                                        worldMatrix, viewMatrix, projectionMatrix);
                                    mDepthCollectShaderNonSkeletal->GetShader()->SetInvShadowDistance(
                                        1.0f / spotlightPtr->GetRadianceRadius());
                                    mDepthCollectShaderNonSkeletal->GetShader()->SetLightWorldPosition(
                                        spotlightPtr->GetPosition());

                                    proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                                }
                                mDepthCollectShaderNonSkeletal->StopShader();
                            }

                            if (skeletalProxies.size() > 0) // Skeletal proxies
                            {
                                mDepthCollectShaderSkeletal->ExecuteShader();
                                mDepthCollectShaderSkeletal->GetShader()->SetWriteDepthLinearly(true);

                                for (auto& proxy : skeletalProxies) {
                                    const auto skeletalProxy = std::static_pointer_cast<SkeletalMeshSceneProxy>(proxy);

                                    const auto& worldMatrix = skeletalProxy->GetMatrix();
                                    const auto& viewMatrices = shadowInfo->GetShadowViewMatrix();
                                    const auto& projectionMatrices = shadowInfo->GetShadowProjectionMatrix();

                                    mDepthCollectShaderSkeletal->GetVertexFactoryShader()->SetMatrices(
                                        worldMatrix, viewMatrices, projectionMatrices);
                                    mDepthCollectShaderSkeletal->GetVertexFactoryShader()->SetSkinningMatrices(
                                        skeletalProxy->GetSkinningMatrices());
                                    mDepthCollectShaderSkeletal->GetShader()->SetInvShadowDistance(
                                        1.0f / spotlightPtr->GetRadianceRadius());
                                    mDepthCollectShaderSkeletal->GetShader()->SetLightWorldPosition(spotlightPtr->GetPosition());

                                    skeletalProxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                                }
                                mDepthCollectShaderSkeletal->StopShader();
                            }
                            glBindFramebuffer(GL_FRAMEBUFFER, 0);

                            // Next frame shadow map will not be updated unless position of objects
                            // on scene are changed
                            shadowInfo->SetIsShadowMapDirty(false);
                        }
                    }
                } else if (lightProxyType == LightSceneProxyType::POINT_LIGHT) {
                    const auto pointLightPtr = std::static_pointer_cast<PointLightSceneProxy>(lightProxyPtr);

                    if (pointLightPtr->IsEnabled()) {
                        const auto& shadowInfo = pointLightPtr->GetProjectedPointShadowInfo();
                        if (shadowInfo && shadowInfo->IsShadowMapDirty()) {
                            shadowInfo->BindShadowFramebuffer(true, true); // every point light has it's own depth texture atlas

                            const auto& [nonSkeletalProxies, skeletalProxies]
                                = proxiesProvider->GetPrimitivesForShadowByDescriptorId(
                                    shadowInfo->GetAtlasResource()->GetTextureDescriptor());

                            if (nonSkeletalProxies.size() > 0) // Non - skeletal proxies
                            {
                                mDepthCollectPointLightShaderNonSkeletal->ExecuteShader();
                                for (auto& proxy : nonSkeletalProxies) {
                                    const auto& worldMatrix = proxy->GetMatrix();
                                    const auto& viewMatrices = shadowInfo->GetShadowViewMatrices();
                                    const auto& projectionMatrices = shadowInfo->GetShadowProjectionMatrices();

                                    mDepthCollectPointLightShaderNonSkeletal->GetShader()->SetTransformationMatrices(
                                        viewMatrices, projectionMatrices);
                                    mDepthCollectPointLightShaderNonSkeletal->GetVertexFactoryShader()->SetMatrices(
                                        worldMatrix, glm::mat4(), glm::mat4());
                                    mDepthCollectPointLightShaderNonSkeletal->GetShader()->SetFarPlane(
                                        pointLightPtr->GetRadianceRadius());
                                    mDepthCollectPointLightShaderNonSkeletal->GetShader()->SetPointLightPosition(
                                        pointLightPtr->GetPosition());

                                    proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                                }
                                mDepthCollectPointLightShaderNonSkeletal->StopShader();
                            }

                            if (skeletalProxies.size() > 0) // Skeletal proxies
                            {
                                mDepthCollectPointLightShaderSkeletal->ExecuteShader();
                                for (auto& proxy : skeletalProxies) {
                                    const auto skeletalProxy = std::static_pointer_cast<SkeletalMeshSceneProxy>(proxy);

                                    const auto& worldMatrix = skeletalProxy->GetMatrix();
                                    const auto& viewMatrices = shadowInfo->GetShadowViewMatrices();
                                    const auto& projectionMatrices = shadowInfo->GetShadowProjectionMatrices();

                                    mDepthCollectPointLightShaderSkeletal->GetVertexFactoryShader()->SetMatrices(
                                        worldMatrix, glm::mat4(), glm::mat4());
                                    mDepthCollectPointLightShaderSkeletal->GetVertexFactoryShader()->SetSkinningMatrices(
                                        skeletalProxy->GetSkinningMatrices());
                                    mDepthCollectPointLightShaderSkeletal->GetShader()->SetTransformationMatrices(
                                        viewMatrices, projectionMatrices);
                                    mDepthCollectPointLightShaderSkeletal->GetShader()->SetFarPlane(
                                        pointLightPtr->GetRadianceRadius());
                                    mDepthCollectPointLightShaderSkeletal->GetShader()->SetPointLightPosition(
                                        pointLightPtr->GetPosition());

                                    skeletalProxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                                }
                                mDepthCollectPointLightShaderSkeletal->StopShader();
                            }
                            glBindFramebuffer(GL_FRAMEBUFFER, 0);

                            // Next frame shadow map will not be updated unless position of objects
                            // in the level are changed
                            shadowInfo->SetIsShadowMapDirty(false);
                        }
                    }
                }

                bNewDepthShadowAtlas = false;
            }
        }
    }
}

void SceneRenderer::DeferredBasePass_RenderThread(const std::shared_ptr<SceneView>& sceneView)
{
    RenderState renderState;
    renderState.GetCullingState().SetIsCullingEnabled(true).SetCullFaceMode(GL_BACK).SetFrontFace(GL_CCW);
    renderState.BindRenderState();

    renderState.GetBlendingState().SetIsBlendingEnabled(false);
    renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(false);
    renderState.GetStencilState()
        .SetIsStencilTestEnabled(true)
        .SetStencilOperation(GL_KEEP, GL_KEEP, GL_REPLACE)
        .SetStencilFunction(GL_ALWAYS, EngineConstants::eStencilValues::SCENE_DEFAULT, 0xFF)
        .SetStencilMask(0xFF);
    renderState.BindRenderState();

    // Deferred shading collect info
    const auto& cameraProxy = sceneView->GetCameraProxy();
    const auto& viewMatrix = cameraProxy->GetViewMatrix();
    const auto& projectionMatrix = cameraProxy->GetProjectionMatrix();

    mInstancedGeometryBatchRenderer->RenderAllBatches(
        cameraProxy, viewMatrix, projectionMatrix, mActiveBindedState, eInstancedGeometryBatchRenderType::DEFERRED);

    const auto deferredBasePassProvider
        = std::static_pointer_cast<DeferredBasePassProxiesProvider>(mProxiesProviders.at(eRenderPassType::DEFERRED_BASE_PASS));
    const auto& deferredProxies = deferredBasePassProvider->GetPrimitives();

    // Only enabled, visible and passed frustum-cull test proxies should be rendered
    for (auto& proxy : deferredProxies) {
        if (proxy->IsEnabled() && proxy->IsVisible() && proxy->IsTransformIntialized()
            && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId())) {
            const int32_t stencilFuncRefValue = proxy->CanBloomBeApplied() ? EngineConstants::eStencilValues::BLOOM
                                                                           : EngineConstants::eStencilValues::SCENE_DEFAULT;
            renderState.GetStencilState().SetStencilFunction(GL_ALWAYS, stencilFuncRefValue, 0xFF);
            renderState.BindRenderState();
            proxy->Render(cameraProxy, viewMatrix, projectionMatrix, mActiveBindedState);
        }
    }

    renderState.GetStencilState().SetIsStencilTestEnabled(false);
}

void SceneRenderer::DeferredLightPass_RenderThread(const std::shared_ptr<CameraSceneProxy>& cameraProxy)
{
    RenderState renderState;
    renderState.GetBlendingState().SetIsBlendingEnabled(false);

    renderState.GetDepthState().SetIsDepthTestEnabled(false).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(false);

    renderState.GetStencilState()
        .SetIsStencilTestEnabled(false)
        .SetStencilOperation(0, 0, 0)
        .SetStencilFunction(GL_NOTEQUAL, EngineConstants::eStencilValues::SCENE_DEFAULT, 0xFF)
        .SetStencilMask(0);
    renderState.BindRenderState();
    // TODO: Make some check if light source (point or spot light) is too far from current view
    // position
    m_deferredLightShader->ExecuteShader();

#ifndef NO_LIT
    // ************************** SHADOWS ************************** //
    size_t pointLightIndex = 0, dirLightIndex = 0, spotlightIndex = 0;
    size_t dirShadowMapCount = 0, pointShadowMapCount = 0, spotlightShadowMapCount = 0;
    const auto bindShadowAtlas = [this](const auto& atlasResource) {
        const auto occupiedSlot = mActiveBindedState.OccupyTextureSlot(atlasResource->GetTextureDescriptor());
        if (!occupiedSlot.bWasAlreadyBound) {
            atlasResource->BindTexture(occupiedSlot.SlotIndex);
        }
        return occupiedSlot.SlotIndex;
    };
    for (auto& dirLightProxy : mDirLightProxiesVec) {
        if (dirLightProxy->IsEnabled()) {
            const auto shadowInfo = dirLightProxy->GetProjectedDirShadowInfo();
            if (shadowInfo) {
                const int32_t shadowMapSlot = bindShadowAtlas(shadowInfo->GetAtlasResource());
                m_deferredLightShader->SetDirectionalLightShadowMapSlot(
                    dirLightIndex, shadowMapSlot, shadowInfo->GetTextureAtlasOffset());

                dirShadowMapCount++;
                dirLightIndex++;
            }
        }
    }

    for (auto& pointLightProxy : mPointLightProxiesVec) {
        if (pointLightProxy->IsEnabled()) {
            const auto shadowInfo = pointLightProxy->GetProjectedPointShadowInfo();
            if (shadowInfo) {
                const int32_t shadowMapSlot = bindShadowAtlas(shadowInfo->GetAtlasResource());
                m_deferredLightShader->SetPointLightShadowMapSlot(pointLightIndex, shadowMapSlot);
                pointShadowMapCount++;
                pointLightIndex++;
            }
        }
    }

    for (const auto& spotLightProxy : mSpotlightProxiesVec) {
        if (spotLightProxy->IsEnabled()) {
            const auto shadowInfo = spotLightProxy->GetProjectedSpotLightShadowInfo();
            if (shadowInfo) {
                const int32_t shadowMapSlot = bindShadowAtlas(shadowInfo->GetAtlasResource());
                m_deferredLightShader->SetSpotlightShadowMapSlot(
                    spotlightIndex, shadowMapSlot, shadowInfo->GetTextureAtlasOffset());
                spotlightShadowMapCount++;
                spotlightIndex++;
            }
        }
    }
// ************************** SHADOWS ************************** //
#endif
    m_deferredLightShader->SetCameraWorldPosition(cameraProxy->GetEyeVector());

    m_deferredLightShader->SetGBufferPosition(m_gbuffer->BindPositionTexture(mActiveBindedState));
    m_deferredLightShader->SetGBufferAlbedo(m_gbuffer->BindAlbedoTexture(mActiveBindedState));
    m_deferredLightShader->SetGBufferNormal(m_gbuffer->BindNormalTexture(mActiveBindedState));
    m_deferredLightShader->SetGBufferEmission(m_gbuffer->BindEmissionTexture(mActiveBindedState));

#ifdef SHADING_MODEL_PBR
    m_deferredLightShader->SetGBufferMetallicRoughness(m_gbuffer->BindMetallicRoughnessTexture(mActiveBindedState));
#endif

#ifndef NO_LIT
    m_deferredLightShader->SetLightsInfo(LightProxiesVector);
#endif
    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLE_STRIP);
    m_deferredLightShader->StopShader();

    renderState.GetCullingState().SetIsCullingEnabled(false);
    renderState.GetDepthState().SetDepthTestWriteMask(true);
    renderState.BindRenderState();
}

void SceneRenderer::ForwardBasePass_RenderThread(const std::shared_ptr<SceneView>& sceneView)
{
    const auto forwardBasePassProvider
        = std::static_pointer_cast<ForwardPassProxiesProvider>(mProxiesProviders.at(eRenderPassType::FORWARD_BASE_PASS));
    // Candidates are already filtered (forward, non-indirect) and sorted by order and shader by the provider.
    const auto& sortedForwardProxies = forwardBasePassProvider->GetPrimitives();
    if (sortedForwardProxies.empty())
        return;

    RenderState renderState;
    renderState.GetBlendingState().SetIsBlendingEnabled(true).SetBlendingFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);

    renderState.GetStencilState()
        .SetIsStencilTestEnabled(true)
        .SetStencilOperation(GL_KEEP, GL_KEEP, GL_REPLACE)
        .SetStencilFunction(GL_ALWAYS, EngineConstants::eStencilValues::SCENE_DEFAULT, 0xFF)
        .SetStencilMask(0xFF);
    renderState.GetCullingState().SetIsCullingEnabled(true).SetCullFaceMode(GL_BACK).SetFrontFace(GL_CCW);
    renderState.BindRenderState();

    // Resolve depth from gBuffer to default frame buffer
    auto cameraProxy = sceneView->GetCameraProxy();
    auto cameraViewPort = cameraProxy->GetViewPort();

    const auto originX = cameraViewPort.OriginX, originY = cameraViewPort.OriginY, screenWidth = cameraViewPort.Width,
               screenHeight = cameraViewPort.Height;

    m_gbuffer->CopyFramebufferDataToDstFramebuffer(
        m_resolvedSceneFramebuffer->GetFramebufferObjectInstance(),
        originX,
        originY,
        screenWidth,
        screenHeight,
        originX,
        originY,
        screenWidth,
        screenHeight,
        GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    constexpr int NoClearFlag = 0;
    m_resolvedSceneFramebuffer->BindResolvedSceneFramebuffer(NoClearFlag);

    const auto& viewMatrix = cameraProxy->GetViewMatrix();
    const auto& projectionMatrix = cameraProxy->GetProjectionMatrix();

    mInstancedGeometryBatchRenderer->RenderAllBatches(
        cameraProxy, viewMatrix, projectionMatrix, mActiveBindedState, eInstancedGeometryBatchRenderType::FORWARD);

    bool depthTestWriteMask = true;
    for (const auto& proxy : sortedForwardProxies) {
        if (proxy->IsDepthWriteMaskEnabled() != depthTestWriteMask) {
            depthTestWriteMask = proxy->IsDepthWriteMaskEnabled();
            renderState.GetDepthState().SetDepthTestWriteMask(depthTestWriteMask);
            renderState.BindRenderState();
        }

        if (proxy->IsEnabled() && proxy->IsVisible() && proxy->IsTransformIntialized()
            && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId())) {
            const int32_t stencilFuncRefValue = proxy->CanBloomBeApplied() ? EngineConstants::eStencilValues::BLOOM
                                                                           : EngineConstants::eStencilValues::SCENE_DEFAULT;
            renderState.GetStencilState().SetStencilFunction(GL_ALWAYS, stencilFuncRefValue, 0xFF);
            renderState.BindRenderState();
            proxy->Render(cameraProxy, viewMatrix, projectionMatrix, mActiveBindedState);
        }
    }

    renderState.GetDepthState().SetDepthTestWriteMask(true);
    renderState.GetBlendingState().SetIsBlendingEnabled(false);
    renderState.GetStencilState().SetIsStencilTestEnabled(false);
    renderState.GetCullingState().SetIsCullingEnabled(false);
    renderState.BindRenderState();

    m_resolvedSceneFramebuffer->UnbindFramebuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void SceneRenderer::OutlinePass(const std::shared_ptr<SceneView>& sceneView)
{
    const auto outlinePassProvider
        = std::static_pointer_cast<OutlinePassProxiesProvider>(mProxiesProviders.at(eRenderPassType::OUTLINE_PASS));
    const auto& outlineCandidates = outlinePassProvider->GetPrimitives();

    // Only enabled, visible and passed frustum-cull test proxies should be rendered
    std::vector<std::shared_ptr<PrimitiveSceneProxy>> visibleProxies;
    visibleProxies.reserve(outlineCandidates.size());
    std::copy_if(
        outlineCandidates.cbegin(),
        outlineCandidates.cend(),
        std::back_inserter(visibleProxies),
        [&sceneView](const auto& proxy) {
            return proxy->IsEnabled() && proxy->IsVisible() && proxy->IsTransformIntialized()
                && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
        });

    if (visibleProxies.empty()) {
        return;
    }

    RenderState renderState;
    renderState.GetStencilState()
        .SetIsStencilTestEnabled(true)
        .SetStencilOperation(GL_KEEP, GL_KEEP, GL_REPLACE)
        .SetStencilFunction(GL_ALWAYS, EngineConstants::eStencilValues::OUTLINE, 0xFF)
        .SetStencilMask(0xFF);
    renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);
    renderState.GetColorState().SetColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    renderState.BindRenderState();

    const auto& cameraProxy = sceneView->GetCameraProxy();
    const auto& viewMatrix = cameraProxy->GetViewMatrix();
    const auto& projectionMatrix = cameraProxy->GetProjectionMatrix();

    // Write outline value to stencil for objects which has to be outlined

    for (auto& proxy : visibleProxies) {
        if (proxy->GetIsOutlineApplied()) {
            proxy->RenderOutlineStencil(cameraProxy, viewMatrix, projectionMatrix, mActiveBindedState);
        }
    }

    renderState.GetColorState().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    renderState.GetStencilState()
        .SetIsStencilTestEnabled(true)
        .SetStencilOperation(GL_KEEP, GL_KEEP, GL_REPLACE)
        .SetStencilFunction(GL_NOTEQUAL, EngineConstants::eStencilValues::OUTLINE, 0xFF)
        .SetStencilMask(0x00);

    renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);
    renderState.BindRenderState();

    // Draw outline (scaled up objects) only where stencil value is not equal to outline
    for (auto& proxy : visibleProxies) {
        if (proxy->GetIsOutlineApplied()) {
            proxy->RenderOutline(cameraProxy, viewMatrix, projectionMatrix, mActiveBindedState);
        }
    }

    renderState.GetStencilState()
        .SetIsStencilTestEnabled(true)
        .SetStencilOperation(GL_KEEP, GL_KEEP, GL_REPLACE)
        .SetStencilFunction(GL_ALWAYS, EngineConstants::eStencilValues::SCENE_DEFAULT, 0xFF)
        .SetStencilMask(0xFF);
    renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);
    renderState.BindRenderState();
}

void SceneRenderer::PlanarReflectionPass()
{
    if (PlanarReflectionProxiesVector.size() <= 0)
        return;

    RenderState renderState;
    renderState.GetBlendingState().SetIsBlendingEnabled(false);
    renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);
    renderState.GetStencilState()
        .SetIsStencilTestEnabled(false)
        .SetStencilOperation(0, 0, 0)
        .SetStencilFunction(GL_NOTEQUAL, EngineConstants::eStencilValues::SCENE_DEFAULT, 0xFF)
        .SetStencilMask(0);
    renderState.GetCullingState().SetIsCullingEnabled(true).SetCullFaceMode(GL_BACK).SetFrontFace(GL_CW);
    renderState.GetClipPlaneState().SetIsClipPlaneEnabled(0, true);
    renderState.GetColorState().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    renderState.BindRenderState();

    const auto planarReflectionPassProvider = std::static_pointer_cast<PlanarReflectionPassProxiesProvider>(
        mProxiesProviders.at(eRenderPassType::PLANAR_REFLECTION_PASS));

    for (const auto& planarReflectionProxy : PlanarReflectionProxiesVector) {
        auto sceneViewWp = planarReflectionProxy->GetSceneViewWeakPtr();
        if (auto scenViewSp = sceneViewWp.lock()) {
            const auto& viewMatrix = scenViewSp->GetCameraProxy()->GetViewMatrix();
            const auto& projectionMatrix = scenViewSp->GetCameraProxy()->GetProjectionMatrix();
            const glm::mat4& mirrorMatrix = planarReflectionProxy->GetMirrorMatrix();
            const glm::vec4& mirrorPlane = planarReflectionProxy->GetReflectionPlane();

            const CameraFrustum& mirroredCameraFrustum
                = CameraFrustum::GetConstructedFromViewProjectionMatrices(viewMatrix * mirrorMatrix, projectionMatrix);

            // Candidates are already collected (non-indirect) and sorted by distance to the reflection plane origin
            // by the provider; here we only keep the ones visible from the mirrored camera frustum.
            const auto& planarCandidates
                = planarReflectionPassProvider->GetPrimitivesForPlane(planarReflectionProxy->GetSceneProxyId());

            std::vector<std::shared_ptr<PrimitiveSceneProxy>> visibleProxies;
            visibleProxies.reserve(planarCandidates.size());
            std::copy_if(
                planarCandidates.cbegin(),
                planarCandidates.cend(),
                std::back_inserter(visibleProxies),
                [&mirroredCameraFrustum](const auto& proxy) {
                    return proxy->IsEnabled() && proxy->IsVisible() && proxy->IsTransformIntialized()
                        && (proxy->IsFrustumCullTestNeeded()
                                ? mirroredCameraFrustum.CollidesWithBoundingBox(proxy->GetTransformedBoundingBox())
                                : true);
                });

            if (not visibleProxies.empty()) {
                planarReflectionProxy->RenderToPlanarReflectionFBO();

                for (auto& proxy : visibleProxies) {
                    proxy->RenderPlanarReflection(mirrorPlane, mirrorMatrix, viewMatrix, projectionMatrix, mActiveBindedState);
                }

                planarReflectionProxy->StopRenderingToPlanarReflectionFBO();
                planarReflectionProxy->ResolveReflectionRenderTargetSurfaceData();
            }
        }
    }

    renderState.GetCullingState().SetIsCullingEnabled(false).SetCullFaceMode(GL_BACK).SetFrontFace(GL_CCW);
    renderState.GetClipPlaneState().SetIsClipPlaneEnabled(0, false);
    renderState.BindRenderState();
}

void SceneRenderer::GuiPass(const std::shared_ptr<SceneView>& sceneView)
{
    RenderState renderState;
    renderState.GetBlendingState().SetIsBlendingEnabled(true).SetBlendingFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderState.GetDepthState().SetIsDepthTestEnabled(false).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(false);

    renderState.GetStencilState()
        .SetIsStencilTestEnabled(true)
        .SetStencilOperation(GL_KEEP, GL_KEEP, GL_REPLACE)
        .SetStencilFunction(GL_ALWAYS, 0x00, 0xFF)
        .SetStencilMask(0xFF);

    renderState.BindRenderState();

    // Alpha channel of UI framebuffer should accumulate coverage, not blend as color.
    // Separate mode: RGB as usual (SRC_ALPHA, ONE_MINUS_SRC_ALPHA), alpha — (ONE, ONE_MINUS_SRC_ALPHA).
    // So RGB in framebuffer is premultiplied, and alpha is correct coverage for subsequent compositing over the scene.
    renderState.GetBlendingState().SetBlendingFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    m_resolvedSceneAndUiFramebuffer->BindResolvedSceneFramebuffer(
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    std::sort(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [](const auto& left, const auto& right) {
        return left->GetCanvasZOrder() < right->GetCanvasZOrder();
    });

    for (const auto& canvas : mUiCanvasProxies) {
        if (canvas->IsVisible()) {
            const auto& canvasOrigin = canvas->GetAbsoluteOrigin();
            const auto& widthHeight = canvas->GetWidthHeight();
            glViewport(canvasOrigin.x, canvasOrigin.y, widthHeight.x, widthHeight.y);
            canvas->Render(mPostFxRenderer, sceneView->GetCameraProxy());
        }
    }

    const auto& viewPortInfo = sceneView->GetCameraProxy()->GetViewPort();
    glViewport(viewPortInfo.OriginX, viewPortInfo.OriginY, viewPortInfo.Width, viewPortInfo.Height);

    renderState.GetDepthState().SetDepthTestWriteMask(true);
    renderState.BindRenderState();
}

void SceneRenderer::FilterLightProxies()
{
    if (IsLightProxiesDirty()) {
        mDirLightProxiesVec.clear();
        mPointLightProxiesVec.clear();
        mSpotlightProxiesVec.clear();

        for (auto& proxy : LightProxiesVector) {
            const LightSceneProxyType lightType = proxy->GetLightProxyType();

            if (lightType == LightSceneProxyType::DIR_LIGHT) {
                mDirLightProxiesVec.emplace_back(std::static_pointer_cast<DirectionalLightSceneProxy>(proxy));
            } else if (lightType == LightSceneProxyType::POINT_LIGHT) {
                mPointLightProxiesVec.emplace_back(std::static_pointer_cast<PointLightSceneProxy>(proxy));
            } else if (lightType == LightSceneProxyType::SPOT_LIGHT) {
                mSpotlightProxiesVec.emplace_back(std::static_pointer_cast<SpotlightSceneProxy>(proxy));
            }
        }

        GroupLightsByShadowMap();
        // Note: bLightProxiesDirty is intentionally not cleared here. SortSceneProxies still needs to observe it this
        // frame to decide whether the shadow depth provider must be re-sorted, and it clears all the dirty flags afterwards.
    }
}

void SceneRenderer::SortSceneProxies(const std::shared_ptr<SceneView>& sceneView)
{
    const auto& cameraProxy = sceneView->GetCameraProxy();

    // Distance/plane-ordered providers (shadow depth, depth pre-pass, outline, planar reflection) sort front-to-back as
    // an early-Z heuristic, so an exact order is not required. Rather than re-sorting them every frame whenever a proxy
    // moves, refresh that order at most once every c_distanceSortRefreshFrameInterval frames — and only if a relevant
    // proxy actually moved. Membership (add/remove) and light changes still force an immediate re-sort.
    static constexpr uint32_t c_distanceSortRefreshFrameInterval = 10;
    ++mFramesSinceDistanceSortRefresh;
    const bool bDistanceRefreshDue = mFramesSinceDistanceSortRefresh >= c_distanceSortRefreshFrameInterval;
    const bool bDeferredOrderStale = bDistanceRefreshDue && bDeferredPrimitivesMoved;
    const bool bAnyPrimitiveOrderStale = bDistanceRefreshDue && (bDeferredPrimitivesMoved || bForwardPrimitivesMoved);

    // A deferred primitive membership change touches every provider that consumes deferred primitives; a forward
    // membership/sort-order change touches the forward and planar reflection providers; a light change only touches the
    // shadow depth provider; a planar reflection change only touches the planar reflection provider. Movement only
    // restales the distance/plane order, refreshed on the periodic cadence above. The deferred base and forward base
    // providers do not order by distance, so movement never restales them.
    const bool bResetShadowDepth = bDeferredPrimitivesDirty || bLightProxiesDirty || bLightProxiesTransformDirty || bDeferredOrderStale;
    const bool bResetPlanarReflection
        = bDeferredPrimitivesDirty || bForwardPrimitivesDirty || bPlanarReflectionProxiesDirty || bAnyPrimitiveOrderStale;
    const bool bResetOutline = bDeferredPrimitivesDirty || bDeferredOrderStale;
    const bool bResetDepthPrePass = bDeferredPrimitivesDirty || bDeferredOrderStale;
    const bool bResetDeferredBase = bDeferredPrimitivesDirty;
    const bool bResetForwardBase = bForwardPrimitivesDirty;

    if (bResetShadowDepth) {
        std::static_pointer_cast<ShadowDepthPassProxiesProvider>(mProxiesProviders[eRenderPassType::SHADOW_DEPTH_PASS])
            ->Reset(PrimitiveProxiesVector, LightProxiesVector);
    }
    if (bResetPlanarReflection) {
        std::static_pointer_cast<PlanarReflectionPassProxiesProvider>(mProxiesProviders[eRenderPassType::PLANAR_REFLECTION_PASS])
            ->Reset(PrimitiveProxiesVector, PlanarReflectionProxiesVector);
    }
    if (bResetOutline) {
        std::static_pointer_cast<OutlinePassProxiesProvider>(mProxiesProviders[eRenderPassType::OUTLINE_PASS])
            ->Reset(PrimitiveProxiesVector, cameraProxy->GetEyeVector());
    }
    if (bResetDepthPrePass) {
        std::static_pointer_cast<DepthPrePassProxiesProvider>(mProxiesProviders[eRenderPassType::DEPTH_PRE_PASS])
            ->Reset(PrimitiveProxiesVector, cameraProxy->GetEyeVector());
    }
    if (bResetDeferredBase) {
        std::static_pointer_cast<DeferredBasePassProxiesProvider>(mProxiesProviders[eRenderPassType::DEFERRED_BASE_PASS])
            ->Reset(PrimitiveProxiesVector);
    }
    if (bResetForwardBase) {
        std::static_pointer_cast<ForwardPassProxiesProvider>(mProxiesProviders[eRenderPassType::FORWARD_BASE_PASS])
            ->Reset(PrimitiveProxiesVector);
    }

    bDeferredPrimitivesDirty = false;
    bForwardPrimitivesDirty = false;
    bLightProxiesDirty = false;
    bLightProxiesTransformDirty = false;
    bPlanarReflectionProxiesDirty = false;

    // When the periodic refresh window elapsed, restart it and drop the accumulated movement flags (the distance/plane
    // order is now considered up to date until the next move).
    if (bDistanceRefreshDue) {
        mFramesSinceDistanceSortRefresh = 0;
        bDeferredPrimitivesMoved = false;
        bForwardPrimitivesMoved = false;
    }
}

void SceneRenderer::GroupLightsByShadowMap()
{
    mGroupedByShadowAtlasLights.clear();
    for (auto& proxy : LightProxiesVector) {
        if (auto shadowInfo = proxy->GetShadowInfo()) {
            if (auto atlasResource = shadowInfo->GetAtlasResource()) {
                auto lastDesc = atlasResource->GetTextureDescriptor();

                const auto groupIt = std::find_if(
                    mGroupedByShadowAtlasLights.begin(), mGroupedByShadowAtlasLights.end(), [=](const auto& lightGroupPair) {
                        return lightGroupPair.first == lastDesc;
                    });

                if (groupIt == mGroupedByShadowAtlasLights.end()) {
                    std::vector<std::shared_ptr<LightSceneProxy>> result;

                    for (const auto& proxy : LightProxiesVector) {
                        if (proxy->GetShadowInfo() && proxy->GetShadowInfo()->GetAtlasResource()) {
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

void SceneRenderer::RenderScene_RenderThread()
{
    ext_assert(mPostFxRenderer, "PostFxRenderer is not initialized!");

    FilterLightProxies();

    for (const auto& sceneView : SceneViewsVector) {
        const auto& cameraProxy = sceneView->GetCameraProxy();
        if (cameraProxy->IsInitializedFirstTime()) {
            sceneView->FrustumCullTest(PrimitiveProxiesVector);

            // Deferred shading is done with main camera
            if (eCameraSceneProxyType::MAIN_SCENE_CAMERA == cameraProxy->GetCameraSceneType()) {

                mActiveBindedState.Reset();

                SortSceneProxies(sceneView);

                ShadowDepthPass(sceneView);

                PlanarReflectionPass();

                m_gbuffer->BindDeferredGBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

                OutlinePass(sceneView);

                DepthPrePass(sceneView);

                DeferredBasePass_RenderThread(sceneView);

                m_resolvedSceneFramebuffer->BindResolvedSceneFramebuffer(
                    GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

                DeferredLightPass_RenderThread(cameraProxy);

                ForwardBasePass_RenderThread(sceneView);

                mPostFxRenderer->ExecuteResolveSceneColor(m_resolvedSceneFramebuffer);

                GuiPass(sceneView);

                mPostFxRenderer->ExecuteResolveGuiColor(m_resolvedSceneAndUiFramebuffer);
                // TODO: rendering to render texture later....
            } else {
                // TODO: rendering to render texture later....
            }

#if DEBUG
            if (bRenderDebugPhysicsData) {
                DebugRenderPhysics(sceneView->GetCameraProxy()->GetViewMatrix(), cameraProxy->GetProjectionMatrix());
            }
#endif
        }
    }
}

void SceneRenderer::SetPrimitiveProxiesDirty(const bool bDirty)
{
    bDeferredPrimitivesDirty = bDirty;
    bForwardPrimitivesDirty = bDirty;
}

void SceneRenderer::SetDeferredPrimitivesDirty(const bool bDirty)
{
    bDeferredPrimitivesDirty = bDirty;
}

void SceneRenderer::SetForwardPrimitivesDirty(const bool bDirty)
{
    bForwardPrimitivesDirty = bDirty;
}

void SceneRenderer::SetDeferredPrimitivesMoved(const bool bMoved)
{
    bDeferredPrimitivesMoved = bMoved;
}

void SceneRenderer::SetForwardPrimitivesMoved(const bool bMoved)
{
    bForwardPrimitivesMoved = bMoved;
}

void SceneRenderer::SetLightProxiesDirty(const bool bDirty)
{
    bLightProxiesDirty = bDirty;
}

void SceneRenderer::SetLightProxiesTransformDirty(const bool bDirty)
{
    bLightProxiesTransformDirty = bDirty;
}

void SceneRenderer::SetPlanarReflectionProxiesDirty(const bool bDirty)
{
    bPlanarReflectionProxiesDirty = bDirty;
}

bool SceneRenderer::IsPrimitiveProxiesDirty() const
{
    return bDeferredPrimitivesDirty || bForwardPrimitivesDirty;
}

bool SceneRenderer::IsLightProxiesDirty() const
{
    return bLightProxiesDirty;
}

bool SceneRenderer::IsPlanarReflectionProxiesDirty() const
{
    return bPlanarReflectionProxiesDirty;
}

std::shared_ptr<SceneView> SceneRenderer::GetSceneViewByCameraProxyId(const int32_t proxyId) const
{
    std::shared_ptr<SceneView> result = nullptr;

    const auto foundSceneIt = std::find_if(SceneViewsVector.begin(), SceneViewsVector.end(), [=](const auto& sceneView) {
        return proxyId == sceneView->GetCameraProxy()->GetSceneProxyId();
    });

    if (foundSceneIt != SceneViewsVector.end()) {
        result = *foundSceneIt;
    }

    return result;
}

std::shared_ptr<PrimitiveSceneProxy> SceneRenderer::GetPrimitiveProxyByProxyId(const int32_t proxyId) const
{
    const auto foundPrimitiveProxyIt
        = std::find_if(PrimitiveProxiesVector.cbegin(), PrimitiveProxiesVector.cend(), [=](const auto& primitiveProxy) {
              return proxyId == primitiveProxy->GetSceneProxyId();
          });

    if (foundPrimitiveProxyIt != PrimitiveProxiesVector.cend()) {
        return *foundPrimitiveProxyIt;
    }
    return nullptr;
}

std::shared_ptr<LightSceneProxy> SceneRenderer::GetLightProxyByProxyId(const int32_t proxyId) const
{
    const auto foundLightProxyIt
        = std::find_if(LightProxiesVector.begin(), LightProxiesVector.end(), [=](const auto& lightProxy) {
              return proxyId == lightProxy->GetSceneProxyId();
          });

    if (foundLightProxyIt != LightProxiesVector.end()) {
        return *foundLightProxyIt;
    }
    return nullptr;
}

std::shared_ptr<MaterialProxy> SceneRenderer::GetMaterialProxyByProxyId(const int32_t proxyId) const
{
    std::shared_ptr<MaterialProxy> result = nullptr;

    const auto foundMaterialProxyIt
        = std::find_if(MaterialProxiesVector.begin(), MaterialProxiesVector.end(), [=](const auto& materialProxy) {
              return proxyId == materialProxy->GetSceneProxyId();
          });

    if (foundMaterialProxyIt != MaterialProxiesVector.end()) {
        result = *foundMaterialProxyIt;
    }

    return result;
}

std::shared_ptr<PlanarReflectionProxy> SceneRenderer::GetPlanarReflectionProxyByProxyId(const int32_t proxyId) const
{
    std::shared_ptr<PlanarReflectionProxy> result = nullptr;

    const auto foundPlanarProxyIt
        = std::find_if(PlanarReflectionProxiesVector.begin(), PlanarReflectionProxiesVector.end(), [=](const auto& planarProxy) {
              return proxyId == planarProxy->GetSceneProxyId();
          });

    if (foundPlanarProxyIt != PlanarReflectionProxiesVector.end()) {
        result = *foundPlanarProxyIt;
    }

    return result;
}

std::vector<std::shared_ptr<PrimitiveSceneProxy>>& SceneRenderer::GetPrimitiveProxies()
{
    return PrimitiveProxiesVector;
}

std::shared_ptr<FreeTypeFontHandler> SceneRenderer::GetFontHandler() const
{
    return mFreeTypeFontHandler;
}

std::shared_ptr<UiSceneProxyBase> SceneRenderer::GetUiSceneProxyByProxyId(const size_t proxyId, const size_t canvasId) const
{
    auto canvasIt = std::find_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [=](const auto& canvasProxy) {
        return canvasId == canvasProxy->GetUiItemUId();
    });
    return canvasIt != mUiCanvasProxies.end() ? (*canvasIt)->GetSceneProxyById(proxyId) : nullptr;
}

std::shared_ptr<UiCanvasSceneProxy> SceneRenderer::GetCanvasSceneProxyByProxyId(const size_t proxyId) const
{
    auto canvasIt = std::find_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [=](const auto& canvasProxy) {
        return proxyId == canvasProxy->GetUiItemUId();
    });
    return canvasIt != mUiCanvasProxies.end() ? *canvasIt : nullptr;
}

void SceneRenderer::RemoveUiSceneProxyByProxyId(const size_t proxyId, const size_t canvasId)
{
    auto canvasIt = std::find_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [=](const auto& canvasProxy) {
        return canvasId == canvasProxy->GetUiItemUId();
    });
    if (canvasIt != mUiCanvasProxies.end()) {
        (*canvasIt)->RemoveUiSceneProxy(proxyId);
    }
}

void SceneRenderer::RemovePrimitiveProxyByProxyId(const int32_t proxyId)
{
    PrimitiveProxiesVector.erase(
        std::remove_if(PrimitiveProxiesVector.begin(), PrimitiveProxiesVector.end(), [=](const auto& primitiveProxy) {
            return proxyId == primitiveProxy->GetSceneProxyId();
        }));
}

void SceneRenderer::RemoveLightProxyByProxyId(const int32_t proxyId)
{
    LightProxiesVector.erase(std::remove_if(LightProxiesVector.begin(), LightProxiesVector.end(), [=](const auto& lightProxy) {
        return proxyId == lightProxy->GetSceneProxyId();
    }));
}

void SceneRenderer::RemovePlanarReflectionSceneProxyByProxyId(const int32_t proxyId)
{
    PlanarReflectionProxiesVector.erase(
        std::remove_if(
            PlanarReflectionProxiesVector.begin(),
            PlanarReflectionProxiesVector.end(),
            [proxyId](const auto& planarReflectionProxy) { return planarReflectionProxy->GetSceneProxyId() == proxyId; }));
}

void SceneRenderer::AddMaterialProxy_OnRenderThread(const std::shared_ptr<MaterialProxy>& materialProxy)
{
    static const uint64_t functionId = Hash64_CT("SceneRenderer::AddMaterialProxy_OnRenderThread");

    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        ext_assert(
            !GetMaterialProxyByProxyId(materialProxy->GetSceneProxyId()),
            "Material proxy with id {} already exists" + std::to_string(materialProxy->GetSceneProxyId()));
        MaterialProxiesVector.emplace_back(materialProxy);
        LogInfo(
            "SceneRenderer::AddMaterialProxy_OnRenderThread: material name: ",
            materialProxy->MaterialName,
            "proxyId: ",
            materialProxy->GetSceneProxyId());
    } else {
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            materialProxy->GetSceneProxyId(),
            functionId,
            [weak = weak_from_this(), materialProxy](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    ext_assert(
                        !sceneRenderer->GetMaterialProxyByProxyId(materialProxy->GetSceneProxyId()),
                        "Material proxy with id {} already exists" + std::to_string(materialProxy->GetSceneProxyId()));
                    sceneRenderer->MaterialProxiesVector.emplace_back(materialProxy);
                    LogInfo(
                        "SceneRenderer::AddMaterialProxy_OnRenderThread: material name: ",
                        materialProxy->MaterialName,
                        "proxyId: ",
                        materialProxy->GetSceneProxyId());
                }
            });
    }
}

void SceneRenderer::UpdatePrimitiveComponentEnable_OnRenderThread(
    const int32_t primitiveSceneProxyIndex, const int32_t creatorObjectId, const uint64_t functionId, const bool bEnabled)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        const auto& primitiveSp = GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
        if (primitiveSp) {
            primitiveSp->SetEnabled(bEnabled);
        }
    } else {
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            creatorObjectId,
            functionId,
            [weak = weak_from_this(), primitiveSceneProxyIndex, bEnabled](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    const auto& primitiveSp = sceneRenderer->GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
                    if (primitiveSp) {
                        primitiveSp->SetEnabled(bEnabled);
                    }
                }
            });
    }
}

void SceneRenderer::UpdateLightComponentEnable_OnRenderThread(
    const int32_t lightSceneProxyIndex, const int32_t creatorObjectId, const uint64_t functionId, const bool bEnabled)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        const auto& lightSp = GetLightProxyByProxyId(lightSceneProxyIndex);
        if (lightSp) {
            lightSp->SetEnabled(bEnabled);
        }
    } else {
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            creatorObjectId,
            functionId,
            [weak = weak_from_this(), lightSceneProxyIndex, bEnabled](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    const auto& lightSp = sceneRenderer->GetLightProxyByProxyId(lightSceneProxyIndex);
                    if (lightSp) {
                        lightSp->SetEnabled(bEnabled);
                    }
                }
            });
    }
}

void SceneRenderer::UpdatePrimitiveComponentVisibility_OnRenderThread(
    const int32_t primitiveSceneProxyIndex, const int32_t creatorObjectId, const uint64_t functionId, const bool visibility)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        const auto& primitiveSp = GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
        if (primitiveSp) {
            primitiveSp->SetVisibility(visibility);
        }
    } else {
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            creatorObjectId,
            functionId,
            [weak = weak_from_this(), primitiveSceneProxyIndex, visibility](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    const auto& primitiveSp = sceneRenderer->GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
                    if (primitiveSp) {
                        primitiveSp->SetVisibility(visibility);
                    }
                }
            });
    }
}

void SceneRenderer::UpdateLightComponentIsVisible_OnRenderThread(
    const int32_t lightSceneProxyIndex, const int32_t creatorObjectId, const uint64_t functionId, const bool visibility)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        const auto& lightSp = GetLightProxyByProxyId(lightSceneProxyIndex);
        if (lightSp) {
            lightSp->SetIsVisible(visibility);
        }
    } else {
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            creatorObjectId,
            functionId,
            [weak = weak_from_this(), lightSceneProxyIndex, visibility](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    const auto& lightSp = sceneRenderer->GetLightProxyByProxyId(lightSceneProxyIndex);
                    if (lightSp) {
                        lightSp->SetIsVisible(visibility);
                    }
                }
            });
    }
}

void SceneRenderer::UpdatePrimitiveComponentSortOrderValue_OnRenderThread(
    const int32_t primitiveSceneProxyIndex,
    const int32_t creatorObjectId,
    const uint64_t functionId,
    const int32_t sortOrderValue)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        const auto& primitiveSp = GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
        if (primitiveSp) {
            primitiveSp->SetSortOrderValue(sortOrderValue);
            // Only the forward provider orders by sort order value (SortPrimitivesByOrderAndShader).
            if (!primitiveSp->IsDeferred()) {
                SetForwardPrimitivesDirty(true);
            }
        }
    } else {
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            creatorObjectId,
            functionId,
            [weak = weak_from_this(), sortOrderValue, primitiveSceneProxyIndex](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    const auto& primitiveSp = sceneRenderer->GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
                    if (primitiveSp) {
                        primitiveSp->SetSortOrderValue(sortOrderValue);
                        if (!primitiveSp->IsDeferred()) {
                            sceneRenderer->SetForwardPrimitivesDirty(true);
                        }
                    }
                }
            });
    }
}

void SceneRenderer::UpdatePrimitiveComponentTransform_OnRenderThread(
    const int32_t primitiveSceneProxyIndex,
    const int32_t creatorObjectId,
    const uint64_t functionId,
    const glm::mat4& newworldMatrix,
    const glm::mat4& newOutlineMatrix,
    const BoundingBox3D& newTransformedBoundingBox)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        const auto& primitiveSp = GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
        if (primitiveSp) {
            primitiveSp->SetWorldMatrix(newworldMatrix);
            primitiveSp->SetOutlineMatrix(newOutlineMatrix);
            primitiveSp->SetTransformedBoundingBox(newTransformedBoundingBox);
            primitiveSp->IsDeferred() ? SetDeferredPrimitivesDirty(true) : SetForwardPrimitivesDirty(true);
        }
    } else {
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            creatorObjectId,
            functionId,
            [weak = weak_from_this(), primitiveSceneProxyIndex, newworldMatrix, newOutlineMatrix, newTransformedBoundingBox](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    const auto& primitiveSp = sceneRenderer->GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
                    if (primitiveSp) {
                        primitiveSp->SetWorldMatrix(newworldMatrix);
                        primitiveSp->SetOutlineMatrix(newOutlineMatrix);
                        primitiveSp->SetTransformedBoundingBox(newTransformedBoundingBox);
                        primitiveSp->IsDeferred() ? sceneRenderer->SetDeferredPrimitivesDirty(true)
                                                  : sceneRenderer->SetForwardPrimitivesDirty(true);
                    }
                }
            });
    }
}

void SceneRenderer::UpdateLightComponentTransform_OnRenderThread(
    const int32_t lightSceneProxyIndex, const int32_t creatorObjectId, const uint64_t functionId, const glm::mat4& newworldMatrix)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        const auto& lightSp = GetLightProxyByProxyId(lightSceneProxyIndex);
        if (lightSp) {
            lightSp->SetWorldMatrix(newworldMatrix);
            SetLightProxiesTransformDirty(true);
        }
    } else {
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            creatorObjectId,
            functionId,
            [weak = weak_from_this(), newworldMatrix, lightSceneProxyIndex](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    const auto& lightProxySp = sceneRenderer->GetLightProxyByProxyId(lightSceneProxyIndex);
                    if (lightProxySp) {
                        lightProxySp->SetWorldMatrix(newworldMatrix);
                        sceneRenderer->SetLightProxiesTransformDirty(true);
                    }
                }
            });
    }
}

void SceneRenderer::RemovePrimitiveSceneProxy_OnRenderThread(const int32_t primitiveSceneProxyIndex)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        RemovePrimitiveProxyByProxyId(primitiveSceneProxyIndex);
        SetPrimitiveProxiesDirty(true);
    } else {
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            primitiveSceneProxyIndex,
            Hash64_CT("SceneRenderer::RemovePrimitiveSceneProxy_OnRenderThread"),
            [weak = weak_from_this(), primitiveSceneProxyIndex](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    sceneRenderer->RemovePrimitiveProxyByProxyId(primitiveSceneProxyIndex);
                    sceneRenderer->SetPrimitiveProxiesDirty(true);
                }
            });
    }
}

void SceneRenderer::DeleteLightSceneProxy_OnRenderThread(const int32_t lightSceneProxyIndex)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        RemoveLightProxyByProxyId(lightSceneProxyIndex);
        SetLightProxiesDirty(true);
    } else {
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            0,
            Hash64_CT("SceneRenderer::DeleteLightSceneProxy_OnRenderThread"),
            [weak = weak_from_this(), lightSceneProxyIndex](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    sceneRenderer->RemoveLightProxyByProxyId(lightSceneProxyIndex);
                    sceneRenderer->SetLightProxiesDirty(true);
                }
            });
    }
}

void SceneRenderer::UpdateLightSceneProxies_OnRenderThread()
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        SetLightProxiesDirty(true);
    } else {
        constexpr int32_t creatorObjectId = 0;
        const uint64_t functionId = Hash64_CT("SceneRenderer::UpdateLightSceneProxies_OnRenderThread");
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            creatorObjectId,
            functionId,
            [weak = weak_from_this()](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    sceneRenderer->SetLightProxiesDirty(true);
                }
            });
    }
}

void SceneRenderer::AddCameraSceneProxy_OnRenderThread(
    const std::shared_ptr<ACamera>& camera, const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        SceneViewsVector.emplace_back(std::make_shared<SceneView>(cameraSceneProxy));
        camera->SetIsCameraProxyReady(true);
    } else {
        LogInfo("SceneRenderer::AddCameraSceneProxy_OnRenderThread: camera proxyId: ", cameraSceneProxy->GetSceneProxyId());
        const uint64_t functionId = Hash64_CT("SceneRenderer::AddCameraSceneProxy_OnRenderThread");

        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            cameraSceneProxy->GetSceneProxyId(),
            functionId,
            [weak = weak_from_this(), camera, cameraSceneProxy](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    sceneRenderer->SceneViewsVector.emplace_back(std::make_shared<SceneView>(cameraSceneProxy));
                    camera->SetIsCameraProxyReady(true);
                }
            });
    }
}

void SceneRenderer::RemoveCameraSceneProxy_OnRenderThread(const int32_t cameraSceneProxyId)
{
    LogInfo("SceneRenderer::RemoveCameraSceneProxy_OnRenderThread: camera proxyId: ", cameraSceneProxyId);
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        SceneViewsVector.erase(
            std::remove_if(
                SceneViewsVector.begin(),
                SceneViewsVector.end(),
                [cameraSceneProxyId](const auto& sceneViewSp) {
                    return sceneViewSp->GetCameraProxy()->GetSceneProxyId() == cameraSceneProxyId;
                }),
            SceneViewsVector.end());
    } else {
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            0,
            Hash64_CT("SceneRenderer::RemoveCameraSceneProxy_OnRenderThread"),
            [weak = weak_from_this(), cameraSceneProxyId](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    sceneRenderer->SceneViewsVector.erase(
                        std::remove_if(
                            sceneRenderer->SceneViewsVector.begin(),
                            sceneRenderer->SceneViewsVector.end(),
                            [cameraSceneProxyId](const auto& sceneViewSp) {
                                return sceneViewSp->GetCameraProxy()->GetSceneProxyId() == cameraSceneProxyId;
                            }),
                        sceneRenderer->SceneViewsVector.end());
                }
            });
    }
}

void SceneRenderer::AddPrimitiveSceneProxy_OnRenderThread(
    const std::shared_ptr<PrimitiveComponent>& primitiveComponent,
    const std::shared_ptr<PrimitiveSceneProxy>& primitiveSceneProxy)
{
    constexpr int32_t creatorObjectId = 0;
    static const uint64_t functionId = Hash64_CT("SceneRenderer::AddPrimitiveSceneProxy_OnRenderThread");

    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        ext_assert(
            !GetPrimitiveProxyByProxyId(primitiveSceneProxy->GetSceneProxyId()),
            "Primitive proxy with id {} already exists" + std::to_string(primitiveSceneProxy->GetSceneProxyId()));
        primitiveSceneProxy->PostConstructorInitialize();
        PrimitiveProxiesVector.emplace_back(primitiveSceneProxy);
        SetPrimitiveProxiesDirty(true);
        primitiveComponent->SetIsSceneProxyReady(true);
    } else {
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            creatorObjectId,
            functionId,
            [weak = weak_from_this(), primitiveComponent, primitiveSceneProxy](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    ext_assert(
                        !sceneRenderer->GetPrimitiveProxyByProxyId(primitiveSceneProxy->GetSceneProxyId()),
                        "Primitive proxy with id {} already exists" + std::to_string(primitiveSceneProxy->GetSceneProxyId()));
                    primitiveSceneProxy->PostConstructorInitialize();
                    sceneRenderer->GetPrimitiveProxies().emplace_back(primitiveSceneProxy);
                    sceneRenderer->SetPrimitiveProxiesDirty(true);
                    primitiveComponent->SetIsSceneProxyReady(true);
                }
            });
    }
}

void SceneRenderer::AddLightSceneProxy_OnRenderThread(
    const std::shared_ptr<LightComponent>& lightComponent, const std::shared_ptr<LightSceneProxy>& lightSceneProxy)
{
    constexpr int32_t creatorObjectId = 0;
    static const uint64_t functionId = Hash64_CT("SceneRenderer::AddLightSceneProxy_OnRenderThread");

    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        ext_assert(
            !GetLightProxyByProxyId(lightSceneProxy->GetSceneProxyId()),
            "Light proxy with id {} already exists" + std::to_string(lightSceneProxy->GetSceneProxyId()));
        LightProxiesVector.emplace_back(lightSceneProxy);
        SetLightProxiesDirty(true);
        lightComponent->SetIsSceneProxyReady(true);
        lightSceneProxy->PostInitialize();
    } else {
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            creatorObjectId,
            functionId,
            [weak = weak_from_this(), lightComponent, lightSceneProxy](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    ext_assert(
                        !sceneRenderer->GetLightProxyByProxyId(lightSceneProxy->GetSceneProxyId()),
                        "Light proxy with id {} already exists" + std::to_string(lightSceneProxy->GetSceneProxyId()));
                    sceneRenderer->LightProxiesVector.emplace_back(lightSceneProxy);
                    sceneRenderer->SetLightProxiesDirty(true);
                    lightComponent->SetIsSceneProxyReady(true);
                    lightSceneProxy->PostInitialize();
                }
            });
    }
}

void SceneRenderer::RegisterUiCanvasProxy_OnRenderThread(
    const std::shared_ptr<::EngineCore::GUI::UiCanvas>& uiCanvas, const std::shared_ptr<UiCanvasSceneProxy>& uiCanvasProxy)
{
    LogInfo("SceneRenderer::RegisterUiCanvasProxy_OnRenderThread: UId = ", uiCanvasProxy->GetUiItemUId());

    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        RegisterUiCanvasProxy(uiCanvasProxy);
        uiCanvas->SetIsSceneProxyReady(true);
    } else {
        constexpr int32_t creatorObjectId = 0;
        constexpr uint64_t functionId = Hash64_CT("SceneRenderer::RegisterUiCanvasProxy_OnRenderThread");

        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            creatorObjectId,
            functionId,
            [weak = weak_from_this(), uiCanvas, uiCanvasProxy](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    sceneRenderer->RegisterUiCanvasProxy(uiCanvasProxy);
                    uiCanvas->SetIsSceneProxyReady(true);
                }
            });
    }
}

void SceneRenderer::UnregisterUiCanvasProxy_OnRenderThread(const size_t canvasUiId)
{
    LogInfo("SceneRenderer::UnregisterUiCanvasProxy_OnRenderThread: UId = ", canvasUiId);

    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        UnregisterUiCanvasProxy(canvasUiId);
    } else {
        constexpr int32_t creatorObjectId = 0;
        constexpr uint64_t functionId = Hash64_CT("SceneRenderer::UnregisterUiCanvasProxy_OnRenderThread");

        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            creatorObjectId,
            functionId,
            [weak = weak_from_this(), canvasUiId](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    sceneRenderer->UnregisterUiCanvasProxy(canvasUiId);
                }
            });
    }
}

void SceneRenderer::RegisterUiSceneProxy_OnRenderThread(
    const std::shared_ptr<UiItemBase>& uiItem, const std::shared_ptr<UiSceneProxyBase>& uiSceneProxy, const size_t canvasUId)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        RegisterUiSceneProxy(uiSceneProxy, canvasUId);
        uiItem->SetIsSceneProxyReady(true);
    } else {
        constexpr int32_t creatorObjectId = 0;
        constexpr uint64_t functionId = Hash64_CT("SceneRenderer::RegisterUiSceneProxy_OnRenderThread");

        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            creatorObjectId,
            functionId,
            [weak = weak_from_this(), uiItem, uiSceneProxy, canvasUId](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    sceneRenderer->RegisterUiSceneProxy(uiSceneProxy, canvasUId);
                    uiItem->SetIsSceneProxyReady(true);
                }
            });
    }
}

void SceneRenderer::UnregisterUiSceneProxy_OnRenderThread(const size_t uiItemUId, const size_t canvasUId)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        UnregisterUiSceneProxy(uiItemUId, canvasUId);
    } else {
        constexpr int32_t creatorObjectId = 0;
        constexpr uint64_t functionId = Hash64_CT("SceneRenderer::UnregisterUiSceneProxy_OnRenderThread");

        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            creatorObjectId,
            functionId,
            [weak = weak_from_this(), uiItemUId, canvasUId](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    sceneRenderer->UnregisterUiSceneProxy(uiItemUId, canvasUId);
                }
            });
    }
}

void SceneRenderer::MaterialPropertiesUpdated_OnRenderThread(
    const int32_t materialProxyIndex, std::vector<std::shared_ptr<MaterialProperty>>&& properties)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        const auto& materialProxySp = GetMaterialProxyByProxyId(materialProxyIndex);
        if (materialProxySp) {
            materialProxySp->UpdateProperties(std::move(properties));
        } else {
            LogInfo(
                "SceneRenderer::MaterialPropertiesUpdated_OnRenderThread: "
                "Warning! Current proxy index doesn't exist on RT. Proxy index = ",
                materialProxyIndex);
        }
    } else {
        constexpr uint64_t functionId = Hash64_CT("SceneRenderer::MaterialPropertiesUpdated_OnRenderThread");
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            0,
            functionId,
            [weak = weak_from_this(), materialProxyIndex, properties = std::move(properties)](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) mutable {
                if (const auto& sceneRenderer = weak.lock()) {
                    const auto& materialProxySp = sceneRenderer->GetMaterialProxyByProxyId(materialProxyIndex);
                    if (materialProxySp) {
                        materialProxySp->UpdateProperties(std::move(properties));
                    } else {
                        LogInfo(
                            "SceneRenderer::MaterialPropertiesUpdated_OnRenderThread: "
                            "Warning! Current proxy index doesn't exist on RT. Proxy index = ",
                            materialProxyIndex);
                    }
                }
            });
    }
}

void SceneRenderer::AddPlanarReflectionSceneProxy_OnRenderThread(
    const std::shared_ptr<PlanarReflectionComponent>& planarReflectionComponent,
    const std::shared_ptr<PlanarReflectionProxy>& proxy)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        const auto& reflectionProxySp = GetPlanarReflectionProxyByProxyId(proxy->GetSceneProxyId());
        ext_assert(
            !reflectionProxySp, "Planar reflection proxy with id {} already exists" + std::to_string(proxy->GetSceneProxyId()));
        PlanarReflectionProxiesVector.emplace_back(proxy);
        SetPlanarReflectionProxiesDirty(true);
        planarReflectionComponent->SetIsSceneProxyReady(true);
    } else {
        constexpr uint64_t functionId = Hash64_CT("SceneRenderer::AddPlanarReflectionSceneProxy_OnRenderThread");
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            proxy->GetSceneProxyId(),
            functionId,
            [proxy, planarReflectionComponent, weak = weak_from_this()](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    const auto& reflectionProxySp = sceneRenderer->GetPlanarReflectionProxyByProxyId(proxy->GetSceneProxyId());
                    ext_assert(
                        !reflectionProxySp,
                        "Planar reflection proxy with id {} already exists" + std::to_string(proxy->GetSceneProxyId()));
                    sceneRenderer->PlanarReflectionProxiesVector.emplace_back(proxy);
                    sceneRenderer->SetPlanarReflectionProxiesDirty(true);
                    planarReflectionComponent->SetIsSceneProxyReady(true);
                }
            });
    }
}

void SceneRenderer::RemovePlanarReflectionSceneProxy_OnRenderThread(const int32_t planarReflectionProxyIndex)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        RemovePlanarReflectionSceneProxyByProxyId(planarReflectionProxyIndex);
        SetPlanarReflectionProxiesDirty(true);
    } else {
        constexpr uint64_t functionId = Hash64_CT("SceneRenderer::RemovePlanarReflectionSceneProxy_OnRenderThread");
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            planarReflectionProxyIndex,
            functionId,
            [weak = weak_from_this(), planarReflectionProxyIndex](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    sceneRenderer->RemovePlanarReflectionSceneProxyByProxyId(planarReflectionProxyIndex);
                    sceneRenderer->SetPlanarReflectionProxiesDirty(true);
                }
            });
    }
}

void SceneRenderer::BindPlanarReflectionSceneProxyToSceneView_OnRenderThread(
    const std::shared_ptr<PlanarReflectionProxy>& planarReflectionProxy, const int32_t cameraSceneProxyId)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        const auto& sceneViewSp = GetSceneViewByCameraProxyId(cameraSceneProxyId);
        if (sceneViewSp) {
            planarReflectionProxy->SetSceneViewWeakPtr(sceneViewSp);
        } else {
            LogInfo(
                "SceneRenderer::BindPlanarReflectionSceneProxyToSceneView_OnRenderThread: "
                "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                cameraSceneProxyId);
        }
    } else {
        constexpr uint64_t functionId = Hash64_CT("SceneRenderer::BindPlanarReflectionSceneProxyToSceneView_OnRenderThread");
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            cameraSceneProxyId,
            functionId,
            [weak = weak_from_this(), cameraSceneProxyId, planarReflectionProxy](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    const auto& sceneViewSp = sceneRenderer->GetSceneViewByCameraProxyId(cameraSceneProxyId);
                    if (sceneViewSp) {
                        planarReflectionProxy->SetSceneViewWeakPtr(sceneViewSp);
                    } else {
                        LogInfo(
                            "SceneRenderer::BindPlanarReflectionSceneProxyToSceneView_OnRenderThread: "
                            "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                            cameraSceneProxyId);
                    }
                }
            });
    }
}

void SceneRenderer::UpdateMeshModelPath_OnRenderThread(
    const int32_t primitiveSceneProxyIndex, const ePrimitiveProxyType primitiveProxyType, const std::string& newModelPath)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        const auto& primitiveProxy = GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
        if (primitiveProxy) {
            if (primitiveProxyType == ePrimitiveProxyType::STATIC_MESH_PROXY) {
                const auto staticMeshProxy = std::static_pointer_cast<StaticMeshSceneProxy>(primitiveProxy);
                staticMeshProxy->SetMeshModelPath(newModelPath);
            } else if (primitiveProxyType == ePrimitiveProxyType::SKELETAL_MESH_PROXY) {
                const auto skeletalMeshProxy = std::static_pointer_cast<SkeletalMeshSceneProxy>(primitiveProxy);
                skeletalMeshProxy->SetMeshModelPath(newModelPath);
            } else {
                LogInfo(
                    "SceneRenderer::UpdateMeshModelPath_OnRenderThread: "
                    "Error! Current proxy is not a mesh proxy. Proxy index = ",
                    primitiveSceneProxyIndex,
                    "proxyType = ",
                    static_cast<int32_t>(primitiveProxyType));
            }
        } else {
            LogInfo(
                "SceneRenderer::UpdateMeshModelPath_OnRenderThread: "
                "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                primitiveSceneProxyIndex);
        }
    } else {
        constexpr uint64_t functionId = Hash64_CT("SceneRenderer::UpdateMeshModelPath_OnRenderThread");
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            primitiveSceneProxyIndex,
            functionId,
            [weak = weak_from_this(), primitiveSceneProxyIndex, newModelPath, primitiveProxyType](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = sceneRendererWp.lock()) {
                    const auto& primitiveProxy = sceneRenderer->GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
                    if (primitiveProxy) {
                        if (primitiveProxyType == ePrimitiveProxyType::STATIC_MESH_PROXY) {
                            const auto staticMeshProxy = std::static_pointer_cast<StaticMeshSceneProxy>(primitiveProxy);
                            staticMeshProxy->SetMeshModelPath(newModelPath);
                        } else if (primitiveProxyType == ePrimitiveProxyType::SKELETAL_MESH_PROXY) {
                            const auto skeletalMeshProxy = std::static_pointer_cast<SkeletalMeshSceneProxy>(primitiveProxy);
                            skeletalMeshProxy->SetMeshModelPath(newModelPath);
                        } else {
                            LogInfo(
                                "SceneRenderer::UpdateMeshModelPath_OnRenderThread: "
                                "Error! Current proxy is not a mesh proxy. Proxy index = ",
                                primitiveSceneProxyIndex,
                                "proxyType = ",
                                static_cast<int32_t>(primitiveProxyType));
                        }
                    } else {
                        LogInfo(
                            "SceneRenderer::UpdateMeshModelPath_OnRenderThread: "
                            "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                            primitiveSceneProxyIndex);
                    }
                }
            });
    }
}

void SceneRenderer::ResetPrimitivesFrustumTestResultForCamera(const int32_t cameraProxyId)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        const auto& sceneView = GetSceneViewByCameraProxyId(cameraProxyId);
        if (sceneView) {
            sceneView->ResetVisibility();
        } else {
            LogInfo(
                "SceneRenderer::ResetPrimitivesFrustumTestResultForCamera: "
                "Error! Current camera proxy index doesn't exist on RT. Proxy index = ",
                cameraProxyId);
        }
    } else {
        constexpr uint64_t functionId = Hash64_CT("SceneRenderer::ResetPrimitivesFrustumTestResultForCamera");
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            cameraProxyId,
            functionId,
            [weak = weak_from_this(), cameraProxyId](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = sceneRendererWp.lock()) {
                    const auto& sceneView = sceneRenderer->GetSceneViewByCameraProxyId(cameraProxyId);
                    if (sceneView) {
                        sceneView->ResetVisibility();
                    } else {
                        LogInfo(
                            "SceneRenderer::ResetPrimitivesFrustumTestResultForCamera: "
                            "Error! Current camera proxy index doesn't exist on RT. Proxy index = ",
                            cameraProxyId);
                    }
                }
            });
    }
}

void SceneRenderer::ResetPrimitiveFrustumTestResult(const int32_t primitiveProxyId)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName)) {
        for (const auto& sceneView : SceneViewsVector) {
            sceneView->ResetVisibilityForPrimitive(primitiveProxyId);
        }
    } else {
        constexpr uint64_t functionId = Hash64_CT("SceneRenderer::ResetPrimitiveFrustumTestResult");
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            primitiveProxyId,
            functionId,
            [weak = weak_from_this(), primitiveProxyId](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = sceneRendererWp.lock()) {
                    for (const auto& sceneView : sceneRenderer->SceneViewsVector) {
                        sceneView->ResetVisibilityForPrimitive(primitiveProxyId);
                    }
                }
            });
    }
}

void SceneRenderer::RegisterText(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy)
{
    mFreeTypeFontHandler->RegisterText(textFieldProxy);
}

void SceneRenderer::UnregisterText(const int32_t textFieldProxyId)
{
    mFreeTypeFontHandler->UnregisterText(textFieldProxyId);
}

void SceneRenderer::TextPositionChanged(const int32_t textFieldProxyId, const glm::vec2& position)
{
    mFreeTypeFontHandler->TextPositionChanged(textFieldProxyId, position);
}

void SceneRenderer::TextColorChanged(const int32_t textFieldProxyId, const glm::vec3& color)
{
    mFreeTypeFontHandler->TextColorChanged(textFieldProxyId, color);
}

void SceneRenderer::TextVisibilityChanged(const int32_t textFieldProxyId, const bool bIsVisible)
{
    mFreeTypeFontHandler->TextVisibilityChanged(textFieldProxyId, bIsVisible);
}

void SceneRenderer::RegisterUiCanvasProxy(const std::shared_ptr<UiCanvasSceneProxy>& canvasSceneProxy)
{
    ext_assert(canvasSceneProxy, "SceneRenderer::RegisterUiCanvasProxy: canvasSceneProxy is nullptr");
    auto canvasIt = std::find_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [&](const auto& canvasProxy) {
        return canvasSceneProxy->GetUiItemUId() == canvasProxy->GetUiItemUId();
    });
    ext_assert(
        canvasIt == mUiCanvasProxies.end(),
        "SceneRenderer::RegisterUiCanvasProxy: canvasSceneProxy with id {} already exists"
            + std::to_string(canvasSceneProxy->GetUiItemUId()));
    mUiCanvasProxies.emplace_back(canvasSceneProxy);
    canvasSceneProxy->SetFontHandler(mFreeTypeFontHandler);
}

void SceneRenderer::UnregisterUiCanvasProxy(const size_t canvasUiId)
{
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName),
        "SceneRenderer::UnregisterUiCanvasProxy: This function must be called from Render thread");
    mUiCanvasProxies.erase(
        std::remove_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [canvasUiId](const auto& canvasProxy) {
            return canvasUiId == canvasProxy->GetUiItemUId();
        }));
}

void SceneRenderer::RegisterUiSceneProxy(const std::shared_ptr<UiSceneProxyBase>& sceneProxy, const size_t canvasUId)
{
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName),
        "SceneRenderer::RegisterUiSceneProxy: This function must be called from Render thread");
    ext_assert(sceneProxy, "SceneRenderer::RegisterUiSceneProxy: sceneProxy is nullptr");
    auto canvasIt = std::find_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [=](const auto& canvasProxy) {
        return canvasUId == canvasProxy->GetUiItemUId();
    });
    ext_assert(
        canvasIt != mUiCanvasProxies.end(),
        "SceneRenderer::RegisterUiSceneProxy: canvas with id {} not found" + std::to_string(canvasUId));
    sceneProxy->SetCanvasSceneProxy((*canvasIt));
    (*canvasIt)->AddUiSceneProxy(sceneProxy);
    sceneProxy->SetSceneRenderer(shared_from_this());
    sceneProxy->OnSceneProxyRegistered();
}

void SceneRenderer::UnregisterUiSceneProxy(const size_t uiItemUId, const size_t canvasUId)
{
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName),
        "SceneRenderer::UnregisterUiSceneProxy: This function must be called from Render thread");
    auto canvasIt = std::find_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [=](const auto& canvasProxy) {
        return canvasUId == canvasProxy->GetUiItemUId();
    });
    ext_assert(
        canvasIt != mUiCanvasProxies.end(),
        "SceneRenderer::UnregisterUiSceneProxy: canvas with id {} not found" + std::to_string(canvasUId));
    (*canvasIt)->RemoveUiSceneProxy(uiItemUId);
}

#if DEBUG

void SceneRenderer::SetDebugPhysicsRenderData(const DebugPhysicsRenderData& debugPhysicsRenderData)
{
    mDebugPhysicsRenderData = debugPhysicsRenderData;
}

void SceneRenderer::DebugRenderPhysics(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    float viewMatVec[16]{0.0f};
    const float* pSource = (const float*)glm::value_ptr(viewMatrix);
    for (int i = 0; i < 16; ++i)
        viewMatVec[i] = pSource[i];

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(pSource);

    float projMatrix[16]{0.0f};
    pSource = (const float*)glm::value_ptr(projectionMatrix);
    for (int i = 0; i < 16; ++i)
        projMatrix[i] = pSource[i];

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projMatrix);

    RenderState renderState;
    renderState.GetBlendingState().SetIsBlendingEnabled(false).SetBlendingFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);

    renderState.GetStencilState()
        .SetIsStencilTestEnabled(false)
        .SetStencilOperation(0, 0, 0)
        .SetStencilFunction(GL_NOTEQUAL, EngineConstants::eStencilValues::SCENE_DEFAULT, 0xFF)
        .SetStencilMask(0);
    renderState.BindRenderState();
    // todo: delete this crap and use buffers =\

    const auto& physicsRenderData = mDebugPhysicsRenderData.GetDebugLines();
    if (physicsRenderData.size()) {
        glBegin(GL_LINES);
        for (size_t i = 0; i < physicsRenderData.size(); ++i) {
            glm::vec3 vert1 = physicsRenderData[i].first;
            glm::vec3 vert2 = physicsRenderData[i].second;

            glColor3f(1, 0, 0);
            glVertex3f(vert1.x, vert1.y, vert1.z);
            glVertex3f(vert2.x, vert2.y, vert2.z);
        }
        glEnd();
    }

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
#endif
#if 0

         for (auto &proxy : PrimitiveProxiesVector)
         {
            if (proxy->GetPrimitiveProxyType() == ePrimitiveProxyType::SKELETAL_MESH_PROXY || proxy->GetPrimitiveProxyType() == ePrimitiveProxyType::STATIC_MESH_PROXY)
            {
               const auto bb = proxy->GetTransformedBoundingBox();
               const auto &positions = bb.GetBoundPositions();

               std::vector<glm::vec3> points =
                   {
                       // Front
                       positions[0],
                       positions[1],
                       positions[2],
                       positions[2],
                       positions[0],
                       positions[3],

                       // Back
                       positions[4],
                       positions[5],
                       positions[6],
                       positions[6],
                       positions[7],
                       positions[4],

                       // Right
                       positions[3],
                       positions[2],
                       positions[6],
                       positions[6],
                       positions[7],
                       positions[3],

                       // Left
                       positions[0],
                       positions[1],
                       positions[5],
                       positions[5],
                       positions[0],
                       positions[4],

                       // Top
                       positions[4],
                       positions[0],
                       positions[3],
                       positions[3],
                       positions[7],
                       positions[4],

                       // Bottom
                       positions[5],
                       positions[1],
                       positions[2],
                       positions[2],
                       positions[6],
                       positions[5],
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
#endif

} // namespace Graphics::Renderer