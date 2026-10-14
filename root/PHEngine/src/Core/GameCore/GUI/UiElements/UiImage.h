#pragma once

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "UiItemBase.h"

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

class UiImage : public UiItemBase {
    std::string mTextureSrc;

    std::shared_ptr<ITexture> mTexture;

    glm::vec3 mColor;

    bool mIsCustomColor;

    float mOpacity;

    float mRotationDegrees;

    bool mIsFlipped;

public:
    explicit UiImage(const std::string& name = std::string(""));

    ~UiImage() override;

    void SetTextureSrc(const std::string& textureSrc);

    void SetTexture(const std::shared_ptr<ITexture>& texture);

    std::string GetTextureSrc() const;

    std::shared_ptr<ITexture> GetTexture() const;

    void SetTextureColor(const glm::vec3& color);

    void SetTextureColor(const uint8_t r, const uint8_t g, const uint8_t b);

    glm::vec3 GetTextureColor() const;

    void SetIsCustomColorEnabled(const bool isCustomColorEnabled);

    bool IsCustomColorEnabled() const;

    void SetOpacity(const float opacity);

    float GetOpacity() const;

    void SetRotationDegrees(const float rotationDegrees);

    float GetRotationDegrees() const;

    void SetIsFlipped(const bool isFlipped);

    bool GetIsFlipped() const;

    std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const override;

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    bool OnPropertiesShouldBeUpdatedOnRenderThread() override;

    bool OnPropertiesShouldBeUpdatedOnLuaThread() override;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    std::string GetUiTypeString() const override;

protected:
    void OnRegistered() override;

    void OnUnregistered() override;

private:
    bool SyncDataOnRenderThread();

    bool SyncDataOnLuaThread();

    void ReallocateTexture(const bool updateRenderThreadData, const bool updateLuaThreadData);
};
} // namespace GUI
} // namespace EngineCore