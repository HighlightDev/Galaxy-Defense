#pragma once

#include <glm/vec2.hpp>
#include <stdint.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <string>

namespace EngineCore {
namespace GUI {
class UiItemBase;
}
} // namespace EngineCore

namespace Graphics {
namespace Renderer {
class SceneRenderer;
} // namespace Renderer

namespace Proxy {
class UiCanvasSceneProxy;

class UiSceneProxyBase {
protected:
    std::weak_ptr<Graphics::Renderer::SceneRenderer> mSceneRendererWp;

    size_t mUiItemUId;

    bool mIsVisible;

    // Key for render sorting: [layer, local z of root, ..., local z of this widget]. Compared lexicographically — parent (prefix)
    // is always before child.
    std::vector<int32_t> mZPath;

    std::weak_ptr<UiCanvasSceneProxy> mParentCanvasProxy;

    glm::vec2 mNormalizedTranslation, mNormalizedScale;

    glm::ivec2 mWidthHeightPixels;

    float mOverlayOpacity{1.0f};

    glm::vec2 mScale;

    glm::vec2 mCenterOffset;

    const std::string mName;

    bool mIsGuiScissorsSlave;

    bool mIsGuiScissorsMaster;

    bool mCanBloomBeApplied;

public:
    UiSceneProxyBase(const ::EngineCore::GUI::UiItemBase* uiItemBase);

    virtual ~UiSceneProxyBase() = default;

    void SetSceneRenderer(const std::weak_ptr<Graphics::Renderer::SceneRenderer>& sceneRendererWp);

    void SetUiItemUid(const size_t UId);

    size_t GetUiItemUId() const;

    void SetIsVisible(const bool isVisible);

    bool IsVisible() const;

    void SetZPath(const std::vector<int32_t>& zPath);

    const std::vector<int32_t>& GetZPath() const;

    void SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale);

    void SetWidthHeightPixels(const glm::ivec2& widthHeight);

    void SetCanvasSceneProxy(const std::weak_ptr<UiCanvasSceneProxy>& parentCanvasProxy);

    void SetOverlayOpacity(const float overlayOpacity);

    float GetOverlayOpacity() const;

    void SetScale(const glm::vec2& scale);

    glm::vec2 GetScale() const;

    void SetCenterOffset(const glm::vec2& offset);

    void SetIsGuiScissorsSlave(const bool isScissorsSlave);

    bool IsGuiScissorsSlave() const;

    void SetIsGuiScissorsMaster(const bool isScissorsMaster);

    bool IsGuiScissorsMaster() const;

    bool CanBloomBeApplied() const;

    void SetCanBloomBeApplied(const bool canBloomBeApplied);

    glm::vec2 GetCenterOffset() const;

    glm::vec2 GetNormalizedWidthHeight() const;

    virtual void OnSceneProxyRegistered() = 0;

    virtual void Render() = 0;

    virtual void CleanUp() = 0;
};
} // namespace Proxy
} // namespace Graphics