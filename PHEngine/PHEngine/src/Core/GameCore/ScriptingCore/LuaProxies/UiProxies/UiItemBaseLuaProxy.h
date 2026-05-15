#pragma once

#include "Core/GameCore/GUI/UiElements/Transform2D/UiAnchorData.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/UiAnchorType.h"
#include "Core/GameCore/ScriptingCore/Common/LuaMouseInputState.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/LuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiProxies/IAnimatableLuaProxy.h"

#include <string>
#include <tuple>
#include <unordered_map>

namespace EngineCore {
namespace GUI {
class UiItemBase;
class AnimationData;
class AnimationSequence;
} // namespace GUI
} // namespace EngineCore

using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {
class UiItemBaseLuaProxy : public LuaProxy, public IAnimatableLuaProxy {
    std::string mUiItemName;

    bool mIsVisible;

    bool mCanInterceptMouseInputEvents;

    size_t mZOrder;

    size_t mWidth;

    size_t mHeight;

    std::unordered_map<eUiAnchor /*src anchor*/, UiAnchorData> mAnchors;

    int32_t mHorizontalCenterOffset;

    int32_t mVerticalCenterOffset;

protected:
    bool mIsMouseInputDataDirty{false};

    bool mInputClicked{false};

    eLuaMouseInputPressState mInputPressState{eLuaMouseInputPressState::MOUSE_BUTTON_RELEASED};

    eLuaMouseInputCursorHoverState mInputCursorHoverState{eLuaMouseInputCursorHoverState::CURSOR_HOVER_LEAVED};

    bool mIsMouseInputReceiverEnabled{false};

    bool mIsGuiScissorsSlave{false};

    bool mIsGuiScissorsMaster{false};

public:
    explicit UiItemBaseLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiItemBase>& ownerUiItemBase);

    std::string GetUiItemName() const;

    void SetParent(const std::string& canvasName, const std::string& parentUiName);

    void SetIsVisible_FromGameThread(const bool isVisible);

    void SetIfCanInterceptMouseInputEvents_FromGameThread(const bool intercepts);

    void SetZOrder_FromGameThread(const size_t zOrder);

    void SetWidth_FromGameThread(const size_t width);

    void SetHeight_FromGameThread(const size_t height);

    void SetAnchor_FromGameThread(const eUiAnchor srcAnchor, const UiAnchorData& uiAnchorData);

    void SetHorizontalCenterOffset_FromGameThread(const int32_t horizontalCenterOffset);

    void SetVerticalCenterOffset_FromGameThread(const int32_t verticalCenterOffset);

    void SetInputClicked_FromGameThread(const bool isClicked);

    void SetInputPressState_FromGameThread(const eLuaMouseInputPressState cursorHoverState);

    void SetInputCursorHoverState_FromGameThread(const eLuaMouseInputCursorHoverState pressState);

    void SetIsGuiScissorsSlave_FromGameThread(const bool isScissorsSlave);

    void SetIsGuiScissorsMaster_FromGameThread(const bool isScissorsMaster);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    std::string GetMouseInputData();

    bool IsVisible() const;

    bool GetIfCanInterceptMouseInputEvents() const;

    void EnableMouseInputReceiverBase();

    bool IsMouseInputDataDirty() const;

    void AddAnimation(const std::string& animationName, const ::EngineCore::GUI::AnimationData& animationData) override;

    void StartAnimation(const std::string& animationName) override;

    void AddSequenceAnimation(
        const std::string& animationName, const ::EngineCore::GUI::AnimationSequence& animationSequence) override;

    void StartSequenceAnimation(const std::string& animationSequenceName) override;

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override
    {
        // No functions to invoke for now
        return false;
    }
};
} // namespace Scripts
} // namespace EngineCore
