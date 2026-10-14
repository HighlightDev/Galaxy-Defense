#pragma once

#include "Core/GameCore/GUI/Common/GridAlignment.h"
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

    UiGridHorizontalAlignmentType mHorizontalAlignment{UiGridHorizontalAlignmentType::LEFT};

    UiGridVerticalAlignmentType mVerticalAlignment{UiGridVerticalAlignmentType::TOP};

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

    void
    SetAlignment(const UiGridHorizontalAlignmentType horizontalAlignment, const UiGridVerticalAlignmentType verticalAlignment);

    UiGridHorizontalAlignmentType GetHorizontalAlignment() const;

    UiGridVerticalAlignmentType GetVerticalAlignment() const;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override;

protected:
    void UpdateAnchorTransform() override;

    bool OnPropertiesShouldBeUpdatedOnLuaThread() override;

private:
    void RecalculatePositionsForChildren();

    bool SyncDataOnLuaThread();

    int32_t GetGridPotentialWidth() const;

    int32_t GetGridPotentialHeight() const;

    int32_t GetRowHeight(const uint32_t rowIndex) const;
};
} // namespace GUI
} // namespace EngineCore