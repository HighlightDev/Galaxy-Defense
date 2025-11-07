#pragma once

#include "Transform2D/UiRowAlignmentType.h"
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

class UiRowLayout : public UiItem {
    uint32_t mSpacing{0};

    eUiRowAlignmentType mAlignmentType{eUiRowAlignmentType::LEFT};

public:
    UiRowLayout(const std::string& name = std::string(""));

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    std::string GetUiTypeString() const override;

    void SetSpacing(const uint32_t spacing);

    uint32_t GetSpacing() const;

    void SetAlignment(const eUiRowAlignmentType alignmentType);

    eUiRowAlignmentType GetAlignment() const;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    void UnpausableTick(const float deltaTimeSec) override;

protected:
    void UpdateAnchorTransform() override;

    void OnPropertiesShouldBeUpdatedOnLuaThread() override;

private:
    void RecalculatePositionsForChildren();

    void SyncDataOnLuaThread();
};
} // namespace GUI
} // namespace EngineCore