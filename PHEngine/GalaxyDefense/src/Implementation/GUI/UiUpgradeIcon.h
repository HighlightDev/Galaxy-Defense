#pragma once

#include "Core/GameCore/GUI/UiElements/UiItemBase.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

#include <glm/vec3.hpp>

namespace Graphics {
namespace Proxy {
class UiSceneProxyBase;
}
} // namespace Graphics

namespace EngineCore {
namespace Scripts {
class LuaProxy;
}
} // namespace EngineCore

using namespace Graphics::Texture;

namespace EngineCore {
namespace GUI {
class UiCanvas;

class UiUpgradeIcon : public UiItemBase {
    std::string mTextureSrc;

    std::shared_ptr<ITexture> mTexture;

    glm::vec3 mColor;
    glm::vec3 mBorderColor;
    glm::vec3 mGlowColor;
    glm::vec3 mFillColor;

    bool mIsCustomColor;

    float mFillStrength;
    float mBorderThicknessPx;
    float mGlowSizePx;

    float mOpacity;

    float mRotationDegrees;

    bool mIsFlipped;

public:
    explicit UiUpgradeIcon(const std::string& name = std::string(""));

    ~UiUpgradeIcon() override;

    void SetTextureSrc(const std::string& textureSrc);

    void SetTexture(const std::shared_ptr<ITexture>& texture);

    std::string GetTextureSrc() const;

    std::shared_ptr<ITexture> GetTexture() const;

    void SetTextureColor(const glm::vec3& color);

    void SetTextureColor(const uint8_t r, const uint8_t g, const uint8_t b);

    glm::vec3 GetTextureColor() const;

    void SetIsCustomColorEnabled(const bool isCustomColorEnabled);

    bool IsCustomColorEnabled() const;

    void SetBorderColor(const glm::vec3& color);

    void SetBorderColor(const uint8_t r, const uint8_t g, const uint8_t b);

    glm::vec3 GetBorderColor() const;

    void SetGlowColor(const glm::vec3& color);

    void SetGlowColor(const uint8_t r, const uint8_t g, const uint8_t b);

    glm::vec3 GetGlowColor() const;

    void SetFillColor(const glm::vec3& color);

    void SetFillColor(const uint8_t r, const uint8_t g, const uint8_t b);

    glm::vec3 GetFillColor() const;

    void SetFillStrength(const float fillStrength);

    float GetFillStrength() const;

    void SetBorderThicknessPx(const float borderThicknessPx);

    float GetBorderThicknessPx() const;

    void SetGlowSizePx(const float glowSizePx);

    float GetGlowSizePx() const;

    void SetOpacity(const float opacity);

    float GetOpacity() const;

    void SetRotationDegrees(const float rotationDegrees);

    float GetRotationDegrees() const;

    void SetIsFlipped(const bool isFlipped);

    bool GetIsFlipped() const;

    std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const override;

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    void OnPropertiesShouldBeUpdatedOnRenderThread() override;

    void OnPropertiesShouldBeUpdatedOnLuaThread() override;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    std::string GetUiTypeString() const override;

protected:
    void OnRegistered() override;

    void OnUnregistered() override;

private:
    void SyncDataOnRenderThread();

    void SyncDataOnLuaThread();

    void ReallocateTexture(const bool updateRenderThreadData, const bool updateLuaThreadData);
};
} // namespace GUI
} // namespace EngineCore
