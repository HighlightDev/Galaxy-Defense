#include "SceneRenderer.h"

#include "Core/CommonCore/EngineConstants.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/LightComponent.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/GUI/Common/TextFieldProxyType.h"
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

#include <gl/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <limits>

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

namespace Graphics {
namespace Renderer {
SceneRenderer::SceneRenderer(InterThreadCommunicationMgr& interThreadMgr)
    : m_interThreadMgr(interThreadMgr)
    , m_gbuffer(std::make_unique<DeferredShadingGBuffer>(ViewPortInfo(
          0,
          0,
          GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth(),
          GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight())))
    , m_resolvedSceneFramebuffer(std::make_shared<ResolvedSceneFramebuffer>(ViewPortInfo(
          0,
          0,
          GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth(),
          GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight())))
    , m_deferredLightShader()
    , m_fontShader()
    , mDepthCollectShaderSkeletal()
    , mDepthCollectShaderNonSkeletal()
    , mDepthCollectPointLightShaderSkeletal()
    , mDepthCollectPointLightShaderNonSkeletal()
    , bProxiesDirty(false)
    , bLightProxiesDirty(false)
    , bPlanarReflectionProxiesDirty(false)
    , mActiveBindedState()
    , mPostFxRenderer(std::make_unique<PostFxRenderer>(ViewPortInfo(
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
    , mForwardRenderingProxiesVec()
    , mSkeletalProxiesVec()
    , mNonSkeletalProxiesVec()
    , mDirLightProxiesVec()
    , mPointLightProxiesVec()
    , mSpotlightProxiesVec()
    , mPlanarReflectionProxiesVec()
    , mGroupedByShadowAtlasLights()
    , mInstancedGeometryBatchRenderer(std::make_shared<InstancedGeometryBatchRenderer>())
{
    LogInfo("SceneRenderer::ctor");
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
    const auto shadersPathStr = FolderManager::GetInstance()->GetShadersPath();

    const ShaderParams depthCollectShaderParams(
        "DepthCollectShader",
        shadersPathStr + "composite_shaders" + SLASH + "depthCollectVS.glsl",
        shadersPathStr + "composite_shaders" + SLASH + "depthCollectFS.glsl");
    const ShaderParams plDepthCollectShaderParams(
        "PointLightDepthCollectShader",
        shadersPathStr + "composite_shaders" + SLASH + "depthCollectPointLightVS.glsl",
        shadersPathStr + "composite_shaders" + SLASH + "depthCollectPointLightFS.glsl",
        shadersPathStr + "composite_shaders" + SLASH + "depthCollectPointLightGS.glsl");
    const CompositeShaderParams staticMeshParams("StaticMeshVertexFactory", depthCollectShaderParams);
    const CompositeShaderParams skeletalMeshParams("SkeletalMeshVertexFactory<4>", depthCollectShaderParams);
    const CompositeShaderParams staticMeshCompositeParams("StaticMeshVertexFactory", plDepthCollectShaderParams);
    const CompositeShaderParams skeletalMeshCompositeParams("SkeletalMeshVertexFactory<4>", plDepthCollectShaderParams);
    const ShaderParams deferredLightShaderParams(
        "DeferredLight Shader", shadersPathStr + "deferredLightPassVS.glsl", shadersPathStr + "deferredLightPassFS.glsl");
    const ShaderParams fontRenderingShaderParams(
        "FontRendering Shader", shadersPathStr + "fontVS.glsl", shadersPathStr + "fontFS.glsl");

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
    m_fontShader = std::make_shared<FontRenderingShader>(fontRenderingShaderParams);
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
    if (m_fontShader) {
        m_fontShader->CleanUp(true);
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

    mForwardRenderingProxiesVec.clear();
    mSkeletalProxiesVec.clear();
    mNonSkeletalProxiesVec.clear();
    mDirLightProxiesVec.clear();
    mPointLightProxiesVec.clear();
    mSpotlightProxiesVec.clear();
    mPlanarReflectionProxiesVec.clear();
    mGroupedByShadowAtlasLights.clear();

    bProxiesDirty = false;
    bLightProxiesDirty = false;
    bPlanarReflectionProxiesDirty = false;

    mDepthCollectShaderNonSkeletal->CleanUp(true);
    mDepthCollectShaderSkeletal->CleanUp(true);
    mDepthCollectPointLightShaderSkeletal->CleanUp(true);
    mDepthCollectPointLightShaderNonSkeletal->CleanUp(true);
    m_deferredLightShader->CleanUp(true);
    m_fontShader->CleanUp(true);

    mDepthCollectShaderNonSkeletal = nullptr;
    mDepthCollectShaderSkeletal = nullptr;
    mDepthCollectPointLightShaderSkeletal = nullptr;
    mDepthCollectPointLightShaderNonSkeletal = nullptr;
    m_deferredLightShader = nullptr;
    m_fontShader = nullptr;
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
    mPostFxRenderer->ResizeRenderTargets(viewPortInfo);
}

void SceneRenderer::DepthPass(const std::shared_ptr<SceneView>& sceneView)
{
    if (mGroupedByShadowAtlasLights.size()) {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        RenderState renderState;
        renderState.GetBlendingState().SetIsBlendingEnabled(false);

        renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);

        renderState.GetStencilState()
            .SetIsStencilTestEnabled(false)
            .SetStencilOperation(0, 0, 0)
            .SetStencilFunction(GL_NOTEQUAL, EngineConstants::eStencilValues::DEFAULT, 0xFF)
            .SetStencilMask(0x00);

        renderState.BindRenderState();

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

                            if (mNonSkeletalProxiesVec.size() > 0) // Non - skeletal proxies
                            {
                                mDepthCollectShaderNonSkeletal->ExecuteShader();
                                mDepthCollectShaderNonSkeletal->GetShader()->SetWriteDepthLinearly(false);
                                for (auto& proxy : mNonSkeletalProxiesVec) {
                                    if (proxy->IsEnabled() && proxy->IsVisible()
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

                            if (mSkeletalProxiesVec.size() > 0) // Skeletal proxies
                            {
                                mDepthCollectShaderSkeletal->ExecuteShader();
                                for (auto& proxy : mSkeletalProxiesVec) {
                                    const bool bShouldRender = proxy->IsEnabled() && proxy->IsVisible()
                                        && dirLightShadowOrthoBound.IsIntersectionWithBox(proxy->GetTransformedBoundingBox());
                                    if (bShouldRender) {
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

                            if (mNonSkeletalProxiesVec.size() > 0) // Non - skeletal proxies
                            {
                                mDepthCollectShaderNonSkeletal->ExecuteShader();
                                mDepthCollectShaderNonSkeletal->GetShader()->SetWriteDepthLinearly(true);
                                for (auto& proxy : mNonSkeletalProxiesVec) {
                                    const bool bShouldRender = proxy->IsTransformIntialized() && proxy->IsEnabled()
                                        && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
                                    if (bShouldRender) {
                                        const auto& worldMatrix = proxy->GetMatrix();
                                        const auto& viewMatrix = shadowInfo->GetShadowViewMatrix();
                                        const auto& projectionMatrix = shadowInfo->GetShadowProjectionMatrix();

                                        mDepthCollectShaderNonSkeletal->GetVertexFactoryShader()->SetMatrices(
                                            worldMatrix, viewMatrix, projectionMatrix);
                                        mDepthCollectShaderNonSkeletal->GetShader()->SetShadowDistance(
                                            spotlightPtr->GetRadianceRadius());
                                        mDepthCollectShaderNonSkeletal->GetShader()->SetLightWorldPosition(
                                            spotlightPtr->GetPosition());

                                        proxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                                    }
                                }
                                mDepthCollectShaderNonSkeletal->StopShader();
                            }
                            if (mSkeletalProxiesVec.size() > 0) // Skeletal proxies
                            {
                                mDepthCollectShaderSkeletal->ExecuteShader();
                                mDepthCollectShaderSkeletal->GetShader()->SetWriteDepthLinearly(true);

                                for (auto& proxy : mSkeletalProxiesVec) {
                                    const bool bShouldRender = proxy->IsTransformIntialized() && proxy->IsEnabled()
                                        && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());

                                    if (bShouldRender) {
                                        const auto skeletalProxy = std::static_pointer_cast<SkeletalMeshSceneProxy>(proxy);

                                        const auto& worldMatrix = skeletalProxy->GetMatrix();
                                        const auto& viewMatrices = shadowInfo->GetShadowViewMatrix();
                                        const auto& projectionMatrices = shadowInfo->GetShadowProjectionMatrix();

                                        mDepthCollectShaderSkeletal->GetVertexFactoryShader()->SetMatrices(
                                            worldMatrix, viewMatrices, projectionMatrices);
                                        mDepthCollectShaderSkeletal->GetVertexFactoryShader()->SetSkinningMatrices(
                                            skeletalProxy->GetSkinningMatrices());
                                        mDepthCollectShaderSkeletal->GetShader()->SetShadowDistance(
                                            spotlightPtr->GetRadianceRadius());
                                        mDepthCollectShaderSkeletal->GetShader()->SetLightWorldPosition(
                                            spotlightPtr->GetPosition());

                                        skeletalProxy->GetSkin()->GetBuffer()->RenderVAO(GL_TRIANGLES);
                                    }
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

                            if (mNonSkeletalProxiesVec.size() > 0) // Non - skeletal proxies
                            {
                                mDepthCollectPointLightShaderNonSkeletal->ExecuteShader();
                                for (auto& proxy : mNonSkeletalProxiesVec) {
                                    const bool bShouldRender = proxy->IsTransformIntialized() && proxy->IsEnabled()
                                        && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
                                    if (bShouldRender) {
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
                                }
                                mDepthCollectPointLightShaderNonSkeletal->StopShader();
                            }
                            if (mSkeletalProxiesVec.size() > 0) // Skeletal proxies
                            {
                                mDepthCollectPointLightShaderSkeletal->ExecuteShader();
                                for (auto& proxy : mSkeletalProxiesVec) {
                                    const bool bShouldRender = proxy->IsTransformIntialized() && proxy->IsEnabled()
                                        && proxy->IsVisible() && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
                                    if (bShouldRender) {
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

    glDisable(GL_CULL_FACE);
}

void SceneRenderer::DeferredBasePass_RenderThread(const std::shared_ptr<SceneView>& sceneView)
{
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    m_gbuffer->BindDeferredGBuffer();

    OutlinePass(sceneView);

    RenderState renderState;
    renderState.GetBlendingState().SetIsBlendingEnabled(false);
    renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);
    renderState.GetStencilState()
        .SetIsStencilTestEnabled(true)
        .SetStencilOperation(GL_KEEP, GL_KEEP, GL_REPLACE)
        .SetStencilFunction(GL_ALWAYS, EngineConstants::eStencilValues::DEFAULT, 0xFF)
        .SetStencilMask(0xFF);
    renderState.BindRenderState();

    // Deferred shading collect info
    const auto& cameraProxy = sceneView->GetCameraProxy();
    const auto& viewMatrix = cameraProxy->GetViewMatrix();
    const auto& projectionMatrix = cameraProxy->GetProjectionMatrix();

    glStencilFunc(GL_ALWAYS, EngineConstants::eStencilValues::DEFAULT, 0xFF);
    mInstancedGeometryBatchRenderer->RenderAllBatches(cameraProxy, viewMatrix, projectionMatrix, mActiveBindedState);

    if (mSkeletalProxiesVec.size() > 0) {
        for (auto& proxy : mSkeletalProxiesVec) {
            const bool bShouldRender = proxy->IsTransformIntialized() && proxy->IsEnabled() && proxy->IsVisible()
                && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
            if (bShouldRender) {
                const int32_t stencilFuncRefValue = proxy->CanBloomBeApplied() ? EngineConstants::eStencilValues::BLOOM
                                                                               : EngineConstants::eStencilValues::DEFAULT;
                glStencilFunc(GL_ALWAYS, stencilFuncRefValue, 0xFF);
                proxy->Render(cameraProxy, viewMatrix, projectionMatrix, mActiveBindedState);
            }
        }
    }

    if (mNonSkeletalProxiesVec.size() > 0) {
        for (auto& proxy : mNonSkeletalProxiesVec) {
            const bool bShouldRender = proxy->IsTransformIntialized() && proxy->IsEnabled() && proxy->IsVisible()
                && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
            if (bShouldRender) {
                const int32_t stencilFuncRefValue = proxy->CanBloomBeApplied() ? EngineConstants::eStencilValues::BLOOM
                                                                               : EngineConstants::eStencilValues::DEFAULT;
                glStencilFunc(GL_ALWAYS, stencilFuncRefValue, 0xFF);
                proxy->Render(cameraProxy, viewMatrix, projectionMatrix, mActiveBindedState);
            }
        }
    }

    renderState.GetStencilState().SetIsStencilTestEnabled(false);

    m_resolvedSceneFramebuffer->BindResolvedSceneFramebuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void SceneRenderer::DeferredLightPass_RenderThread(const std::shared_ptr<CameraSceneProxy>& cameraProxy)
{
    RenderState renderState;
    renderState.GetBlendingState().SetIsBlendingEnabled(false);

    renderState.GetDepthState().SetIsDepthTestEnabled(false).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(false);

    renderState.GetStencilState()
        .SetIsStencilTestEnabled(false)
        .SetStencilOperation(0, 0, 0)
        .SetStencilFunction(GL_NOTEQUAL, EngineConstants::eStencilValues::DEFAULT, 0xFF)
        .SetStencilMask(0);
    renderState.BindRenderState();
    // TODO: Make some check if light source (point or spot light) is too far from current view
    // position
    m_deferredLightShader->ExecuteShader();

#ifndef NO_LIT
    // ************************** SHADOWS ************************** //
    size_t pointLightIndex = 0, dirLightIndex = 0, spotlightIndex = 0;
    size_t shadowMapSlot = 5, dirShadowMapCount = 0, pointShadowMapCount = 0, spotlightShadowMapCount = 0;
    for (auto& dirLightProxy : mDirLightProxiesVec) {
        if (dirLightProxy->IsEnabled()) {
            const auto shadowInfo = dirLightProxy->GetProjectedDirShadowInfo();
            if (shadowInfo) {
                shadowInfo->GetAtlasResource()->BindTexture(shadowMapSlot);
                m_deferredLightShader->SetDirectionalLightShadowMapSlot(
                    dirLightIndex, shadowMapSlot, shadowInfo->GetTextureAtlasOffset());

                dirShadowMapCount++;
                dirLightIndex++;
                shadowMapSlot++;
            }
        }
    }

    for (auto& pointLightProxy : mPointLightProxiesVec) {
        if (pointLightProxy->IsEnabled()) {
            const auto shadowInfo = pointLightProxy->GetProjectedPointShadowInfo();
            if (shadowInfo) {
                shadowInfo->GetAtlasResource()->BindTexture(shadowMapSlot);
                m_deferredLightShader->SetPointLightShadowMapSlot(pointLightIndex, shadowMapSlot);
                shadowMapSlot++;
                pointShadowMapCount++;
                pointLightIndex++;
            }
        }
    }

    for (const auto& spotLightProxy : mSpotlightProxiesVec) {
        if (spotLightProxy->IsEnabled()) {
            const auto shadowInfo = spotLightProxy->GetProjectedSpotLightShadowInfo();
            if (shadowInfo) {
                shadowInfo->GetAtlasResource()->BindTexture(shadowMapSlot);
                m_deferredLightShader->SetSpotlightShadowMapSlot(
                    spotlightIndex, shadowMapSlot, shadowInfo->GetTextureAtlasOffset());
                shadowMapSlot++;
                spotlightShadowMapCount++;
                spotlightIndex++;
            }
        }
    }
// ************************** SHADOWS ************************** //
#endif
    m_deferredLightShader->SetCameraWorldPosition(cameraProxy->GetEyeVector());

    m_gbuffer->BindPositionTexture(0);
    m_gbuffer->BindAlbedoTexture(1);
    m_gbuffer->BindNormalTexture(2);
    m_gbuffer->BindMetallicRoughnessTexture(3);
    m_gbuffer->BindEmissionTexture(4);

    m_deferredLightShader->SetGBufferPosition(0);
    m_deferredLightShader->SetGBufferAlbedo(1);
    m_deferredLightShader->SetGBufferNormal(2);
    m_deferredLightShader->SetGBufferEmission(4);

#ifdef SHADING_MODEL_PBR
    m_deferredLightShader->SetGBufferMetallicRoughness(3);
#endif

#ifndef NO_LIT
    m_deferredLightShader->SetLightsInfo(LightProxiesVector);
#endif
    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
    m_deferredLightShader->StopShader();

    glDisable(GL_CULL_FACE);
    renderState.GetDepthState().SetDepthTestWriteMask(true);
    renderState.BindRenderState();
}

void SceneRenderer::ForwardBasePass_RenderThread(const std::shared_ptr<SceneView>& sceneView)
{
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

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

    RenderState renderState;
    renderState.GetBlendingState().SetIsBlendingEnabled(true).SetBlendingFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);

    renderState.GetStencilState()
        .SetIsStencilTestEnabled(true)
        .SetStencilOperation(GL_KEEP, GL_KEEP, GL_REPLACE)
        .SetStencilFunction(GL_ALWAYS, EngineConstants::eStencilValues::DEFAULT, 0xFF)
        .SetStencilMask(0xFF);
    renderState.BindRenderState();

    bool depthTestWriteMask = true;
    bool depthTestWriteMaskDirty = false;
    for (const auto& proxy : mForwardRenderingProxiesVec) {
        if (proxy->IsDepthWriteMaskEnabled() != depthTestWriteMask) {
            depthTestWriteMask = proxy->IsDepthWriteMaskEnabled();
            depthTestWriteMaskDirty = true;
        }
        if (depthTestWriteMaskDirty) {
            renderState.GetDepthState().SetDepthTestWriteMask(depthTestWriteMask);
            renderState.BindRenderState();
        }

        const bool bShouldRender = proxy->IsTransformIntialized() && proxy->IsEnabled() && proxy->IsVisible()
            && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
        if (bShouldRender) {
            const int32_t stencilFuncRefValue
                = proxy->CanBloomBeApplied() ? EngineConstants::eStencilValues::BLOOM : EngineConstants::eStencilValues::DEFAULT;
            glStencilFunc(GL_ALWAYS, stencilFuncRefValue, 0xFF);
            proxy->Render(
                sceneView->GetCameraProxy(),
                sceneView->GetCameraProxy()->GetViewMatrix(),
                cameraProxy->GetProjectionMatrix(),
                mActiveBindedState);
        }
    }

    renderState.GetDepthState().SetDepthTestWriteMask(true);
    renderState.GetBlendingState().SetIsBlendingEnabled(false);
    renderState.GetStencilState().SetIsStencilTestEnabled(false);
    glDisable(GL_CULL_FACE);

    m_resolvedSceneFramebuffer->UnbindFramebuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void SceneRenderer::OutlinePass(const std::shared_ptr<SceneView>& sceneView)
{
    RenderState renderState;
    renderState.GetStencilState()
        .SetIsStencilTestEnabled(true)
        .SetStencilOperation(GL_KEEP, GL_KEEP, GL_REPLACE)
        .SetStencilFunction(GL_ALWAYS, EngineConstants::eStencilValues::OUTLINE, 0xFF)
        .SetStencilMask(0xFF);
    renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);
    renderState.BindRenderState();

    const auto& cameraProxy = sceneView->GetCameraProxy();
    const auto& viewMatrix = cameraProxy->GetViewMatrix();
    const auto& projectionMatrix = cameraProxy->GetProjectionMatrix();

    // Write outline value to stencil for objects which has to be outlined
    {
        glColorMask(false, false, false, false);
        if (mSkeletalProxiesVec.size() > 0) {
            for (auto& proxy : mSkeletalProxiesVec) {
                const bool bShouldRender = proxy->IsTransformIntialized() && proxy->IsEnabled() && proxy->IsVisible()
                    && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
                if (bShouldRender && proxy->GetIsOutlineApplied()) {
                    proxy->RenderOutlineStencil(cameraProxy, viewMatrix, projectionMatrix, mActiveBindedState);
                }
            }
        }

        if (mNonSkeletalProxiesVec.size() > 0) {
            for (auto& proxy : mNonSkeletalProxiesVec) {
                const bool bShouldRender = proxy->IsTransformIntialized() && proxy->IsEnabled() && proxy->IsVisible()
                    && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
                if (bShouldRender && proxy->GetIsOutlineApplied()) {
                    proxy->RenderOutlineStencil(cameraProxy, viewMatrix, projectionMatrix, mActiveBindedState);
                }
            }
        }
        glColorMask(true, true, true, true);
    }

    // Draw outline (scaled up objects) only where stencil value is not equal to outline
    {
        renderState.GetStencilState()
            .SetIsStencilTestEnabled(true)
            .SetStencilOperation(GL_KEEP, GL_KEEP, GL_REPLACE)
            .SetStencilFunction(GL_NOTEQUAL, EngineConstants::eStencilValues::OUTLINE, 0xFF)
            .SetStencilMask(0x00);

        renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);
        renderState.BindRenderState();

        if (mSkeletalProxiesVec.size() > 0) {
            for (auto& proxy : mSkeletalProxiesVec) {
                const bool bShouldRender = proxy->IsTransformIntialized() && proxy->IsEnabled() && proxy->IsVisible()
                    && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
                if (bShouldRender && proxy->GetIsOutlineApplied()) {
                    proxy->RenderOutline(cameraProxy, viewMatrix, projectionMatrix, mActiveBindedState);
                }
            }
        }

        if (mNonSkeletalProxiesVec.size() > 0) {
            for (auto& proxy : mNonSkeletalProxiesVec) {
                const bool bShouldRender = proxy->IsTransformIntialized() && proxy->IsEnabled() && proxy->IsVisible()
                    && sceneView->IsPrimitiveVisible(proxy->GetSceneProxyId());
                if (bShouldRender && proxy->GetIsOutlineApplied()) {
                    proxy->RenderOutline(cameraProxy, viewMatrix, projectionMatrix, mActiveBindedState);
                }
            }
        }
    }
    renderState.GetStencilState()
        .SetIsStencilTestEnabled(true)
        .SetStencilOperation(GL_KEEP, GL_KEEP, GL_REPLACE)
        .SetStencilFunction(GL_ALWAYS, EngineConstants::eStencilValues::DEFAULT, 0xFF)
        .SetStencilMask(0xFF);
    renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);
    renderState.BindRenderState();
}

void SceneRenderer::PlanarReflectionPass()
{
    if (mPlanarReflectionProxiesVec.size() <= 0)
        return;

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CLIP_DISTANCE0);

    RenderState renderState;
    renderState.GetBlendingState().SetIsBlendingEnabled(false);

    renderState.GetDepthState().SetIsDepthTestEnabled(true).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);

    renderState.GetStencilState()
        .SetIsStencilTestEnabled(false)
        .SetStencilOperation(0, 0, 0)
        .SetStencilFunction(GL_NOTEQUAL, EngineConstants::eStencilValues::DEFAULT, 0xFF)
        .SetStencilMask(0);

    renderState.BindRenderState();

    for (const auto& planarReflectionProxy : mPlanarReflectionProxiesVec) {
        auto wp = planarReflectionProxy->GetSceneViewWeakPtr();
        if (auto scenViewSp = wp.lock()) {
            const auto& viewMatrix = scenViewSp->GetCameraProxy()->GetViewMatrix();
            const auto& projectionMatrix = scenViewSp->GetCameraProxy()->GetProjectionMatrix();
            const glm::mat4& mirrorMatrix = planarReflectionProxy->GetMirrorMatrix();
            const glm::vec4& mirrorPlane = planarReflectionProxy->GetReflectionPlane();

            const CameraFrustum& mirroredCameraFrustum
                = CameraFrustum::GetConstructedFromViewProjectionMatrices(viewMatrix * mirrorMatrix, projectionMatrix);

            planarReflectionProxy->RenderToPlanarReflectionFBO();

            if (mSkeletalProxiesVec.size() > 0) {
                for (auto& proxy : mSkeletalProxiesVec) {
                    if (proxy->IsTransformIntialized() && proxy->IsEnabled() && proxy->IsVisible()) {
                        bool bDraw = proxy->IsFrustumCullTestNeeded()
                            ? mirroredCameraFrustum.CollidesWithBoundingBox(proxy->GetTransformedBoundingBox())
                            : true;
                        if (bDraw)
                            proxy->RenderPlanarReflection(
                                mirrorPlane, mirrorMatrix, viewMatrix, projectionMatrix, mActiveBindedState);
                    }
                }
            }

            if (mNonSkeletalProxiesVec.size() > 0) {
                for (auto& proxy : mNonSkeletalProxiesVec) {
                    if (proxy->IsTransformIntialized() && proxy->IsEnabled() && proxy->IsVisible()) {
                        bool bDraw = proxy->IsFrustumCullTestNeeded()
                            ? mirroredCameraFrustum.CollidesWithBoundingBox(proxy->GetTransformedBoundingBox())
                            : true;
                        if (bDraw)
                            proxy->RenderPlanarReflection(
                                mirrorPlane, mirrorMatrix, viewMatrix, projectionMatrix, mActiveBindedState);
                    }
                }
            }

            if (mForwardRenderingProxiesVec.size() > 0) {
                for (auto& proxy : mForwardRenderingProxiesVec) {
                    if (proxy->IsTransformIntialized() && proxy->IsEnabled() && proxy->IsVisible()) {
                        bool bDraw = proxy->IsFrustumCullTestNeeded()
                            ? mirroredCameraFrustum.CollidesWithBoundingBox(proxy->GetTransformedBoundingBox())
                            : true;
                        if (bDraw)
                            proxy->RenderPlanarReflection(
                                mirrorPlane, mirrorMatrix, viewMatrix, projectionMatrix, mActiveBindedState);
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

void SceneRenderer::FontPass(const std::shared_ptr<SceneView>& sceneView)
{
    const auto& renderDataMap = mFreeTypeFontHandler->GetFontBatcherMap();
    const bool bHasTextToRender = std::any_of(renderDataMap.cbegin(), renderDataMap.cend(), [](const auto& pair) {
        return pair.second->GetFreeTypeTextFieldProxies().size() > 0;
    });

    RenderState renderState;
    if (bHasTextToRender) {
        renderState.GetBlendingState().SetIsBlendingEnabled(true).SetBlendingFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        renderState.GetDepthState().SetIsDepthTestEnabled(false).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(true);
        renderState.GetStencilState()
            .SetIsStencilTestEnabled(false)
            .SetStencilOperation(0, 0, 0)
            .SetStencilFunction(GL_NOTEQUAL, EngineConstants::eStencilValues::DEFAULT, 0xFF)
            .SetStencilMask(0);
        renderState.BindRenderState();

        m_fontShader->ExecuteShader();
        for (const auto& renderData : renderDataMap) {
            const auto& renderDataSp = renderData.second;
            renderDataSp->GetFontTextureAtlas()->BindTexture(0);

            const auto& textFields = renderDataSp->GetFreeTypeTextFieldProxies();
            m_fontShader->SetFontAtlasSlot(0);
            for (const auto& textField : textFields) {
                if (textField->IsVisible() && eTextFieldProxyType::HUD_TEXT_FIELD == textField->GetTextFieldProxyType()) {
                    m_fontShader->SetPosition(textField->GetPosition());
                    m_fontShader->SetColor(textField->GetColor());
                    renderDataSp->GetFreeTypeFontAtlas()->GetBuffer()->RenderVAO(
                        textField->GetVertexStart(), textField->GetVerticesCount(), GL_TRIANGLES);
                }
            }
        }
        m_fontShader->StopShader();
    }

    renderState.GetBlendingState().SetIsBlendingEnabled(false);
}

void SceneRenderer::GuiPass(const std::shared_ptr<SceneView>& sceneView)
{
    RenderState renderState;
    renderState.GetBlendingState().SetIsBlendingEnabled(true).SetBlendingFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderState.GetDepthState().SetIsDepthTestEnabled(false).SetDepthTestFunc(GL_LEQUAL).SetDepthTestWriteMask(false);

    renderState.GetStencilState()
        .SetIsStencilTestEnabled(false)
        .SetStencilOperation(0, 0, 0)
        .SetStencilFunction(GL_NOTEQUAL, EngineConstants::eStencilValues::DEFAULT, 0xFF)
        .SetStencilMask(0);

    renderState.BindRenderState();

    std::sort(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [](const auto& left, const auto& right) {
        return left->GetCanvasZOrder() < right->GetCanvasZOrder();
    });

    for (const auto& canvas : mUiCanvasProxies) {
        if (canvas->IsVisible()) {
            const auto& canvasOrigin = canvas->GetAbsoluteOrigin();
            const auto& widthHeight = canvas->GetWidthHeight();
            glViewport(canvasOrigin.x, canvasOrigin.y, widthHeight.x, widthHeight.y);
            canvas->Render();
        }
    }

    const auto& viewPortInfo = sceneView->GetCameraProxy()->GetViewPort();
    glViewport(viewPortInfo.OriginX, viewPortInfo.OriginY, viewPortInfo.Width, viewPortInfo.Height);

    renderState.GetDepthState().SetDepthTestWriteMask(true);
    renderState.BindRenderState();
}

void SceneRenderer::PrepareSceneProxiesForRender()
{
    if (bProxiesDirty) {
        mForwardRenderingProxiesVec.clear();
        mSkeletalProxiesVec.clear();
        mNonSkeletalProxiesVec.clear();

        for (auto& proxy : PrimitiveProxiesVector) {
            const auto primitiveProxyType = proxy->GetPrimitiveProxyType();
            if (primitiveProxyType != ePrimitiveProxyType::INDIRECT_RENDERED_PROXY) {
                if (proxy->IsDeferred()) {
                    if (proxy->GetPrimitiveProxyType() == ePrimitiveProxyType::SKELETAL_MESH_PROXY)
                        mSkeletalProxiesVec.emplace_back(std::static_pointer_cast<SkeletalMeshSceneProxy>(proxy));
                    else
                        mNonSkeletalProxiesVec.emplace_back(proxy);
                } else {
                    mForwardRenderingProxiesVec.emplace_back(proxy);
                }
            }
        }
        SetProxiesAreDirty(false);
    }

    if (bLightProxiesDirty) {
        mDirLightProxiesVec.clear();
        mPointLightProxiesVec.clear();
        mSpotlightProxiesVec.clear();

        for (auto& proxy : LightProxiesVector) {
            const LightSceneProxyType& lightType = proxy->GetLightProxyType();

            if (lightType == LightSceneProxyType::DIR_LIGHT) {
                mDirLightProxiesVec.emplace_back(std::static_pointer_cast<DirectionalLightSceneProxy>(proxy));
            } else if (lightType == LightSceneProxyType::POINT_LIGHT) {
                mPointLightProxiesVec.emplace_back(std::static_pointer_cast<PointLightSceneProxy>(proxy));
            } else if (lightType == LightSceneProxyType::SPOT_LIGHT) {
                mSpotlightProxiesVec.emplace_back(std::static_pointer_cast<SpotlightSceneProxy>(proxy));
            }
        }

        GroupLightsByShadowMap();

        SetLightProxiesAreDirty(false);
    }

    if (bPlanarReflectionProxiesDirty) {
        for (const auto& proxy : PlanarReflectionProxiesVector) {
            auto findIt = std::find_if(
                mPlanarReflectionProxiesVec.begin(), mPlanarReflectionProxiesVec.end(), [&](const auto& existingProxy) {
                    return proxy->GetSceneProxyId() == existingProxy->GetSceneProxyId();
                });

            if (mPlanarReflectionProxiesVec.end() == findIt) {
                mPlanarReflectionProxiesVec.emplace_back(proxy);
            }
        }

        SetPlanarReflectionProxiesAreDirty(false);
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
    PrepareSceneProxiesForRender();

    for (const auto& sceneView : SceneViewsVector) {
        const auto& cameraProxy = sceneView->GetCameraProxy();
        if (cameraProxy->IsInitializedFirstTime()) {
            sceneView->DoVisibilityTest();

            // Deferred shading is done with main camera
            if (eCameraSceneProxyType::MAIN_SCENE_CAMERA == cameraProxy->GetCameraSceneType()) {

                mActiveBindedState.Reset();

                SortPrimitives(sceneView);

                PlanarReflectionPass();

                DepthPass(sceneView);

                DeferredBasePass_RenderThread(sceneView);

                DeferredLightPass_RenderThread(cameraProxy);

                if (mForwardRenderingProxiesVec.size())
                    ForwardBasePass_RenderThread(sceneView);

                if (mPostFxRenderer)
                    mPostFxRenderer->Execute(m_resolvedSceneFramebuffer);

                FontPass(sceneView);

                GuiPass(sceneView);
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

void SceneRenderer::SetProxiesAreDirty(const bool bDirty)
{
    bProxiesDirty = bDirty;
}

void SceneRenderer::SetLightProxiesAreDirty(const bool bDirty)
{
    bLightProxiesDirty = bDirty;
}

void SceneRenderer::SetPlanarReflectionProxiesAreDirty(const bool bDirty)
{
    bPlanarReflectionProxiesDirty = bDirty;
}

std::shared_ptr<SceneView> SceneRenderer::GetSceneViewByProxyId(const int32_t proxyId) const
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
    PlanarReflectionProxiesVector.erase(std::remove_if(
        PlanarReflectionProxiesVector.begin(), PlanarReflectionProxiesVector.end(), [proxyId](const auto& planarReflectionProxy) {
            return planarReflectionProxy->GetSceneProxyId() == proxyId;
        }));
}

void SceneRenderer::AddMaterialProxy_OnRenderThread(const std::shared_ptr<MaterialProxy>& materialProxy)
{
    static const uint64_t functionId = Hash64_CT("SceneRenderer::AddMaterialProxy_OnRenderThread");

    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
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
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
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
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
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
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
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
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
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
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
        const auto& primitiveSp = GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
        if (primitiveSp) {
            primitiveSp->SetSortOrderValue(sortOrderValue);
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
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
        const auto& primitiveSp = GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
        if (primitiveSp) {
            primitiveSp->SetWorldMatrix(newworldMatrix);
            primitiveSp->SetOutlineMatrix(newOutlineMatrix);
            primitiveSp->SetTransformedBoundingBox(newTransformedBoundingBox);
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
                    }
                }
            });
    }
}

void SceneRenderer::UpdateLightComponentTransform_OnRenderThread(
    const int32_t lightSceneProxyIndex,
    const int32_t creatorObjectId,
    const uint64_t functionId,
    const glm::mat4& newworldMatrix)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
        const auto& lightSp = GetLightProxyByProxyId(lightSceneProxyIndex);
        if (lightSp) {
            lightSp->SetWorldMatrix(newworldMatrix);
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
                    }
                }
            });
    }
}

void SceneRenderer::RemovePrimitiveSceneProxy_OnRenderThread(const int32_t primitiveSceneProxyIndex)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
        RemovePrimitiveProxyByProxyId(primitiveSceneProxyIndex);
        SetProxiesAreDirty(true);
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
                    sceneRenderer->SetProxiesAreDirty(true);
                }
            });
    }
}

void SceneRenderer::UpdatePrimitiveSceneProxies_OnRenderThread()
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
        SetProxiesAreDirty(true);
    } else {
        m_interThreadMgr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            0,
            Hash64_CT("SceneRenderer::UpdatePrimitiveSceneProxies_OnRenderThread"),
            [weak = weak_from_this()](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRenderer = weak.lock()) {
                    sceneRenderer->SetProxiesAreDirty(true);
                }
            });
    }
}

void SceneRenderer::DeleteLightSceneProxy_OnRenderThread(const int32_t lightSceneProxyIndex)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
        RemoveLightProxyByProxyId(lightSceneProxyIndex);
        SetLightProxiesAreDirty(true);
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
                    sceneRenderer->SetLightProxiesAreDirty(true);
                }
            });
    }
}

void SceneRenderer::UpdateLightSceneProxies_OnRenderThread()
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
        SetLightProxiesAreDirty(true);
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
                    sceneRenderer->SetLightProxiesAreDirty(true);
                }
            });
    }
}

void SceneRenderer::AddCameraSceneProxy_OnRenderThread(
    const std::shared_ptr<ACamera>& camera, const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
        SceneViewsVector.emplace_back(std::make_shared<SceneView>(cameraSceneProxy, GetPrimitiveProxies()));
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
                    sceneRenderer->SceneViewsVector.emplace_back(
                        std::make_shared<SceneView>(cameraSceneProxy, sceneRenderer->GetPrimitiveProxies()));
                    camera->SetIsCameraProxyReady(true);
                }
            });
    }
}

void SceneRenderer::RemoveCameraSceneProxy_OnRenderThread(const int32_t cameraSceneProxyId)
{
    LogInfo("SceneRenderer::RemoveCameraSceneProxy_OnRenderThread: camera proxyId: ", cameraSceneProxyId);
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
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

    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
        ext_assert(
            !GetPrimitiveProxyByProxyId(primitiveSceneProxy->GetSceneProxyId()),
            "Primitive proxy with id {} already exists" + std::to_string(primitiveSceneProxy->GetSceneProxyId()));
        primitiveSceneProxy->PostConstructorInitialize();
        PrimitiveProxiesVector.emplace_back(primitiveSceneProxy);
        SetProxiesAreDirty(true);
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
                    sceneRenderer->SetProxiesAreDirty(true);
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

    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
        ext_assert(
            !GetLightProxyByProxyId(lightSceneProxy->GetSceneProxyId()),
            "Light proxy with id {} already exists" + std::to_string(lightSceneProxy->GetSceneProxyId()));
        LightProxiesVector.emplace_back(lightSceneProxy);
        SetLightProxiesAreDirty(true);
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
                    sceneRenderer->SetLightProxiesAreDirty(true);
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

    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
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

    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
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
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
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
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
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
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
        const auto& materialProxySp = GetMaterialProxyByProxyId(materialProxyIndex);
        if (materialProxySp) {
            materialProxySp->UpdateProperties(std::move(properties));
        } else {
            LogInfo(
                "SceneRenderer::MaterialPropertiesUpdated_OnRenderThread: "
                "Error! Current proxy index doesn't exist on RT. Proxy index = ",
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
                            "Error! Current proxy index doesn't exist on RT. Proxy index = ",
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
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
        const auto& reflectionProxySp = GetPlanarReflectionProxyByProxyId(proxy->GetSceneProxyId());
        ext_assert(
            !reflectionProxySp, "Planar reflection proxy with id {} already exists" + std::to_string(proxy->GetSceneProxyId()));
        PlanarReflectionProxiesVector.emplace_back(proxy);
        SetPlanarReflectionProxiesAreDirty(true);
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
                    sceneRenderer->SetPlanarReflectionProxiesAreDirty(true);
                    planarReflectionComponent->SetIsSceneProxyReady(true);
                }
            });
    }
}

void SceneRenderer::RemovePlanarReflectionSceneProxy_OnRenderThread(const int32_t planarReflectionProxyIndex)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
        RemovePlanarReflectionSceneProxyByProxyId(planarReflectionProxyIndex);
        SetPlanarReflectionProxiesAreDirty(true);
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
                    sceneRenderer->SetPlanarReflectionProxiesAreDirty(true);
                }
            });
    }
}

void SceneRenderer::BindPlanarReflectionSceneProxyToSceneView_OnRenderThread(
    const std::shared_ptr<PlanarReflectionProxy>& planarReflectionProxy, const int32_t cameraSceneProxyId)
{
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
        const auto& sceneViewSp = GetSceneViewByProxyId(cameraSceneProxyId);
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
                    const auto& sceneViewSp = sceneRenderer->GetSceneViewByProxyId(cameraSceneProxyId);
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
    if (ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render")) {
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
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"),
        "SceneRenderer::UnregisterUiCanvasProxy: This function must be called from Render thread");
    mUiCanvasProxies.erase(
        std::remove_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [canvasUiId](const auto& canvasProxy) {
            return canvasUiId == canvasProxy->GetUiItemUId();
        }));
}

void SceneRenderer::RegisterUiSceneProxy(const std::shared_ptr<UiSceneProxyBase>& sceneProxy, const size_t canvasUId)
{
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"),
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
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"),
        "SceneRenderer::UnregisterUiSceneProxy: This function must be called from Render thread");
    auto canvasIt = std::find_if(mUiCanvasProxies.begin(), mUiCanvasProxies.end(), [=](const auto& canvasProxy) {
        return canvasUId == canvasProxy->GetUiItemUId();
    });
    ext_assert(
        canvasIt != mUiCanvasProxies.end(),
        "SceneRenderer::UnregisterUiSceneProxy: canvas with id {} not found" + std::to_string(canvasUId));
    (*canvasIt)->RemoveUiSceneProxy(uiItemUId);
}

void SceneRenderer::SortPrimitives(const std::shared_ptr<SceneView>& sceneView)
{
    PrimitiveSorter sorter;
    mSkeletalProxiesVec = sorter.SortPrimitivesByShaderAndDistanceToCamera(sceneView->GetCameraProxy(), mSkeletalProxiesVec);
    mNonSkeletalProxiesVec
        = sorter.SortPrimitivesByShaderAndDistanceToCamera(sceneView->GetCameraProxy(), mNonSkeletalProxiesVec);
    mForwardRenderingProxiesVec = sorter.SortPrimitivesByOrderAndShader(mForwardRenderingProxiesVec);
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
        .SetStencilFunction(GL_NOTEQUAL, EngineConstants::eStencilValues::DEFAULT, 0xFF)
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

} // namespace Renderer
} // namespace Graphics