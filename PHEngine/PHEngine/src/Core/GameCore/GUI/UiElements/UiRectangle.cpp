#include "UiRectangle.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiRectangleLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/UiSceneProxy/UiRectangleSceneProxy.h"
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
UiRectangle::UiRectangle(const std::string& name)
    : UiItemBase(name)
    , mColor(glm::vec3(1.0f))
    , mOpacity(1.0f)
    , mBorderRadius(0.0f)
    , mColorProperty(std::make_shared<EngineObjectProperty<glm::vec3>>(
          mColor, "Color", [this](const glm::vec3& newColorVaue) { SetColor(newColorVaue); }))
    , mOpacityProperty(std::make_shared<EngineObjectProperty<float>>(
          mOpacity, "Opacity", [this](const float newOpacityValue) { SetOpacity(newOpacityValue); }))
{
    assert(!mProperties.count("Color"));
    assert(!mProperties.count("Opacity"));
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

    const auto& jsonObj = nlohmann::json::parse(luaJsonPropsStr);
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
}

void UiRectangle::SyncDataOnRenderThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiRectangle::SyncDataOnRenderThread");
    if (mIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst)) {
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
                         borderRadius = mBorderRadius]() {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                            if (uiSceneProxy) {
                                const auto& rectangleSceneProxy = std::static_pointer_cast<UiRectangleSceneProxy>(uiSceneProxy);
                                rectangleSceneProxy->SetColor(color);
                                rectangleSceneProxy->SetOpacity(opacity);
                                rectangleSceneProxy->SetBorderRadius(borderRadius);
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
    if (mIsLuaProxyReady.load(std::memory_order::memory_order_seq_cst)) {
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
                     borderRadius = mBorderRadius]() {
                        if (const auto& rectangleLuaProxy
                            = std::static_pointer_cast<UiRectangleLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            rectangleLuaProxy->SetOpacity_FromGameThread(opacity);
                            rectangleLuaProxy->SetColor_FromGameThread(color);
                            rectangleLuaProxy->SetBorderRadius_FromGameThread(borderRadius);
                        }
                    });
            }
        }
    }
}
} // namespace GUI
} // namespace EngineCore