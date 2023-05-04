#pragma once

#include "LuaProxy.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/UiAnchorType.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/UiAnchorData.h"
#include "Core/GameCore/ScriptingCore/Common/LuaMouseInputState.h"

#include <string>
#include <unordered_map>
#include <tuple>

namespace EngineCore
{
    namespace GUI
    {
        class UiItemBase;
    }
}

using namespace EngineCore::GUI;

namespace EngineCore
{
    namespace Scripts
    {
        class UiItemBaseLuaProxy
            : public LuaProxy
        {
            std::string mUiItemName;

            bool mIsVisible;

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

        public:
            explicit UiItemBaseLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiItemBase> &ownerUiItemBase);

            std::string GetUiItemName() const;

            void SetParent(const std::string &canvasName, const std::string &parentUiName);

            void SetIsVisible_FromGameThread(const bool isVisible);

            void SetZOrder_FromGameThread(const size_t zOrder);

            void SetWidth_FromGameThread(const size_t width);

            void SetHeight_FromGameThread(const size_t height);

            void SetAnchor_FromGameThread(const eUiAnchor srcAnchor, const UiAnchorData &uiAnchorData);

            void SetHorizontalCenterOffset_FromGameThread(const int32_t horizontalCenterOffset);

            void SetVerticalCenterOffset_FromGameThread(const int32_t verticalCenterOffset);

            void SetInputClicked_FromGameThread(const bool isClicked);

            void SetInputPressState_FromGameThread(const eLuaMouseInputPressState cursorHoverState);

            void SetInputCursorHoverState_FromGameThread(const eLuaMouseInputCursorHoverState pressState);

            void OnLuaThreadDataUpdated(const std::string &jsonParameters) override;

            std::string GetGameThreadData() override;

            std::string GetMouseInputData();

            bool IsVisible() const;

            void EnableMouseInputReceiverBase();

            bool IsMouseInputDataDirty() const;
        };
    }
}
