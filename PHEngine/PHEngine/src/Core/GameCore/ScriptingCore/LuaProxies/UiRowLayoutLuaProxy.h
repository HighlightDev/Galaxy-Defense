#pragma once

#include "Core/GameCore/GUI/UiElements/Transform2D/UiRowAlignmentType.h"
#include "UiItemLuaProxy.h"

namespace EngineCore {
namespace GUI {
class UiRowLayout;
}
} // namespace EngineCore

using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {
class UiRowLayoutLuaProxy : public UiItemLuaProxy {
protected:
    uint32_t mSpacing{0};

    eUiRowAlignmentType mAlignmentType;

public:
    explicit UiRowLayoutLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiRowLayout>& ownerUiItem);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetSpacing_FromGameThread(const uint32_t value);

    void SetAlignment_FromGameThread(const eUiRowAlignmentType alignmentType);
};
} // namespace Scripts
} // namespace EngineCore
