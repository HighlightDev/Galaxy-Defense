
#include "UiProgressBar.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiProxies/UiProgressBarLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/UiSceneProxy/UiProgressBarSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace Resources;

namespace EngineCore {
namespace GUI {
UiProgressBar::UiProgressBar(const std::string& name)
    : UiItemBase(name)
    , mEmptyColor(glm::vec3(0.0f))
    , mFilledColor(glm::vec3(1.0f))
    , mOpacity(1.0f)
    , mFillPercentValue(0.0f)
    , mBorderRadius(0.0f)
    , mOpacityProperty(std::make_shared<EngineObjectProperty<float>>(
          mOpacity, "Opacity", [this](const float newOpacityValue) { SetOpacity(newOpacityValue); }))
{
    ext_assert(!mProperties.count("Opacity"), "UiProgressBar::ctor: Property 'Opacity' already exists");
    mProperties.emplace("Opacity", mOpacityProperty);
}

UiProgressBar::~UiProgressBar()
{
}

void UiProgressBar::OnRegistered()
{
    if (const auto& sceneSp = GetScene().lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            if (const auto& parentCanvasSp = mParentCanvas.lock()) {
                const auto thisSceneProxy = CreateUiSceneProxy();
                sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(
                    std::static_pointer_cast<UiProgressBar>(shared_from_this()), thisSceneProxy, parentCanvasSp->GetUId());
            }
        }
    }
}

void UiProgressBar::OnUnregistered()
{
}

bool UiProgressBar::OnPropertiesShouldBeUpdatedOnRenderThread()
{
    return UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread() && SyncDataOnRenderThread();
}

bool UiProgressBar::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    return UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread() && SyncDataOnLuaThread();
}

void UiProgressBar::SetEmptyColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(color, mEmptyColor)) {
        mEmptyColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiProgressBar::SetEmptyColor(const uint8_t r, const uint8_t g, const uint8_t b)
{
    static constexpr float INV_COLOR_MAX_BYTE_VALUE = 1.0f / 255.0f;
    glm::vec3 color = glm::vec3(
        static_cast<float>(r) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(g) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(b) * INV_COLOR_MAX_BYTE_VALUE);
    SetEmptyColor(color);
}

void UiProgressBar::SetEmptyColor(const uint32_t hexColor)
{
    SetEmptyColor(EngineMath::FromHexColorToVec3Color(hexColor));
}

void UiProgressBar::SetFilledColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(color, mFilledColor)) {
        mFilledColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiProgressBar::SetFilledColor(const uint8_t r, const uint8_t g, const uint8_t b)
{
    static constexpr float INV_COLOR_MAX_BYTE_VALUE = 1.0f / 255.0f;
    glm::vec3 color = glm::vec3(
        static_cast<float>(r) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(g) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(b) * INV_COLOR_MAX_BYTE_VALUE);
    SetFilledColor(color);
}

void UiProgressBar::SetFilledColor(const uint32_t hexColor)
{
    SetFilledColor(EngineMath::FromHexColorToVec3Color(hexColor));
}

void UiProgressBar::SetOpacity(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(opacity, mOpacity)) {
        mOpacity = opacity;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiProgressBar::SetFillPercentValue(const float value)
{
    if (!EngineMath::FloatsNearEqual(value, mFillPercentValue)) {
        mFillPercentValue = value;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiProgressBar::GetOpacity() const
{
    return mOpacity;
}

glm::vec3 UiProgressBar::GetEmptyColor() const
{
    return mEmptyColor;
}

glm::vec3 UiProgressBar::GetFilledColor() const
{
    return mFilledColor;
}

float UiProgressBar::GetFillPercentValue() const
{
    return mFillPercentValue;
}

float UiProgressBar::GetBorderRadius() const
{
    return mBorderRadius;
}

void UiProgressBar::SetBorderRadius(const float radius)
{
    if (!EngineMath::FloatsNearEqual(radius, mBorderRadius)) {
        mBorderRadius = radius;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

std::string UiProgressBar::GetUiTypeString() const
{
    return "UiProgressBar";
}

std::shared_ptr<UiSceneProxyBase> UiProgressBar::CreateUiSceneProxy() const
{
    return std::make_shared<UiProgressBarSceneProxy>(this);
}

std::shared_ptr<LuaProxy> UiProgressBar::ReplicateLuaProxy()
{
    return std::make_shared<UiProgressBarLuaProxy>(std::static_pointer_cast<UiProgressBar>(shared_from_this()));
}

void UiProgressBar::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
    UiItemBase::SyncFromLuaJsonProperties(luaJsonPropsStr);

    const auto& jsonObj = nlohmann::json::parse(luaJsonPropsStr);
    if (jsonObj.contains("empty_color")) {
        const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["empty_color"]);
        if (!EngineMath::CheckSimilarityVec3(color, mEmptyColor)) {
            mEmptyColor = color;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("filled_color")) {
        const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["filled_color"]);
        if (!EngineMath::CheckSimilarityVec3(color, mFilledColor)) {
            mFilledColor = color;
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
    if (jsonObj.contains("fill_percent_value")) {
        const auto fillPercent = jsonObj["fill_percent_value"].get<float>();
        if (!EngineMath::FloatsNearEqual(mFillPercentValue, fillPercent)) {
            mFillPercentValue = fillPercent;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("border_radius")) {
        const auto borderRadius = jsonObj["border_radius"].get<float>();
        if (!EngineMath::FloatsNearEqual(mBorderRadius, borderRadius)) {
            mBorderRadius = borderRadius;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
}

bool UiProgressBar::SyncDataOnRenderThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiProgressBar::SyncDataOnRenderThread");
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
                         canvasUId = canvasSp->GetUId(),
                         emptyColor = mEmptyColor,
                         filledColor = mFilledColor,
                         opacity = mOpacity,
                         fillPercentValue = mFillPercentValue,
                         borderRadius = mBorderRadius](
                            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                            std::weak_ptr<EngineCore::Scene> sceneWp,
                            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                            if (uiSceneProxy) {
                                const auto& statusBarSceneProxy = std::static_pointer_cast<UiProgressBarSceneProxy>(uiSceneProxy);
                                statusBarSceneProxy->SetEmptyColor(emptyColor);
                                statusBarSceneProxy->SetFilledColor(filledColor);
                                statusBarSceneProxy->SetOpacity(opacity);
                                statusBarSceneProxy->SetFillPercentValue(fillPercentValue);
                                statusBarSceneProxy->SetBorderRadius(borderRadius);
                            }
                        });
                }
            }
        }

        return true;
    }
    return false;
}

bool UiProgressBar::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiProgressBar::SyncDataOnLuaThread");
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
                     opacity = mOpacity,
                     emptyColor = mEmptyColor,
                     filledColor = mFilledColor,
                     fillPercentValue = mFillPercentValue,
                     borderRadius = mBorderRadius](
                        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                        std::weak_ptr<EngineCore::Scene> sceneWp,
                        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        if (const auto& progreessBarLuaProxy
                            = std::static_pointer_cast<UiProgressBarLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            progreessBarLuaProxy->SetOpacity_FromGameThread(opacity);
                            progreessBarLuaProxy->SetEmptyColor_FromGameThread(emptyColor);
                            progreessBarLuaProxy->SetFilledColor_FromGameThread(filledColor);
                            progreessBarLuaProxy->SetFillPercentValue_FromGrameThread(fillPercentValue);
                            progreessBarLuaProxy->SetBorderRadius_FromGameThread(borderRadius);
                        }
                    });
            }
        }
        return true;
    }
    return false;
}
} // namespace GUI
} // namespace EngineCore
