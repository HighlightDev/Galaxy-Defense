#pragma once

#include "UiItem.h"

#include <glm/mat4x4.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

namespace EngineCore {
namespace Scripts {
class LuaProxy;
}
} // namespace EngineCore

namespace EngineCore {
namespace GUI {
class UiCanvas;

class UiGridLayout : public UiItem {
    uint32_t mHorizontalSpacing{0};

    uint32_t mVerticalSpacing{0};

    uint32_t mColumnsCount{1};

    uint32_t mRowsCount{1};

public:
    UiGridLayout(const std::string& name = std::string(""));

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    std::string GetUiTypeString() const override;

    void SetHorizontalSpacing(const uint32_t spacing);

    uint32_t GetHorizontalSpacing() const;

    void SetVerticalSpacing(const uint32_t spacing);

    uint32_t GetVerticalSpacing() const;

    void SetColumnsCount(const uint32_t count);

    uint32_t GetColumnsCount() const;

    void SetRowsCount(const uint32_t count);

    uint32_t GetRowsCount() const;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    void UnpausableTick(const float deltaTimeSec) override;

protected:
    void UpdateAnchorTransform() override;

    void OnPropertiesShouldBeUpdatedOnLuaThread() override;

private:
    void RecalculatePositionsForChildren();

    void SyncDataOnLuaThread();

    int32_t GetGridPotentialWidth() const;

    int32_t GetGridPotentialHeight() const;

    int32_t GetRowHeight(const uint32_t rowIndex) const;
};
} // namespace GUI
} // namespace EngineCore