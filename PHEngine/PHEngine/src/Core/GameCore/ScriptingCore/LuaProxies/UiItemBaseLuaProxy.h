#pragma once

#include "LuaProxy.h"
#include "UiAnchorLuaData.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/UiAnchorType.h"

#include <string>
#include <unordered_map>

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

            std::unordered_map<eUiAnchor /*src anchor*/, UiAnchorLuaData> mAnchors;

            int32_t mHorizontalCenterOffset;
            
            int32_t mVerticalCenterOffset;

        public:
            explicit UiItemBaseLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiItemBase> &ownerUiItemBase);

            std::string GetUiItemName() const;

            void SetIsVisible_FromGameThread(const bool isVisible);

            void SetZOrder_FromGameThread(const size_t zOrder);

            void SetWidth_FromGameThread(const size_t width);

            void SetHeight_FromGameThread(const size_t height);

            void SetAnchor_FromGameThread(const eUiAnchor srcAnchor, const eUiAnchor &dstAnchor, const int32_t dstUiLuaProxyId);

            void SetHorizontalCenterOffset_FromGameThread(const int32_t horizontalCenterOffset);

            void SetVerticalCenterOffset_FromGameThread(const int32_t verticalCenterOffset);

            void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

            std::string GetGameThreadData() override;

            bool IsVisible() const;
        };
    }
}
