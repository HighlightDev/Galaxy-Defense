#include "UiUpgradeIcon.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"
#include "Implementation/GUI/LuaProxies/UiUpgradeIconLuaProxy.h"
#include "Implementation/GUI/SceneProxies/UiUpgradeIconSceneProxy.h"

#include <json/json.hpp>

#include <functional>

using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace Resources;
using namespace Game;

namespace EngineCore {
namespace GUI {
UiUpgradeIcon::UiUpgradeIcon(const std::string& name)
    : UiItemBase(name)
    , mTextureSrc("")
    , mTexture()
    , mColor(1.0f, 1.0f, 1.0f)
    , mBorderColor(0.91f, 0.36f, 0.02f)
    , mGlowColor(1.0f, 0.55f, 0.22f)
    , mFillColor(0.91f, 0.36f, 0.02f)
    , mIsCustomColor(false)
    , mFillStrength(0.55f)
    , mBorderThicknessPx(2.0f)
    , mGlowSizePx(10.0f)
    , mOpacity(1.0f)
    , mRotationDegrees(0.0f)
    , mIsFlipped(false)
{
}

UiUpgradeIcon::~UiUpgradeIcon()
{
    if (mTexture) {
        TexturePool::GetInstance()->TryToFreeMemory(mTexture);
    }
}

void UiUpgradeIcon::OnRegistered()
{
    if (const auto& sceneSp = GetScene().lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            if (const auto& parentCanvasSp = mParentCanvas.lock()) {
                const auto thisSceneProxy = CreateUiSceneProxy();
                sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(
                    std::static_pointer_cast<UiUpgradeIcon>(shared_from_this()), thisSceneProxy, parentCanvasSp->GetUId());
            }
        }
    }
}

void UiUpgradeIcon::OnUnregistered()
{
}

void UiUpgradeIcon::ReallocateTexture(const bool updateRenderThreadData, const bool updateLuaThreadData)
{
    static constexpr uint64_t functionId = Hash64_CT("UiUpgradeIcon::ReallocateTexture");
    if (const auto& sceneSp = GetScene().lock()) {
        auto& interThreadMngr = sceneSp->GetInterThreadCommunicationManager();
        interThreadMngr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            GetUId(),
            functionId,
            [this, updateRenderThreadData, updateLuaThreadData](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                const auto& texturePool = TexturePool::GetInstance();
                if (mTexture) {
                    texturePool->TryToFreeMemory(mTexture);
                }
                mTexture = texturePool->GetOrAllocateResource(mTextureSrc);

                if (updateRenderThreadData) {
                    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                }
                if (updateLuaThreadData) {
                    SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
                }
            });
    }
}

void UiUpgradeIcon::OnPropertiesShouldBeUpdatedOnRenderThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();
    SyncDataOnRenderThread();
}

void UiUpgradeIcon::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();

    SyncDataOnLuaThread();
}

void UiUpgradeIcon::SetTextureSrc(const std::string& textureSrc)
{
    if (mTextureSrc != textureSrc) {
        mTextureSrc = textureSrc;
        ReallocateTexture(true, true);
    }
}

void UiUpgradeIcon::SetTexture(const std::shared_ptr<ITexture>& texture)
{
    static constexpr uint64_t functionId = Hash64_CT("UiUpgradeIcon::SetTexture");
    if (const auto& sceneSp = GetScene().lock()) {
        auto& interThreadMngr = sceneSp->GetInterThreadCommunicationManager();
        interThreadMngr.ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            GetUId(),
            functionId,
            [this, texture](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (mTexture && mTextureSrc != "") {
                    ext_assert(
                        TexturePool::GetInstance()->TryToFreeMemory(mTexture),
                        "UiUpgradeIcon::SetTexture: Failed to free previous texture memory");
                    mTextureSrc = "";
                    mTexture = nullptr;
                }

                mTexture = texture;
                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
            });
    }
}

std::string UiUpgradeIcon::GetTextureSrc() const
{
    return mTextureSrc;
}

std::shared_ptr<ITexture> UiUpgradeIcon::GetTexture() const
{
    return mTexture;
}

void UiUpgradeIcon::SetTextureColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mColor, color)) {
        mColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiUpgradeIcon::SetTextureColor(const uint8_t r, const uint8_t g, const uint8_t b)
{
    static constexpr float INV_COLOR_MAX_BYTE_VALUE = 1.0f / 255.0f;
    glm::vec3 color = glm::vec3(
        static_cast<float>(r) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(g) * INV_COLOR_MAX_BYTE_VALUE,
        static_cast<float>(b) * INV_COLOR_MAX_BYTE_VALUE);
    SetTextureColor(color);
}

glm::vec3 UiUpgradeIcon::GetTextureColor() const
{
    return mColor;
}

void UiUpgradeIcon::SetIsCustomColorEnabled(const bool isCustomColorEnabled)
{
    if (mIsCustomColor != isCustomColorEnabled) {
        mIsCustomColor = isCustomColorEnabled;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

bool UiUpgradeIcon::IsCustomColorEnabled() const
{
    return mIsCustomColor;
}

void UiUpgradeIcon::SetBorderColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mBorderColor, color)) {
        mBorderColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiUpgradeIcon::SetBorderColor(const uint8_t r, const uint8_t g, const uint8_t b)
{
    static constexpr float INV_COLOR_MAX_BYTE_VALUE = 1.0f / 255.0f;
    SetBorderColor(glm::vec3(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE));
}

glm::vec3 UiUpgradeIcon::GetBorderColor() const
{
    return mBorderColor;
}

void UiUpgradeIcon::SetGlowColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mGlowColor, color)) {
        mGlowColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiUpgradeIcon::SetGlowColor(const uint8_t r, const uint8_t g, const uint8_t b)
{
    static constexpr float INV_COLOR_MAX_BYTE_VALUE = 1.0f / 255.0f;
    SetGlowColor(glm::vec3(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE));
}

glm::vec3 UiUpgradeIcon::GetGlowColor() const
{
    return mGlowColor;
}

void UiUpgradeIcon::SetFillColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mFillColor, color)) {
        mFillColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiUpgradeIcon::SetFillColor(const uint8_t r, const uint8_t g, const uint8_t b)
{
    static constexpr float INV_COLOR_MAX_BYTE_VALUE = 1.0f / 255.0f;
    SetFillColor(glm::vec3(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE));
}

glm::vec3 UiUpgradeIcon::GetFillColor() const
{
    return mFillColor;
}

void UiUpgradeIcon::SetFillStrength(const float fillStrength)
{
    if (!EngineMath::FloatsNearEqual(mFillStrength, fillStrength)) {
        mFillStrength = fillStrength;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiUpgradeIcon::GetFillStrength() const
{
    return mFillStrength;
}

void UiUpgradeIcon::SetBorderThicknessPx(const float borderThicknessPx)
{
    if (!EngineMath::FloatsNearEqual(mBorderThicknessPx, borderThicknessPx)) {
        mBorderThicknessPx = borderThicknessPx;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiUpgradeIcon::GetBorderThicknessPx() const
{
    return mBorderThicknessPx;
}

void UiUpgradeIcon::SetGlowSizePx(const float glowSizePx)
{
    if (!EngineMath::FloatsNearEqual(mGlowSizePx, glowSizePx)) {
        mGlowSizePx = glowSizePx;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiUpgradeIcon::GetGlowSizePx() const
{
    return mGlowSizePx;
}

void UiUpgradeIcon::SetOpacity(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
        mOpacity = opacity;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiUpgradeIcon::GetOpacity() const
{
    return mOpacity;
}

void UiUpgradeIcon::SetRotationDegrees(const float rotationDegrees)
{
    if (!EngineMath::FloatsNearEqual(mRotationDegrees, rotationDegrees)) {
        mRotationDegrees = rotationDegrees;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiUpgradeIcon::GetRotationDegrees() const
{
    return mRotationDegrees;
}

void UiUpgradeIcon::SetIsFlipped(const bool isFlipped)
{
    if (mIsFlipped != isFlipped) {
        mIsFlipped = isFlipped;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

bool UiUpgradeIcon::GetIsFlipped() const
{
    return mIsFlipped;
}

std::string UiUpgradeIcon::GetUiTypeString() const
{
    return "UiUpgradeIcon";
}

std::shared_ptr<UiSceneProxyBase> UiUpgradeIcon::CreateUiSceneProxy() const
{
    return std::make_shared<UiUpgradeIconSceneProxy>(this);
}

std::shared_ptr<LuaProxy> UiUpgradeIcon::ReplicateLuaProxy()
{
    return std::make_shared<UiUpgradeIconLuaProxy>(std::static_pointer_cast<UiUpgradeIcon>(shared_from_this()));
}

void UiUpgradeIcon::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
    UiItemBase::SyncFromLuaJsonProperties(luaJsonPropsStr);

    const auto& jsonObj = nlohmann::json::parse(luaJsonPropsStr);
    if (jsonObj.contains("texture_source")) {
        const auto texture_source = jsonObj["texture_source"].get<std::string>();
        if (mTextureSrc != texture_source) {
            mTextureSrc = texture_source;
            ReallocateTexture(true, false);
        }
    }
    if (jsonObj.contains("color")) {
        const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["color"]);
        if (!EngineMath::CheckSimilarityVec3(color, mColor)) {
            mColor = color;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("border_color")) {
        const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["border_color"]);
        if (!EngineMath::CheckSimilarityVec3(color, mBorderColor)) {
            mBorderColor = color;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("glow_color")) {
        const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["glow_color"]);
        if (!EngineMath::CheckSimilarityVec3(color, mGlowColor)) {
            mGlowColor = color;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("fill_color")) {
        const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["fill_color"]);
        if (!EngineMath::CheckSimilarityVec3(color, mFillColor)) {
            mFillColor = color;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("is_custom_color")) {
        const auto is_custom_color = jsonObj["is_custom_color"].get<bool>();
        if (mIsCustomColor != is_custom_color) {
            mIsCustomColor = is_custom_color;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("fill_strength")) {
        const auto fill_strength = jsonObj["fill_strength"].get<float>();
        if (!EngineMath::FloatsNearEqual(mFillStrength, fill_strength)) {
            mFillStrength = fill_strength;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("border_thickness_px")) {
        const auto border_thickness_px = jsonObj["border_thickness_px"].get<float>();
        if (!EngineMath::FloatsNearEqual(mBorderThicknessPx, border_thickness_px)) {
            mBorderThicknessPx = border_thickness_px;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("glow_size_px")) {
        const auto glow_size_px = jsonObj["glow_size_px"].get<float>();
        if (!EngineMath::FloatsNearEqual(mGlowSizePx, glow_size_px)) {
            mGlowSizePx = glow_size_px;
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
    if (jsonObj.contains("rotation_degrees")) {
        const auto rotation_degrees = jsonObj["rotation_degrees"].get<float>();
        if (!EngineMath::FloatsNearEqual(mRotationDegrees, rotation_degrees)) {
            mRotationDegrees = rotation_degrees;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("is_flipped")) {
        const auto is_flipped = jsonObj["is_flipped"].get<bool>();
        if (mIsFlipped != is_flipped) {
            mIsFlipped = is_flipped;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
}

void UiUpgradeIcon::SyncDataOnRenderThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiUpgradeIcon::SyncDataOnRenderThread");
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
                         textureSp = mTexture,
                         iconColor = mColor,
                         borderColor = mBorderColor,
                         glowColor = mGlowColor,
                         fillColor = mFillColor,
                         isCustomColor = mIsCustomColor,
                         fillStrength = mFillStrength,
                         borderThicknessPx = mBorderThicknessPx,
                         glowSizePx = mGlowSizePx,
                         opacity = mOpacity,
                         rotationDegrees = mRotationDegrees,
                         isFlipped = mIsFlipped](
                            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                            std::weak_ptr<EngineCore::Scene> sceneWp,
                            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canasUId);
                            if (uiSceneProxy) {
                                const auto& iconSceneProxy = std::static_pointer_cast<UiUpgradeIconSceneProxy>(uiSceneProxy);
                                iconSceneProxy->SetTexture(textureSp);
                                iconSceneProxy->SetIconCustomColor(iconColor);
                                iconSceneProxy->SetBorderColor(borderColor);
                                iconSceneProxy->SetGlowColor(glowColor);
                                iconSceneProxy->SetFillColor(fillColor);
                                iconSceneProxy->SetIsCustomIconColor(isCustomColor);
                                iconSceneProxy->SetFillStrength(fillStrength);
                                iconSceneProxy->SetBorderThicknessPx(borderThicknessPx);
                                iconSceneProxy->SetGlowSizePx(glowSizePx);
                                iconSceneProxy->SetOpacity(opacity);
                                iconSceneProxy->SetRotationDegrees(rotationDegrees);
                                iconSceneProxy->SetIsFlipped(isFlipped);
                            }
                        });
                }
            }
        }
    } else {
        mIsPropertiesShouldBeUpdatedOnRenderThread = true;
    }
}

void UiUpgradeIcon::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiUpgradeIcon::SyncDataOnLuaThread");
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
                     textureSrc = mTextureSrc,
                     textureColor = mColor,
                     borderColor = mBorderColor,
                     glowColor = mGlowColor,
                     fillColor = mFillColor,
                     isCustomColor = mIsCustomColor,
                     fillStrength = mFillStrength,
                     borderThicknessPx = mBorderThicknessPx,
                     glowSizePx = mGlowSizePx,
                     opacity = mOpacity,
                     rotationDegrees = mRotationDegrees,
                     isFlipped = mIsFlipped](
                        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                        std::weak_ptr<EngineCore::Scene> sceneWp,
                        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        if (const auto& iconLuaProxy
                            = std::static_pointer_cast<UiUpgradeIconLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            iconLuaProxy->SetTextureSource_FromGameThread(textureSrc);
                            iconLuaProxy->SetTextureColor_FromGameThread(textureColor);
                            iconLuaProxy->SetBorderColor_FromGameThread(borderColor);
                            iconLuaProxy->SetGlowColor_FromGameThread(glowColor);
                            iconLuaProxy->SetFillColor_FromGameThread(fillColor);
                            iconLuaProxy->SetIsCustomColor_FromGameThread(isCustomColor);
                            iconLuaProxy->SetFillStrength_FromGameThread(fillStrength);
                            iconLuaProxy->SetBorderThicknessPx_FromGameThread(borderThicknessPx);
                            iconLuaProxy->SetGlowSizePx_FromGameThread(glowSizePx);
                            iconLuaProxy->SetOpacity_FromGameThread(opacity);
                            iconLuaProxy->SetRotationDegrees_FromGameThread(rotationDegrees);
                            iconLuaProxy->SetIsFlipped_FromGameThread(isFlipped);
                        }
                    });
            }
        }
    }
}
} // namespace GUI
} // namespace EngineCore
