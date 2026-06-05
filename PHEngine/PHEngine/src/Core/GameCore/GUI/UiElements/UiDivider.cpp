#include "UiDivider.h"

#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiProxies/UiDividerLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/UiSceneProxy/UiDividerSceneProxy.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace Resources;
using namespace EngineCore::DataProviders;

namespace EngineCore {
namespace GUI {
UiDivider::UiDivider(const std::string& name)
    : UiItemBase(name)
    , mColor(glm::vec3(1.0f))
    , mOpacity(1.0f)
    , mLineWidthPx(20.0f)
    , mEdgeFade(0.0f)
    , mOrientation(eDividerOrientation::HORIZONTAL)
    , mColorProperty(
          std::make_shared<EngineObjectProperty<glm::vec3>>(
              mColor, "Color", [this](const glm::vec3& newColorValue) { SetColor(newColorValue); }))
    , mOpacityProperty(std::make_shared<EngineObjectProperty<float>>(mOpacity, "Opacity", [this](const float newOpacityValue) {
        SetOpacity(newOpacityValue);
    }))
{
    ext_assert(!mProperties.count("Color"), "UiDivider::ctor: Property 'Color' already exists");
    ext_assert(!mProperties.count("Opacity"), "UiDivider::ctor: Property 'Opacity' already exists");
    mProperties.emplace("Color", mColorProperty);
    mProperties.emplace("Opacity", mOpacityProperty);
}

UiDivider::~UiDivider()
{
}

void UiDivider::OnRegistered()
{
    if (const auto& sceneSp = GetScene().lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            if (const auto& parentCanvasSp = mParentCanvas.lock()) {
                const auto thisSceneProxy = CreateUiSceneProxy();
                sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(
                    std::static_pointer_cast<UiDivider>(shared_from_this()), thisSceneProxy, parentCanvasSp->GetUId());
            }
        }
    }
}

void UiDivider::OnUnregistered()
{
}

void UiDivider::SetColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(color, mColor)) {
        mColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiDivider::SetColor(const uint8_t r, const uint8_t g, const uint8_t b)
{
    static constexpr float INV_COLOR_MAX_BYTE_VALUE = 1.0f / 255.0f;
    glm::vec3 color = glm::vec3(
        static_cast<float>(r) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(g) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(b) * INV_COLOR_MAX_BYTE_VALUE);
    SetColor(color);
}

bool UiDivider::OnPropertiesShouldBeUpdatedOnRenderThread()
{
    return UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread() && SyncDataOnRenderThread();
}

bool UiDivider::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    return UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread() && SyncDataOnLuaThread();
}

void UiDivider::SetColor(const uint32_t hexColor)
{
    SetColor(EngineMath::FromHexColorToVec3Color(hexColor));
}

void UiDivider::SetOpacity(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(opacity, mOpacity)) {
        mOpacity = opacity;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiDivider::GetOpacity() const
{
    return mOpacity;
}

glm::vec3 UiDivider::GetColor() const
{
    return mColor;
}

void UiDivider::SetDividerOrientation(const eDividerOrientation orientation)
{
    if (mOrientation != orientation) {
        mOrientation = orientation;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

eDividerOrientation UiDivider::GetDividerOrientation() const
{
    return mOrientation;
}

void UiDivider::SetLineWidthPx(const float widthPx)
{
    if (!EngineMath::FloatsNearEqual(mLineWidthPx, widthPx)) {
        mLineWidthPx = widthPx;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiDivider::GetLineWidthPx() const
{
    return mLineWidthPx;
}

void UiDivider::SetEdgeFade(const float edgeFade)
{
    if (!EngineMath::FloatsNearEqual(mEdgeFade, edgeFade)) {
        mEdgeFade = edgeFade;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiDivider::GetEdgeFade() const
{
    return mEdgeFade;
}

std::string UiDivider::GetUiTypeString() const
{
    return "UiDivider";
}

std::shared_ptr<UiSceneProxyBase> UiDivider::CreateUiSceneProxy() const
{
    return std::make_shared<UiDividerSceneProxy>(this);
}

std::shared_ptr<LuaProxy> UiDivider::ReplicateLuaProxy()
{
    return std::make_shared<UiDividerLuaProxy>(std::static_pointer_cast<UiDivider>(shared_from_this()));
}

void UiDivider::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
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
    if (jsonObj.contains("line_width_px")) {
        const auto lineWidthPx = jsonObj["line_width_px"].get<float>();
        if (!EngineMath::FloatsNearEqual(mLineWidthPx, lineWidthPx)) {
            mLineWidthPx = lineWidthPx;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("edge_fade")) {
        const auto edgeFade = jsonObj["edge_fade"].get<float>();
        if (!EngineMath::FloatsNearEqual(mEdgeFade, edgeFade)) {
            mEdgeFade = edgeFade;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("divider_orientation")) {
        const auto dividerOrientation = static_cast<eDividerOrientation>(jsonObj["divider_orientation"].get<int32_t>());
        if (mOrientation != dividerOrientation) {
            mOrientation = dividerOrientation;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
}

bool UiDivider::SyncDataOnRenderThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiDivider::SyncDataOnRenderThread");
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
                         lineWidthPx = mLineWidthPx,
                         edgeFade = mEdgeFade,
                         orientation = mOrientation](
                            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                            std::weak_ptr<EngineCore::Scene> sceneWp,
                            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                            if (uiSceneProxy) {
                                const auto& dividerSceneProxy = std::static_pointer_cast<UiDividerSceneProxy>(uiSceneProxy);
                                dividerSceneProxy->SetColor(color);
                                dividerSceneProxy->SetOpacity(opacity);
                                dividerSceneProxy->SetLineWidthPx(lineWidthPx);
                                dividerSceneProxy->SetEdgeFade(edgeFade);
                                dividerSceneProxy->SetDividerOrientation(orientation);
                            }
                        });
                }
            }
        }

        return true;
    }
    return false;
}

bool UiDivider::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiDivider::SyncDataOnLuaThread");
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
                     lineWidthPx = mLineWidthPx,
                     edgeFade = mEdgeFade,
                     orientation = mOrientation](
                        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                        std::weak_ptr<EngineCore::Scene> sceneWp,
                        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        if (const auto& dividerLuaProxy
                            = std::static_pointer_cast<UiDividerLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            dividerLuaProxy->SetOpacity_FromGameThread(opacity);
                            dividerLuaProxy->SetColor_FromGameThread(color);
                            dividerLuaProxy->SetLineWidthPx_FromGameThread(lineWidthPx);
                            dividerLuaProxy->SetEdgeFade_FromGameThread(edgeFade);
                            dividerLuaProxy->SetDividerOrientation_FromGameThread(orientation);
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