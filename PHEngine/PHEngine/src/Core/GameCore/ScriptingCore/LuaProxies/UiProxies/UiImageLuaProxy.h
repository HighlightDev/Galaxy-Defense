#pragma once

#include "UiItemBaseLuaProxy.h"

#include <glm/vec3.hpp>

#include <string>

namespace EngineCore {
namespace GUI {
class UiImage;
}
} // namespace EngineCore

namespace EngineCore {
namespace Scripts {
class UiImageLuaProxy : public UiItemBaseLuaProxy {
protected:
    std::string mTextureSrc;

    glm::vec3 mColor;

    bool mIsCustomColor{false};

    float mOpacity;

    float mRotationDegrees;

    bool mIsFlipped;

public:
    explicit UiImageLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiImage>& ownerUiItem);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetTextureSource_FromGameThread(const std::string& textureSrc);

    void SetOpacity_FromGameThread(const float opacity);

    void SetColor_FromGameThread(const glm::vec3& color);

    void SetUseCustomColor_FromGameThread(const bool enableCustomColor);

    void SetRotationDegrees_FromGameThread(const float rotationDegrees);

    void SetIsFlipped_FromGameThread(const bool isFlipped);
};
} // namespace Scripts
} // namespace EngineCore
