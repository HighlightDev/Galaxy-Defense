#pragma once

#include <glm/vec2.hpp>
#include <stdint.h>

#include <cstddef>
#include <cstdint>
#include <memory>

namespace EngineCore {
namespace GUI {
class UiItemBase;
}
} // namespace EngineCore

namespace Graphics {
namespace Proxy {
class UiCanvasSceneProxy;

class UiSceneProxyBase {
protected:
    size_t mUiItemUId;

    bool mIsVisible;

    size_t mZOrder;

    std::weak_ptr<UiCanvasSceneProxy> mParentCanvasProxy;

    glm::vec2 mNormalizedTranslation, mNormalizedScale;

    glm::ivec2 mWidthHightPixels;

    float mOverlayOpacity{1.0f};

    float mScale{1.0f};

    glm::vec2 mCenterOffset;

public:
    UiSceneProxyBase(const ::EngineCore::GUI::UiItemBase* uiItemBase);

    virtual ~UiSceneProxyBase() = default;

    void SetUiItemUid(const size_t UId);

    size_t GetUiItemUId() const;

    void SetIsVisible(const bool isVisible);

    bool IsVisible() const;

    void SetZOrder(const size_t zOrder);

    size_t GetZOrder() const;

    void SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale);

    void SetWidthHeightPixels(const glm::ivec2& widthHeight);

    void SetCanvasSceneProxy(const std::weak_ptr<UiCanvasSceneProxy>& parentCanvasProxy);

    void SetOverlayOpacity(const float overlayOpacity);

    float GetOverlayOpacity() const;

    void SetScale(const float scale);

    float GetScale() const;

    void SetCenterOffset(const glm::vec2& offset);

    glm::vec2 GetCenterOffset() const;

    virtual void OnSceneProxyRegistered() = 0;

    virtual void Render() = 0;

    virtual void CleanUp() = 0;
};
} // namespace Proxy
} // namespace Graphics