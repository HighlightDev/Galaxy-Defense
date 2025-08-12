#pragma once

#include "Core/GameCore/GUI/Common/FontHandler.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontHandler.h"
#include "Core/GameCore/GUI/HudText/HudTextField.h"
#include "Core/GameCore/Physics/DebugRender/DebugPhysicsRenderData.h"
#include "Core/GameCore/ShaderImplementation/DeferredLightShader.h"
#include "Core/GameCore/ShaderImplementation/DepthCollectShader.h"
#include "Core/GameCore/ShaderImplementation/FontRenderingShader.h"
#include "Core/GameCore/ShaderImplementation/PointLightDepthShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkeletalMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatchRenderer.h"
#include "Core/GraphicsCore/OpenGL/Shader/MaterialShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryCompositeShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GraphicsCore/PostFX/PostFxRenderer.h"
#include "Core/GraphicsCore/Renderer/ActiveBindedState.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingGBuffer.h"
#include "Core/GraphicsCore/Renderer/RenderState.h"
#include "Core/GraphicsCore/Renderer/ResolvedSceneFramebuffer.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PlanarReflectionProxy.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SpotlightSceneProxy.h"
#include "Core/GraphicsCore/SceneViewInfo/SceneView.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GraphicsCore/UiSceneProxy/UiCanvasSceneProxy.h"
#include "Core/GraphicsCore/UiSceneProxy/UiSceneProxyBase.h"
#include "Core/InterThreadCommunicationMgr.h"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstddef>
#include <memory>
#include <utility>

using namespace Thread;
using namespace EngineCore;
using namespace EngineCore::ShaderImpl;
using namespace EnginePhysics;
using namespace Event;
using namespace Graphics::GeometryBatching;

namespace EngineCore {
class PrimitiveComponent;
class LightComponent;
class PlanarReflectionComponent;
class ACamera;

namespace GUI {
class UiItemBase;
class UiCanvas;
class FreeTypeFontHandler;
class FreeTypeTextFieldProxy;
} // namespace GUI
} // namespace EngineCore

namespace Graphics {
namespace Renderer {
class SceneRenderer : public std::enable_shared_from_this<SceneRenderer> {
    InterThreadCommunicationMgr& m_interThreadMgr;

    // G-buffers
    std::unique_ptr<DeferredShadingGBuffer> m_gbuffer;

    std::shared_ptr<ResolvedSceneFramebuffer> m_resolvedSceneFramebuffer;

    // Shaders
    std::shared_ptr<DeferredLightShader> m_deferredLightShader;
    std::shared_ptr<FontRenderingShader> m_fontShader;
    std::shared_ptr<VertexFactoryCompositeShader<SkeletalMeshVertexFactory<4>, DepthCollectShader>> mDepthCollectShaderSkeletal;
    std::shared_ptr<VertexFactoryCompositeShader<StaticMeshVertexFactory, DepthCollectShader>> mDepthCollectShaderNonSkeletal;
    std::shared_ptr<VertexFactoryCompositeShader<SkeletalMeshVertexFactory<4>, PointLightDepthCollectShader>>
        mDepthCollectPointLightShaderSkeletal;
    std::shared_ptr<VertexFactoryCompositeShader<StaticMeshVertexFactory, PointLightDepthCollectShader>>
        mDepthCollectPointLightShaderNonSkeletal;

    std::unique_ptr<PostFxRenderer> mPostFxRenderer;

    bool bProxiesDirty;
    bool bLightProxiesDirty;
    bool bPlanarReflectionProxiesDirty;

    ActiveBindedState mActiveBindedState;

#if DEBUG
    int32_t mDebugUiCanvasId{-1};
    bool bRenderDebugPhysicsData{false};
    DebugPhysicsRenderData mDebugPhysicsRenderData;
#endif

    std::vector<std::shared_ptr<SceneView>> SceneViewsVector;
    std::vector<std::shared_ptr<PrimitiveSceneProxy>> PrimitiveProxiesVector;
    std::vector<std::shared_ptr<LightSceneProxy>> LightProxiesVector;
    std::vector<std::shared_ptr<MaterialProxy>> MaterialProxiesVector;
    std::vector<std::shared_ptr<PlanarReflectionProxy>> PlanarReflectionProxiesVector;
    std::vector<std::shared_ptr<UiCanvasSceneProxy>> mUiCanvasProxies;
    std::shared_ptr<FontHandler> mFontHandler;

    std::shared_ptr<::EngineCore::GUI::FreeTypeFontHandler> mFreeTypeFontHandler;

    // these proxies are collected from general type of proxies
    std::vector<std::shared_ptr<PrimitiveSceneProxy>> mForwardRenderingProxiesVec;
    std::vector<std::shared_ptr<SkeletalMeshSceneProxy>> mSkeletalProxiesVec;
    std::vector<std::shared_ptr<PrimitiveSceneProxy>> mNonSkeletalProxiesVec;
    std::vector<std::shared_ptr<DirectionalLightSceneProxy>> mDirLightProxiesVec;
    std::vector<std::shared_ptr<PointLightSceneProxy>> mPointLightProxiesVec;
    std::vector<std::shared_ptr<SpotlightSceneProxy>> mSpotlightProxiesVec;
    std::vector<std::shared_ptr<PlanarReflectionProxy>> mPlanarReflectionProxiesVec;
    std::vector<std::pair<size_t, std::vector<std::shared_ptr<LightSceneProxy>>>> mGroupedByShadowAtlasLights;

    std::shared_ptr<InstancedGeometryBatchRenderer> mInstancedGeometryBatchRenderer;

public:
    SceneRenderer(InterThreadCommunicationMgr& interThreadMgr);

    ~SceneRenderer();

    void CleanUp();

    void PostLevelInit();

    void RenderScene_RenderThread();

    std::shared_ptr<SceneView> GetSceneViewByProxyId(const int32_t proxyId) const;
    std::shared_ptr<PrimitiveSceneProxy> GetPrimitiveProxyByProxyId(const int32_t proxyId) const;
    std::shared_ptr<LightSceneProxy> GetLightProxyByProxyId(const int32_t proxyId) const;
    std::shared_ptr<MaterialProxy> GetMaterialProxyByProxyId(const int32_t proxyId) const;
    std::shared_ptr<PlanarReflectionProxy> GetPlanarReflectionProxyByProxyId(const int32_t proxyId) const;
    std::shared_ptr<UiSceneProxyBase> GetUiSceneProxyByProxyId(const size_t proxyId, const size_t canvasId) const;
    std::shared_ptr<UiCanvasSceneProxy> GetCanvasSceneProxyByProxyId(const size_t proxyId) const;

    std::vector<std::shared_ptr<PrimitiveSceneProxy>>& GetPrimitiveProxies();

    std::shared_ptr<FreeTypeFontHandler> GetFontHandler() const;

    void RemovePrimitiveProxyByProxyId(const int32_t proxyId);

    void RemoveLightProxyByProxyId(const int32_t proxyId);

    void RemovePlanarReflectionSceneProxyByProxyId(const int32_t proxyId);

    void SetProxiesAreDirty(const bool bDirty);

    void SetLightProxiesAreDirty(const bool bDirty);

    void SetPlanarReflectionProxiesAreDirty(const bool bDirty);

    void MaterialProxyAdded_OnRenderThread(const std::shared_ptr<MaterialProxy>& materialProxy);

    void UpdatePrimitiveComponentEnable_OnRenderThread(
        const int32_t primitiveSceneProxyIndex, const int32_t creatorObjectId, const uint64_t functionId, const bool bEnabled);

    void UpdateLightComponentEnable_OnRenderThread(
        const int32_t lightSceneProxyIndex, const int32_t creatorObjectId, const uint64_t functionId, const bool bEnabled);

    void UpdatePrimitiveComponentVisibility_OnRenderThread(
        const int32_t primitiveSceneProxyIndex, const int32_t creatorObjectId, const uint64_t functionId, const bool visibility);

    void UpdateLightComponentIsVisible_OnRenderThread(
        const int32_t lightSceneProxyIndex, const int32_t creatorObjectId, const uint64_t functionId, const bool visibility);

    void UpdatePrimitiveComponentSortOrderValue_OnRenderThread(
        const int32_t primitiveSceneProxyIndex,
        const int32_t creatorObjectId,
        const uint64_t functionId,
        const int32_t sortOrderValue);

    void UpdatePrimitiveComponentTransform_OnRenderThread(
        const int32_t primitiveSceneProxyIndex,
        const int32_t creatorObjectId,
        const uint64_t functionId,
        const glm::mat4& newRelativeMatrix,
        const glm::mat4& newOutlineMatrix,
        const BoundingBox3D& newTransformedBoundingBox);

    void UpdateLightComponentTransform_OnRenderThread(
        const int32_t lightSceneProxyIndex,
        const int32_t creatorObjectId,
        const uint64_t functionId,
        const glm::mat4& newRelativeMatrix);

    void PrimitiveSceneProxyDeleted_OnRenderThread(const int32_t primitiveSceneProxyIndex);

    void PrimitiveSceneProxiesUpdated_OnRenderThread();

    void LightSceneProxyDeleted_OnRenderThread(const int32_t lightSceneProxyIndex);

    void LightSceneProxiesUpdated_OnRenderThread();

    void CameraSceneProxyAdded_OnRenderThread(
        const std::shared_ptr<::EngineCore::ACamera>& camera, const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy);

    void RemoveCameraSceneProxy_OnRenderThread(const int32_t cameraSceneProxyId);

    void PrimitiveSceneProxyAdded_OnRenderThread(
        const std::shared_ptr<::EngineCore::PrimitiveComponent>& primitiveComponent,
        const std::shared_ptr<PrimitiveSceneProxy>& primitiveSceneProxy);

    void LightSceneProxyAdded_OnRenderThread(
        const std::shared_ptr<::EngineCore::LightComponent>& lightComponent,
        const std::shared_ptr<LightSceneProxy>& lightSceneProxy);

    void
    RegisterText_OnRenderThread(const std::shared_ptr<HudTextField>& textField, const bool subscribeOnTextScreenSpaceSizeUpdate);

    void UnregisterText_OnRenderThread(const std::shared_ptr<HudTextField>& textField);

    void RegisterUiCanvasProxy_OnRenderThread(
        const std::shared_ptr<::EngineCore::GUI::UiCanvas>& uiCanvas, const std::shared_ptr<UiCanvasSceneProxy>& uiCanvasProxy);

    void UnregisterUiCanvasProxy_OnRenderThread(const size_t canvasUiId);

    void RegisterUiSceneProxy_OnRenderThread(
        const std::shared_ptr<::EngineCore::GUI::UiItemBase>& uiItem,
        const std::shared_ptr<UiSceneProxyBase>& uiSceneProxy,
        const size_t canvasUId);

    void UnregisterUiSceneProxy_OnRenderThread(const size_t uiItemUId, const size_t canvasUId);

    void TextDataChanged_OnRenderThread(
        const std::shared_ptr<HudTextField>& textField, const eTextChangedDataType textChangedDataType);

    void MaterialPropertiesUpdated_OnRenderThread(
        const int32_t materialProxyIndex, std::vector<std::shared_ptr<MaterialProperty>>&& properties);

    void PlanarReflectionSceneProxyAdded_OnRenderThread(
        const std::shared_ptr<::EngineCore::PlanarReflectionComponent>& planarReflectionComponent,
        const std::shared_ptr<PlanarReflectionProxy>& proxy);

    void BindPlanarReflectionSceneProxyToSceneView_OnRenderThread(
        const std::shared_ptr<PlanarReflectionProxy>& planarReflectionProxy, const int32_t cameraSceneProxyId);

    void RegisterText(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy);

    void UnregisterText(const int32_t textFieldProxyId);

    void TextPositionChanged(const int32_t textFieldProxyId, const glm::vec2& position);

    void TextColorChanged(const int32_t textFieldProxyId, const glm::vec3& color);

    void TextChanged(const int32_t textFieldProxyId, const std::string& text);

    void TextVisibilityChanged(const int32_t textFieldProxyId, const bool bIsVisible);

    InterThreadCommunicationMgr& GetInterThreadCommunicationManager();

    std::shared_ptr<InstancedGeometryBatchRenderer> GetInstancedGeometryBatchRenderer() const;

    void OnWindowSizeChanged(const ViewPortInfo& viewPortInfo);

    void Initialize();

#if DEBUG

    void SetDebugUiCanvasId(const int32_t debugCanvasProxyUId);

    void SetDebugPhysicsRenderData(const DebugPhysicsRenderData& debugPhysicsRenderData);

private:
    void DebugRenderPhysics(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
#endif

private:
    void InitializeCoreShaders();

    void RegisterUiCanvasProxy(const std::shared_ptr<UiCanvasSceneProxy>& canvasSceneProxy);

    void UnregisterUiCanvasProxy(const size_t canvasUiId);

    void RegisterUiSceneProxy(const std::shared_ptr<UiSceneProxyBase>& sceneProxy, const size_t canvasUId);

    void UnregisterUiSceneProxy(const size_t uiItemUId, const size_t canvasUId);

    void PrepareSceneProxiesForRender();

    void GroupLightsByShadowMap();

    void DeferredLightPass_RenderThread(const std::shared_ptr<CameraSceneProxy>& cameraProxy);

    void DeferredBasePass_RenderThread(const std::shared_ptr<SceneView>& sceneView);

    void ForwardBasePass_RenderThread(const std::shared_ptr<SceneView>& sceneView);

    void DepthPass(const std::shared_ptr<SceneView>& sceneView);

    void PlanarReflectionPass();

    void OutlinePass(const std::shared_ptr<SceneView>& sceneView);

    void HudTextPass();

    void FontPass(const std::shared_ptr<SceneView>& sceneView);

    void GuiPass(const std::shared_ptr<SceneView>& sceneView);

    void RegisterFonts();

    void SortPrimitives(const std::shared_ptr<SceneView>& sceneView);
};

} // namespace Renderer
} // namespace Graphics
