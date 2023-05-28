#pragma once

#include "UiItemBaseLuaProxy.h"

namespace EngineCore
{
    namespace GUI
    {
        class UiItem;
    }
}

namespace EngineCore
{
    namespace Scripts
    {
        class UiItemLuaProxy
            : public UiItemBaseLuaProxy
        {
        public:
            explicit UiItemLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiItem> &ownerUiItem);

            bool IsAnimationSupported() const override;
        };
    }
}
