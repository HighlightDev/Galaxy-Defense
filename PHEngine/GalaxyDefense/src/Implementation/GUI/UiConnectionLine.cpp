#include "UiConnectionLine.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"
#include "Implementation/GUI/LuaProxies/UiConnectionLineLuaProxy.h"
#include "Implementation/GUI/SceneProxies/UiConnectionLineSceneProxy.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace Game;

namespace EngineCore {
namespace GUI {
UiConnectionLine::UiConnectionLine(const std::string& name)
    : UiItemBase(name)
    , mStartPoint(0.0f, 0.0f)
    , mEndPoint(1.0f, 1.0f)
    , mStartAnchorTarget("")
    , mEndAnchorTarget("")
    , mColor(0.35f, 0.55f, 0.78f)
    , mThicknessPx(2.0f)
    , mDashLengthPx(0.0f)
    , mGapLengthPx(0.0f)
    , mOpacity(1.0f)
{
}

UiConnectionLine::~UiConnectionLine()
{
}

void UiConnectionLine::OnRegistered()
{
    if (const auto& sceneSp = GetScene().lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            if (const auto& parentCanvasSp = mParentCanvas.lock()) {
                const auto thisSceneProxy = CreateUiSceneProxy();
                sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(
                    std::static_pointer_cast<UiConnectionLine>(shared_from_this()), thisSceneProxy, parentCanvasSp->GetUId());
            }
        }
    }
}

void UiConnectionLine::OnUnregistered()
{
}

void UiConnectionLine::OnPropertiesShouldBeUpdatedOnRenderThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();
    SyncDataOnRenderThread();
}

void UiConnectionLine::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();
    SyncDataOnLuaThread();
}

void UiConnectionLine::SetStartPoint(const glm::vec2& startPoint)
{
    if (!EngineMath::CheckSimilarityVec2(mStartPoint, startPoint)) {
        mStartPoint = startPoint;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

glm::vec2 UiConnectionLine::GetStartPoint() const
{
    return mStartPoint;
}

void UiConnectionLine::SetEndPoint(const glm::vec2& endPoint)
{
    if (!EngineMath::CheckSimilarityVec2(mEndPoint, endPoint)) {
        mEndPoint = endPoint;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

glm::vec2 UiConnectionLine::GetEndPoint() const
{
    return mEndPoint;
}

void UiConnectionLine::SetStartAnchorTarget(const std::string& targetUiItemName)
{
    if (mStartAnchorTarget != targetUiItemName) {
        mStartAnchorTarget = targetUiItemName;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        SetIsTransformDirty(true);
    }
}

std::string UiConnectionLine::GetStartAnchorTarget() const
{
    return mStartAnchorTarget;
}

void UiConnectionLine::SetEndAnchorTarget(const std::string& targetUiItemName)
{
    if (mEndAnchorTarget != targetUiItemName) {
        mEndAnchorTarget = targetUiItemName;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        SetIsTransformDirty(true);
    }
}

std::string UiConnectionLine::GetEndAnchorTarget() const
{
    return mEndAnchorTarget;
}

bool UiConnectionLine::IsTransformDependentToUiItem(const std::string& uiItemName) const
{
    if (UiItemBase::IsTransformDependentToUiItem(uiItemName)) {
        return true;
    }
    return (!mStartAnchorTarget.empty() && uiItemName == mStartAnchorTarget)
        || (!mEndAnchorTarget.empty() && uiItemName == mEndAnchorTarget);
}

void UiConnectionLine::RecalculateAnchorPositions()
{
    UiItemBase::RecalculateAnchorPositions();
    ResolveAnchoredEndpoints();
}

void UiConnectionLine::UnpausableTick(const float deltaTimeSec)
{
    UiItemBase::UnpausableTick(deltaTimeSec);
    // Reresolve endpoints each frame: this is robust to initialization order (target/parent may not be ready at the moment of
    // first RecalculateAnchorPositions) and keeps line anchored to nodes when they are moved.
    ResolveAnchoredEndpoints();
}

void UiConnectionLine::ResolveAnchoredEndpoints()
{
    if (mStartAnchorTarget.empty() && mEndAnchorTarget.empty()) {
        return;
    }

    const auto& lineBox = GetBoundingArea();
    const glm::vec2 lineMin = glm::vec2(lineBox.GetMin());
    const glm::vec2 lineSize = glm::vec2(lineBox.GetMax()) - lineMin;
    if (lineSize.x <= 0.0f || lineSize.y <= 0.0f) {
        return;
    }

    bool changed = false;
    const auto resolveEndpoint = [&](const std::string& targetName, glm::vec2& endpoint) {
        if (targetName.empty()) {
            return;
        }
        const auto& targetUiItem = TryFindAncestryUiItem(targetName);
        if (!targetUiItem) {
            return;
        }
        const auto& targetBox = targetUiItem->GetBoundingArea();
        const glm::vec2 targetCenter = glm::vec2(targetBox.GetOrigin());
        glm::vec2 normalized = (targetCenter - lineMin) / lineSize;
        if (!EngineMath::CheckSimilarityVec2(normalized, endpoint)) {
            endpoint = normalized;
            changed = true;
        }
    };
    resolveEndpoint(mStartAnchorTarget, mStartPoint);
    resolveEndpoint(mEndAnchorTarget, mEndPoint);

    if (changed) {
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
    }
}

void UiConnectionLine::SetColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mColor, color)) {
        mColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiConnectionLine::SetColor(const uint8_t r, const uint8_t g, const uint8_t b)
{
    static constexpr float INV_COLOR_MAX_BYTE_VALUE = 1.0f / 255.0f;
    SetColor(glm::vec3(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE));
}

glm::vec3 UiConnectionLine::GetColor() const
{
    return mColor;
}

void UiConnectionLine::SetThicknessPx(const float thicknessPx)
{
    if (!EngineMath::FloatsNearEqual(mThicknessPx, thicknessPx)) {
        mThicknessPx = thicknessPx;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiConnectionLine::GetThicknessPx() const
{
    return mThicknessPx;
}

void UiConnectionLine::SetDashLengthPx(const float dashLengthPx)
{
    if (!EngineMath::FloatsNearEqual(mDashLengthPx, dashLengthPx)) {
        mDashLengthPx = dashLengthPx;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiConnectionLine::GetDashLengthPx() const
{
    return mDashLengthPx;
}

void UiConnectionLine::SetGapLengthPx(const float gapLengthPx)
{
    if (!EngineMath::FloatsNearEqual(mGapLengthPx, gapLengthPx)) {
        mGapLengthPx = gapLengthPx;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiConnectionLine::GetGapLengthPx() const
{
    return mGapLengthPx;
}

void UiConnectionLine::SetOpacity(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
        mOpacity = opacity;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiConnectionLine::GetOpacity() const
{
    return mOpacity;
}

std::string UiConnectionLine::GetUiTypeString() const
{
    return "UiConnectionLine";
}

std::shared_ptr<UiSceneProxyBase> UiConnectionLine::CreateUiSceneProxy() const
{
    return std::make_shared<UiConnectionLineSceneProxy>(this);
}

std::shared_ptr<LuaProxy> UiConnectionLine::ReplicateLuaProxy()
{
    return std::make_shared<UiConnectionLineLuaProxy>(std::static_pointer_cast<UiConnectionLine>(shared_from_this()));
}

void UiConnectionLine::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
    UiItemBase::SyncFromLuaJsonProperties(luaJsonPropsStr);

    const auto& jsonObj = nlohmann::json::parse(luaJsonPropsStr);
    if (jsonObj.contains("start_point")) {
        const glm::vec2 startPoint = nlohmann_utilities::GetXyFromJsonMap(jsonObj["start_point"]);
        if (!EngineMath::CheckSimilarityVec2(startPoint, mStartPoint)) {
            mStartPoint = startPoint;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("end_point")) {
        const glm::vec2 endPoint = nlohmann_utilities::GetXyFromJsonMap(jsonObj["end_point"]);
        if (!EngineMath::CheckSimilarityVec2(endPoint, mEndPoint)) {
            mEndPoint = endPoint;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("color")) {
        const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["color"]);
        if (!EngineMath::CheckSimilarityVec3(color, mColor)) {
            mColor = color;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("thickness_px")) {
        const auto thicknessPx = jsonObj["thickness_px"].get<float>();
        if (!EngineMath::FloatsNearEqual(mThicknessPx, thicknessPx)) {
            mThicknessPx = thicknessPx;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("dash_length_px")) {
        const auto dashLengthPx = jsonObj["dash_length_px"].get<float>();
        if (!EngineMath::FloatsNearEqual(mDashLengthPx, dashLengthPx)) {
            mDashLengthPx = dashLengthPx;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("gap_length_px")) {
        const auto gapLengthPx = jsonObj["gap_length_px"].get<float>();
        if (!EngineMath::FloatsNearEqual(mGapLengthPx, gapLengthPx)) {
            mGapLengthPx = gapLengthPx;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("opacity")) {
        const auto opacity = jsonObj["opacity"].get<float>();
        if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
            mOpacity = opacity;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("start_anchor_target")) {
        const auto startAnchorTarget = jsonObj["start_anchor_target"].get<std::string>();
        if (mStartAnchorTarget != startAnchorTarget) {
            mStartAnchorTarget = startAnchorTarget;
            SetIsTransformDirty(true);
        }
    }
    if (jsonObj.contains("end_anchor_target")) {
        const auto endAnchorTarget = jsonObj["end_anchor_target"].get<std::string>();
        if (mEndAnchorTarget != endAnchorTarget) {
            mEndAnchorTarget = endAnchorTarget;
            SetIsTransformDirty(true);
        }
    }
}

void UiConnectionLine::SyncDataOnRenderThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiConnectionLine::SyncDataOnRenderThread");
    if (mIsSceneProxyReady.load(std::memory_order::seq_cst)) {
        if (const auto& sceneSp = GetScene().lock()) {
            if (const auto& canvasSp = GetParentCanvas().lock()) {
                if (const auto& sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
                    sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                        eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                        GetUId(),
                        functionId,
                        [sceneRenderer,
                         myUId = GetUId(),
                         canasUId = canvasSp->GetUId(),
                         startPoint = mStartPoint,
                         endPoint = mEndPoint,
                         color = mColor,
                         thicknessPx = mThicknessPx,
                         dashLengthPx = mDashLengthPx,
                         gapLengthPx = mGapLengthPx,
                         opacity = mOpacity](
                            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                            std::weak_ptr<EngineCore::Scene> sceneWp,
                            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canasUId);
                            if (uiSceneProxy) {
                                const auto& lineSceneProxy = std::static_pointer_cast<UiConnectionLineSceneProxy>(uiSceneProxy);
                                lineSceneProxy->SetStartPoint(startPoint);
                                lineSceneProxy->SetEndPoint(endPoint);
                                lineSceneProxy->SetColor(color);
                                lineSceneProxy->SetThicknessPx(thicknessPx);
                                lineSceneProxy->SetDashLengthPx(dashLengthPx);
                                lineSceneProxy->SetGapLengthPx(gapLengthPx);
                                lineSceneProxy->SetOpacity(opacity);
                            }
                        });
                }
            }
        }
    } else {
        mIsPropertiesShouldBeUpdatedOnRenderThread = true;
    }
}

void UiConnectionLine::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiConnectionLine::SyncDataOnLuaThread");
    if (mIsLuaProxyReady.load(std::memory_order::seq_cst)) {
        if (const auto& sceneSp = GetScene().lock()) {
            if (const auto& luaScriptProcessorSp = GetLuaScriptProcessorWp().lock()) {
                SetIsPropertiesShouldBeUpdatedOnLuaThread(false);
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                    GetUId(),
                    functionId,
                    [luaScriptProcessorSp,
                     luaProxyId = GetLuaProxyId(),
                     startPoint = mStartPoint,
                     endPoint = mEndPoint,
                     startAnchorTarget = mStartAnchorTarget,
                     endAnchorTarget = mEndAnchorTarget,
                     color = mColor,
                     thicknessPx = mThicknessPx,
                     dashLengthPx = mDashLengthPx,
                     gapLengthPx = mGapLengthPx,
                     opacity = mOpacity](
                        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                        std::weak_ptr<EngineCore::Scene> sceneWp,
                        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        if (const auto& lineLuaProxy
                            = std::static_pointer_cast<UiConnectionLineLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            lineLuaProxy->SetStartPoint_FromGameThread(startPoint);
                            lineLuaProxy->SetEndPoint_FromGameThread(endPoint);
                            lineLuaProxy->SetStartAnchorTarget_FromGameThread(startAnchorTarget);
                            lineLuaProxy->SetEndAnchorTarget_FromGameThread(endAnchorTarget);
                            lineLuaProxy->SetColor_FromGameThread(color);
                            lineLuaProxy->SetThicknessPx_FromGameThread(thicknessPx);
                            lineLuaProxy->SetDashLengthPx_FromGameThread(dashLengthPx);
                            lineLuaProxy->SetGapLengthPx_FromGameThread(gapLengthPx);
                            lineLuaProxy->SetOpacity_FromGameThread(opacity);
                        }
                    });
            }
        }
    }
}
} // namespace GUI
} // namespace EngineCore
