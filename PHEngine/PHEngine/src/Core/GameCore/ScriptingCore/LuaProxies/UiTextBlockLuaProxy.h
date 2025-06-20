#pragma once

#include "UiRectangleLuaProxy.h"

#include <glm/vec3.hpp>

namespace EngineCore {
namespace GUI {
class UiTextBlock;
}
} // namespace EngineCore

namespace EngineCore {
namespace Scripts {
class UiTextBlockLuaProxy : public UiRectangleLuaProxy {
protected:
public:
    explicit UiTextBlockLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiTextBlock>& ownerTextBlock);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;
};
} // namespace Scripts
} // namespace EngineCore
