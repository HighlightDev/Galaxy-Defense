#include "UiRectangle.h"

#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiRectangleLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/UiSceneProxy/UiRectangleSceneProxy.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"

#ifdef DEBUG
#include "Core/GameCore/GUI/UiElements/UiLabel.h"
#endif

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace Resources;
using namespace EngineCore::DataProviders;

namespace EngineCore {
namespace GUI {
UiRectangle::UiRectangle(const std::string& name)
    : UiItemBase(name)
    , mColor(glm::vec3(1.0f))
    , mOpacity(1.0f)
    , mBorderRadius(0.0f)
    , mIsRoundTop(true)
    , mIsRoundBottom(true)
    , mColorProperty(std::make_shared<EngineObjectProperty<glm::vec3>>(
          mColor, "Color", [this](const glm::vec3& newColorVaue) { SetColor(newColorVaue); }))
    , mOpacityProperty(std::make_shared<EngineObjectProperty<float>>(
          mOpacity, "Opacity", [this](const float newOpacityValue) { SetOpacity(newOpacityValue); }))
#ifdef DEBUG
    , mDebugLabel(std::make_shared<UiLabel>("Lora-VariableFont_wght", "Rectangle_DebugLabel_" + std::to_string(GetUId())))
#endif
{
    ext_assert(!mProperties.count("Color"), "UiRectangle::ctor: Property 'Color' already exists");
    ext_assert(!mProperties.count("Opacity"), "UiRectangle::ctor: Property 'Opacity' already exists");
    mProperties.emplace("Color", mColorProperty);
    mProperties.emplace("Opacity", mOpacityProperty);
}

UiRectangle::~UiRectangle()
{
}

void UiRectangle::OnRegistered()
{
    if (const auto& sceneSp = GetScene().lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            if (const auto& parentCanvasSp = mParentCanvas.lock()) {
                const auto thisSceneProxy = CreateUiSceneProxy();
                sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(
                    std::static_pointer_cast<UiRectangle>(shared_from_this()), thisSceneProxy, parentCanvasSp->GetUId());
            }
        }
    }
}

void UiRectangle::OnPostRegistered()
{
#ifdef DEBUG

    if (EngineUtility::EngineConfigHolder::GetInstance()->GetEngineConfig().EnableDebugUiWidgetNaming) {
        const auto& parentCanvasSp = mParentCanvas.lock();
        ext_assert(parentCanvasSp, "UiRectangle::OnPostRegistered: Parent canvas is expired, name: " + GetName());

        const auto labelWidth = GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth() / 2;
        const auto labelHeight = GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight() / 2;

        mDebugLabel->SetParents(parentCanvasSp, std::static_pointer_cast<UiRectangle>(shared_from_this()));
        mDebugLabel->Initialize();
        mDebugLabel->SetTextColor(glm::vec3(1.0f, 0.0f, 0.0f));
        mDebugLabel->SetFontSize(18);
        mDebugLabel->SetAnchor(eUiAnchor::HORIZONTAL_CENTER, eUiAnchor::HORIZONTAL_CENTER, GetName());
        mDebugLabel->SetAnchor(eUiAnchor::VERTICAL_CENTER, eUiAnchor::VERTICAL_CENTER, GetName());
        mDebugLabel->SetWidth(labelWidth);
        mDebugLabel->SetHeight(labelHeight);
        mDebugLabel->SetZOrder(std::numeric_limits<size_t>::max());
        mDebugLabel->SetText(GetName());
        mDebugLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::CENTER);
        mDebugLabel->SetTextVerticalAlignment(eTextVerticalAlignmentType::CENTER);
        mDebugLabel->UpdateIsHiddenForDebugging(true);
    }
#endif
}

#ifdef DEBUG
void UiRectangle::SetIsHiddenForDebugging(const bool isHiddenForDebugging)
{
    if (mDebugLabel) {
        mDebugLabel->UpdateIsHiddenForDebugging(isHiddenForDebugging);
        UiItemBase::SetIsHiddenForDebugging(isHiddenForDebugging);
    }
}
#endif

void UiRectangle::OnUnregistered()
{
}

void UiRectangle::SetColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(color, mColor)) {
        mColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiRectangle::SetColor(const uint8_t r, const uint8_t g, const uint8_t b)
{
    static constexpr float INV_COLOR_MAX_BYTE_VALUE = 1.0f / 255.0f;
    glm::vec3 color = glm::vec3(
        static_cast<float>(r) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(g) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(b) * INV_COLOR_MAX_BYTE_VALUE);
    SetColor(color);
}

void UiRectangle::OnPropertiesShouldBeUpdatedOnRenderThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();

    SyncDataOnRenderThread();
}

void UiRectangle::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();

    SyncDataOnLuaThread();
}

void UiRectangle::SetColor(const uint32_t hexColor)
{
    SetColor(EngineMath::FromHexColorToVec3Color(hexColor));
}

void UiRectangle::SetOpacity(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(opacity, mOpacity)) {
        mOpacity = opacity;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiRectangle::GetOpacity() const
{
    return mOpacity;
}

glm::vec3 UiRectangle::GetColor() const
{
    return mColor;
}

void UiRectangle::SetBorderRadius(const float radiusPx)
{
    if (!EngineMath::FloatsNearEqual(radiusPx, mBorderRadius)) {
        mBorderRadius = radiusPx;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiRectangle::GetBorderRadius() const
{
    return mBorderRadius;
}

bool UiRectangle::GetIsRoundTop() const
{
    return mIsRoundTop;
}

bool UiRectangle::GetIsRoundBottom() const
{
    return mIsRoundBottom;
}

void UiRectangle::SetIsRoundTop(const bool bIsRoundTop)
{
    if (mIsRoundTop != bIsRoundTop) {
        mIsRoundTop = bIsRoundTop;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiRectangle::SetIsRoundBottom(const bool bIsRoundBottom)
{
    if (mIsRoundBottom != bIsRoundBottom) {
        mIsRoundBottom = bIsRoundBottom;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

std::string UiRectangle::GetUiTypeString() const
{
    return "UiRectangle";
}

std::shared_ptr<UiSceneProxyBase> UiRectangle::CreateUiSceneProxy() const
{
    return std::make_shared<UiRectangleSceneProxy>(this);
}

std::shared_ptr<LuaProxy> UiRectangle::ReplicateLuaProxy()
{
    return std::make_shared<UiRectangleLuaProxy>(std::static_pointer_cast<UiRectangle>(shared_from_this()));
}

void UiRectangle::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
    UiItemBase::SyncFromLuaJsonProperties(luaJsonPropsStr);

    const auto jsonObj = nlohmann::json::parse(luaJsonPropsStr);
    if (jsonObj.contains("color")) {
        const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["color"]);
        if (!EngineMath::CheckSimilarityVec3(color, mColor)) {
            mColor = color;
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
    if (jsonObj.contains("border_radius")) {
        const auto borderRadius = jsonObj["border_radius"].get<float>();
        if (!EngineMath::FloatsNearEqual(mBorderRadius, borderRadius)) {
            mBorderRadius = borderRadius;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("is_round_top")) {
        const auto isRoundTop = jsonObj["is_round_top"].get<bool>();
        if (mIsRoundTop != isRoundTop) {
            mIsRoundTop = isRoundTop;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("is_round_bottom")) {
        const auto isRoundBottom = jsonObj["is_round_bottom"].get<bool>();
        if (mIsRoundBottom != isRoundBottom) {
            mIsRoundBottom = isRoundBottom;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
}

void UiRectangle::SyncDataOnRenderThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiRectangle::SyncDataOnRenderThread");
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
                         color = mColor,
                         opacity = mOpacity,
                         borderRadius = mBorderRadius,
                         isRoundTop = mIsRoundTop,
                         isRoundBottom = mIsRoundBottom](
                            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                            std::weak_ptr<EngineCore::Scene> sceneWp,
                            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                            if (uiSceneProxy) {
                                const auto& rectangleSceneProxy = std::static_pointer_cast<UiRectangleSceneProxy>(uiSceneProxy);
                                rectangleSceneProxy->SetColor(color);
                                rectangleSceneProxy->SetOpacity(opacity);
                                rectangleSceneProxy->SetBorderRadius(borderRadius);
                                rectangleSceneProxy->SetIsRoundTop(isRoundTop);
                                rectangleSceneProxy->SetIsRoundBottom(isRoundBottom);
                            }
                        });
                }
            }
        }
    } else {
        mIsPropertiesShouldBeUpdatedOnRenderThread = true;
    }
}

void UiRectangle::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiRectangle::SyncDataOnLuaThread");
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
                     color = mColor,
                     borderRadius = mBorderRadius,
                     isRoundTop = mIsRoundTop,
                     isRoundBottom = mIsRoundBottom](
                        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                        std::weak_ptr<EngineCore::Scene> sceneWp,
                        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        if (const auto& rectangleLuaProxy
                            = std::static_pointer_cast<UiRectangleLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            rectangleLuaProxy->SetOpacity_FromGameThread(opacity);
                            rectangleLuaProxy->SetColor_FromGameThread(color);
                            rectangleLuaProxy->SetBorderRadius_FromGameThread(borderRadius);
                            rectangleLuaProxy->SetIsRoundTop_FromGameThread(isRoundTop);
                            rectangleLuaProxy->SetIsRoundBottom_FromGameThread(isRoundBottom);
                        }
                    });
            }
        }
    }
}
} // namespace GUI
} // namespace EngineCore