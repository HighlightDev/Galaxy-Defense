#pragma once

#include "UiMouseInputReceiverBase.h"

#include <glm/vec2.hpp>

#include <functional>
#include <memory>

namespace EngineCore {
namespace GUI {
class UiItemBase;
class UiToggleButton;

class UiMouseInputReceiverToggleButton : public UiMouseInputReceiverBase {
protected:
    std::weak_ptr<UiToggleButton> mOwnerWp;

    std::function<void(std::weak_ptr<UiToggleButton>, bool)> mButtonToggledCallback;

public:
    explicit UiMouseInputReceiverToggleButton(const std::shared_ptr<UiToggleButton>& ownerWp);

    void SetButtonToggledCallback(std::function<void(std::weak_ptr<UiToggleButton>, bool)> callback);

protected:
    void OnMouseClicked(const BoundingBox2D<glm::ivec2>& mouseInputArea, const glm::ivec2& mouseCursorPosition) override;
};
} // namespace GUI
} // namespace EngineCore