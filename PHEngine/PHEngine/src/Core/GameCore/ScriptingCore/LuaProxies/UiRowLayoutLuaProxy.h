#pragma once

#include "UiItemLuaProxy.h"

namespace EngineCore
{
    namespace GUI
    {
        class UiRowLayout;
    }
}

namespace EngineCore
{
    namespace Scripts
    {
        class UiRowLayoutLuaProxy
            : public UiItemLuaProxy
        {
        protected:
            uint32_t mSpacing{0};

        public:
            explicit UiRowLayoutLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiRowLayout> &ownerUiItem);

            void OnLuaThreadDataUpdated(const std::string &jsonParameters) override;

            std::string GetGameThreadData() override;

            void SetSpacing_FromGameThread(const uint32_t value);
        };
    }
}
