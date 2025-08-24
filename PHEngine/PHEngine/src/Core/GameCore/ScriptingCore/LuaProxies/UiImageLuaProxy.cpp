#include "UiImageLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GameCore/Scene.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {
UiImageLuaProxy::UiImageLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiImage>& ownerImage)
    : UiItemBaseLuaProxy(ownerImage)
    , mTextureSrc(ownerImage->GetTextureSrc())
    , mColor(ownerImage->GetTextureColor())
    , mIsCustomColor(ownerImage->IsCustomColorEnabled())
    , mOpacity(ownerImage->GetOpacity())
    , mRotationDegrees(ownerImage->GetRotationDegrees())
    , mIsFlipped(ownerImage->GetIsFlipped())
{
}

void UiImageLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
    static constexpr auto functionId = Hash64_CT("UiImageLuaProxy::OnLuaThreadDataUpdated");
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
                assert(replicator);
                const auto& uiImage = std::static_pointer_cast<::EngineCore::GUI::UiImage>(replicator);
                assert(uiImage);
                uiImage->SyncFromLuaJsonProperties(jsonStr);
            });
    }
}

std::string UiImageLuaProxy::GetGameThreadData()
{
    const auto& baseJsonStr = UiItemBaseLuaProxy::GetGameThreadData();
    auto jsonObj = nlohmann::json::parse(baseJsonStr);
    std::vector<float> colorVec = {mColor.r, mColor.g, mColor.b};
    jsonObj["color"] = colorVec;
    jsonObj["is_custom_color"] = mIsCustomColor;
    jsonObj["texture_source"] = mTextureSrc;
    jsonObj["opacity"] = mOpacity;
    jsonObj["rotation_degrees"] = mRotationDegrees;
    jsonObj["is_flipped"] = mIsFlipped;
    return jsonObj.dump();
}

void UiImageLuaProxy::SetTextureSource_FromGameThread(const std::string& textureSrc)
{
    if (mTextureSrc != textureSrc) {
        mTextureSrc = textureSrc;
        mIsLuaDataDirty = true;
    }
}

void UiImageLuaProxy::SetRotationDegrees_FromGameThread(const float rotationDegrees)
{
    if (!EngineMath::FloatsNearEqual(mRotationDegrees, rotationDegrees)) {
        mRotationDegrees = rotationDegrees;
        mIsLuaDataDirty = true;
    }
}

void UiImageLuaProxy::SetIsFlipped_FromGameThread(const bool isFlipped)
{
    if (mIsFlipped != isFlipped) {
        mIsFlipped = isFlipped;
        mIsLuaDataDirty = true;
    }
}

void UiImageLuaProxy::SetOpacity_FromGameThread(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
        mOpacity = opacity;
        mIsLuaDataDirty = true;
    }
}

void UiImageLuaProxy::SetColor_FromGameThread(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(mColor, color)) {
        mColor = color;
        mIsLuaDataDirty = true;
    }
}

void UiImageLuaProxy::SetUseCustomColor_FromGameThread(const bool enableCustomColor)
{
    if (mIsCustomColor != enableCustomColor) {
        mIsCustomColor = enableCustomColor;
        mIsLuaDataDirty = true;
    }
}
} // namespace Scripts
} // namespace EngineCore
