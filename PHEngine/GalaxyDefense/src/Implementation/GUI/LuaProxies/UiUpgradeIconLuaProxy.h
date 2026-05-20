#pragma once

#include "Core/GameCore/ScriptingCore/LuaProxies/UiProxies/UiItemBaseLuaProxy.h"

#include <glm/vec3.hpp>

#include <string>

namespace EngineCore::GUI {
class UiUpgradeIcon;
} // namespace EngineCore::GUI

namespace Game {

class UiUpgradeIconLuaProxy : public ::EngineCore::Scripts::UiItemBaseLuaProxy {
protected:
    std::string mTextureSrc;

    glm::vec3 mTextureColor;
    glm::vec3 mBorderColor;
    glm::vec3 mGlowColor;
    glm::vec3 mFillColor;

    bool mIsCustomColor;
    bool mIsFlipped;

    float mFillStrength;
    float mBorderThicknessPx;
    float mGlowSizePx;
    float mOpacity;
    float mRotationDegrees;

public:
    explicit UiUpgradeIconLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiUpgradeIcon>& ownerUiItem);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetTextureSource_FromGameThread(const std::string& textureSrc);

    void SetTextureColor_FromGameThread(const glm::vec3& color);

    void SetBorderColor_FromGameThread(const glm::vec3& color);

    void SetGlowColor_FromGameThread(const glm::vec3& color);

    void SetFillColor_FromGameThread(const glm::vec3& color);

    void SetIsCustomColor_FromGameThread(const bool isCustomColor);

    void SetFillStrength_FromGameThread(const float fillStrength);

    void SetBorderThicknessPx_FromGameThread(const float borderThicknessPx);

    void SetGlowSizePx_FromGameThread(const float glowSizePx);

    void SetOpacity_FromGameThread(const float opacity);

    void SetRotationDegrees_FromGameThread(const float rotationDegrees);

    void SetIsFlipped_FromGameThread(const bool isFlipped);
};

} // namespace Game
