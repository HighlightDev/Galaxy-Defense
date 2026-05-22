#pragma once

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/UiSceneProxy/UiSceneProxyBase.h"
#include "Implementation/GUI/Shaders/UiUpgradeIconGlowShader.h"
#include "Implementation/GUI/Shaders/UiUpgradeIconShader.h"

#include <glm/vec3.hpp>

#include <memory>

namespace EngineCore::GUI {
class UiUpgradeIcon;
} // namespace EngineCore::GUI

using namespace Graphics::Texture;

namespace Game {

class UiUpgradeIconSceneProxy : public ::Graphics::Proxy::UiSceneProxyBase {
    std::shared_ptr<UiUpgradeIconShader> mShader;
    std::shared_ptr<UiUpgradeIconGlowShader> mGlowShader;

    std::shared_ptr<ITexture> mTexture;

    glm::vec3 mFillColor;
    glm::vec3 mBorderColor;
    glm::vec3 mGlowColor;
    glm::vec3 mIconCustomColor;

    bool mIsCustomIconColor;

    float mFillStrength;
    float mBorderThicknessPx;
    float mGlowSizePx;
    float mOpacity;

    float mRotationDegrees;

    bool mIsFlipped;
    bool mGlowVisible;

public:
    explicit UiUpgradeIconSceneProxy(const ::EngineCore::GUI::UiUpgradeIcon* uiUpgradeIcon);

    ~UiUpgradeIconSceneProxy() override;

    void Render() override;

    void SetTexture(const std::shared_ptr<ITexture>& texture);

    void SetFillColor(const glm::vec3& color);

    void SetBorderColor(const glm::vec3& color);

    void SetGlowColor(const glm::vec3& color);

    void SetIconCustomColor(const glm::vec3& color);

    void SetIsCustomIconColor(const bool isCustomIconColor);

    void SetFillStrength(const float fillStrength);

    void SetBorderThicknessPx(const float borderThicknessPx);

    void SetGlowSizePx(const float glowSizePx);

    void SetOpacity(const float opacity);

    void SetRotationDegrees(const float rotationDegrees);

    void SetIsFlipped(const bool isFlipped);

    void SetGlowVisible(const bool glowVisible);

    void CleanUp() override;

    void OnSceneProxyRegistered() override;
};

} // namespace Game
