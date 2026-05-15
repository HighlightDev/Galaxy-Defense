#pragma once

#include "Core/GameCore/Input/MouseEventEnums.h"
#include "UiItem.h"

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

class UiScrollList : public UiItem {
    friend class UiScrollListScrollHelper;

    int32_t mScrollOffset{0};

    int32_t mScrollSpeed{30};

    uint32_t mSpacing{0};

    bool mIsHovered{false};

    std::shared_ptr<UiScrollListScrollHelper> mScrollHelper;

public:
    explicit UiScrollList(const std::string& name = std::string(""));

    ~UiScrollList() override;

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const override;

    std::string GetUiTypeString() const override;

    void SetScrollOffset(const int32_t offset);

    int32_t GetScrollOffset() const;

    void SetScrollSpeed(const int32_t speed);

    int32_t GetScrollSpeed() const;

    void SetSpacing(const uint32_t spacing);

    uint32_t GetSpacing() const;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    void UnpausableTick(const float deltaTimeSec) override;

    void RemoveUiItem(const std::shared_ptr<UiItemBase>& uiItem) override;

protected:
    void OnRegistered() override;

    void OnUnregistered() override;

    void UpdateAnchorTransform() override;

    void OnPropertiesShouldBeUpdatedOnLuaThread() override;

private:
    void HandleScroll(const EngineCore::eMouseScrollDirection direction);

    int32_t GetMaxScrollOffset() const;

    void RecalculatePositionsForChildren();

    void PropagateGuiScissorsToChildren();

    void SyncDataOnLuaThread();
};

} // namespace GUI
} // namespace EngineCore
