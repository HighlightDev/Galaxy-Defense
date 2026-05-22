#include "UiUpgradeIconLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Implementation/GUI/UiUpgradeIcon.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;
using namespace EngineCore::Scripts;

namespace Game {

UiUpgradeIconLuaProxy::UiUpgradeIconLuaProxy(const std::shared_ptr<UiUpgradeIcon>& ownerUiItem)
    : UiItemBaseLuaProxy(ownerUiItem)
    , mTextureSrc(ownerUiItem->GetTextureSrc())
    , mTextureColor(ownerUiItem->GetTextureColor())
    , mBorderColor(ownerUiItem->GetBorderColor())
    , mGlowColor(ownerUiItem->GetGlowColor())
    , mFillColor(ownerUiItem->GetFillColor())
    , mIsCustomColor(ownerUiItem->IsCustomColorEnabled())
    , mIsFlipped(ownerUiItem->GetIsFlipped())
    , mGlowVisible(ownerUiItem->GetGlowVisible())
    , mFillStrength(ownerUiItem->GetFillStrength())
    , mBorderThicknessPx(ownerUiItem->GetBorderThicknessPx())
    , mGlowSizePx(ownerUiItem->GetGlowSizePx())
    , mOpacity(ownerUiItem->GetOpacity())
    , mRotationDegrees(ownerUiItem->GetRotationDegrees())
{
}

void UiUpgradeIconLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
    static constexpr auto functionId = Hash64_CT("UiUpgradeIconLuaProxy::OnLuaThreadDataUpdated");
    if (const auto sceneSp = mSceneWp.lock()) {
        const auto replicatorId = GetReplicatorId();
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            mLuaProxyId,
            functionId,
            [sceneSp, replicatorId, jsonStr = jsonParameters](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                const auto& replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                ext_assert(replicator, "UiUpgradeIconLuaProxy::OnLuaThreadDataUpdated: replicator is null");
                const auto& uiUpgradeIcon = std::static_pointer_cast<UiUpgradeIcon>(replicator);
                ext_assert(uiUpgradeIcon, "UiUpgradeIconLuaProxy::OnLuaThreadDataUpdated: uiUpgradeIcon is null");
                uiUpgradeIcon->SyncFromLuaJsonProperties(jsonStr);
            });
    }
}

std::string UiUpgradeIconLuaProxy::GetGameThreadData()
{
    const auto& baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
    auto jsonObj = nlohmann::json::parse(baseJsonStr);
    jsonObj["texture_source"] = mTextureSrc;
    jsonObj["color"] = std::vector<float>{mTextureColor.r, mTextureColor.g, mTextureColor.b};
    jsonObj["border_color"] = std::vector<float>{mBorderColor.r, mBorderColor.g, mBorderColor.b};
    jsonObj["glow_color"] = std::vector<float>{mGlowColor.r, mGlowColor.g, mGlowColor.b};
    jsonObj["fill_color"] = std::vector<float>{mFillColor.r, mFillColor.g, mFillColor.b};
    jsonObj["is_custom_color"] = mIsCustomColor;
    jsonObj["fill_strength"] = mFillStrength;
    jsonObj["border_thickness_px"] = mBorderThicknessPx;
    jsonObj["glow_size_px"] = mGlowSizePx;
    jsonObj["opacity"] = mOpacity;
    jsonObj["rotation_degrees"] = mRotationDegrees;
    jsonObj["is_flipped"] = mIsFlipped;
    jsonObj["glow_visible"] = mGlowVisible;
    return jsonObj.dump();
}

void UiUpgradeIconLuaProxy::SetTextureSource_FromGameThread(const std::string& textureSrc)
{
    if (mTextureSrc != textureSrc) {
        mTextureSrc = textureSrc;
        mIsLuaDataDirty = true;
    }
}

void UiUpgradeIconLuaProxy::SetTextureColor_FromGameThread(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mTextureColor, color)) {
        mTextureColor = color;
        mIsLuaDataDirty = true;
    }
}

void UiUpgradeIconLuaProxy::SetBorderColor_FromGameThread(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mBorderColor, color)) {
        mBorderColor = color;
        mIsLuaDataDirty = true;
    }
}

void UiUpgradeIconLuaProxy::SetGlowColor_FromGameThread(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mGlowColor, color)) {
        mGlowColor = color;
        mIsLuaDataDirty = true;
    }
}

void UiUpgradeIconLuaProxy::SetFillColor_FromGameThread(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mFillColor, color)) {
        mFillColor = color;
        mIsLuaDataDirty = true;
    }
}

void UiUpgradeIconLuaProxy::SetIsCustomColor_FromGameThread(const bool isCustomColor)
{
    if (mIsCustomColor != isCustomColor) {
        mIsCustomColor = isCustomColor;
        mIsLuaDataDirty = true;
    }
}

void UiUpgradeIconLuaProxy::SetFillStrength_FromGameThread(const float fillStrength)
{
    if (!EngineMath::FloatsNearEqual(mFillStrength, fillStrength)) {
        mFillStrength = fillStrength;
        mIsLuaDataDirty = true;
    }
}

void UiUpgradeIconLuaProxy::SetBorderThicknessPx_FromGameThread(const float borderThicknessPx)
{
    if (!EngineMath::FloatsNearEqual(mBorderThicknessPx, borderThicknessPx)) {
        mBorderThicknessPx = borderThicknessPx;
        mIsLuaDataDirty = true;
    }
}

void UiUpgradeIconLuaProxy::SetGlowSizePx_FromGameThread(const float glowSizePx)
{
    if (!EngineMath::FloatsNearEqual(mGlowSizePx, glowSizePx)) {
        mGlowSizePx = glowSizePx;
        mIsLuaDataDirty = true;
    }
}

void UiUpgradeIconLuaProxy::SetOpacity_FromGameThread(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
        mOpacity = opacity;
        mIsLuaDataDirty = true;
    }
}

void UiUpgradeIconLuaProxy::SetRotationDegrees_FromGameThread(const float rotationDegrees)
{
    if (!EngineMath::FloatsNearEqual(mRotationDegrees, rotationDegrees)) {
        mRotationDegrees = rotationDegrees;
        mIsLuaDataDirty = true;
    }
}

void UiUpgradeIconLuaProxy::SetIsFlipped_FromGameThread(const bool isFlipped)
{
    if (mIsFlipped != isFlipped) {
        mIsFlipped = isFlipped;
        mIsLuaDataDirty = true;
    }
}

void UiUpgradeIconLuaProxy::SetGlowVisible_FromGameThread(const bool glowVisible)
{
    if (mGlowVisible != glowVisible) {
        mGlowVisible = glowVisible;
        mIsLuaDataDirty = true;
    }
}

} // namespace Game
