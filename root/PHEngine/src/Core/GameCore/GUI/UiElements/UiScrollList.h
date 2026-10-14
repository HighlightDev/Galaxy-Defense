#pragma once

#include "Core/GameCore/Input/MouseEventEnums.h"
#include "UiItem.h"

#include <glm/vec3.hpp>

#include <cstdint>
#include <memory>

namespace EngineCore {
namespace Scripts {
class LuaProxy;
}
} // namespace EngineCore

namespace Graphics {
namespace Proxy {
class UiSceneProxyBase;
}
} // namespace Graphics

namespace EngineCore {
namespace GUI {

class UiScrollListScrollHelper;

enum class eScrollbarSide : uint8_t { NONE = 0, LEFT = 1, RIGHT = 2 };

class UiScrollList : public UiItem {
    friend class UiScrollListScrollHelper;

    int32_t mScrollOffset{0};

    int32_t mScrollSpeed{30};

    uint32_t mSpacing{0};

    bool mIsHovered{false};

    eScrollbarSide mScrollbarSide{eScrollbarSide::NONE};

    glm::vec3 mScrollbarBackgroundColor{0.2f, 0.2f, 0.2f};

    glm::vec3 mScrollbarThumbColor{0.6f, 0.6f, 0.6f};

    uint32_t mScrollbarThicknessPixels{8};

    std::shared_ptr<UiScrollListScrollHelper> mScrollHelper;

public:
    explicit UiScrollList(const std::string& name = std::string(""));

    ~UiScrollList() override;

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const override;

    std::string GetUiTypeString() const override;

    void SetScrollOffset(const int32_t offset);

    int32_t GetScrollOffset() const;

    int32_t GetMaxScrollOffset() const;

    void SetScrollSpeed(const int32_t speed);

    int32_t GetScrollSpeed() const;

    void SetSpacing(const uint32_t spacing);

    uint32_t GetSpacing() const;

    void SetScrollbarSide(const eScrollbarSide side);

    eScrollbarSide GetScrollbarSide() const;

    void SetScrollbarBackgroundColor(const glm::vec3& color);

    glm::vec3 GetScrollbarBackgroundColor() const;

    void SetScrollbarThumbColor(const glm::vec3& color);

    glm::vec3 GetScrollbarThumbColor() const;

    void SetScrollbarThicknessPixels(const uint32_t thicknessPixels);

    uint32_t GetScrollbarThicknessPixels() const;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override;

    void RemoveUiItem(const std::shared_ptr<UiItemBase>& uiItem) override;

protected:
    void OnRegistered() override;

    void OnUnregistered() override;

    void UpdateAnchorTransform() override;

    bool OnPropertiesShouldBeUpdatedOnLuaThread() override;

    bool OnPropertiesShouldBeUpdatedOnRenderThread() override;

private:
    void HandleScroll(const EngineCore::eMouseScrollDirection direction);

    void RecalculatePositionsForChildren();

    void PropagateGuiScissorsToChildren();

    bool SyncDataOnLuaThread();

    bool SyncDataOnRenderThread();
};

} // namespace GUI
} // namespace EngineCore
