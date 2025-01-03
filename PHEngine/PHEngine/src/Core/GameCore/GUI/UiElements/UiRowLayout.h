#pragma once

#include "UiItem.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/mat4x4.hpp>

namespace EngineCore
{
    namespace Scripts
    {
        class LuaProxy;
    }
}

namespace EngineCore
{
    namespace GUI
    {
        class UiCanvas;

        class UiRowLayout : public UiItem
        {
            uint32_t mSpacing{0};

        public:
            UiRowLayout(const std::string &name = std::string(""));

            std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

            std::string GetUiTypeString() const override;

            void SetSpacing(const uint32_t spacing);

            uint32_t GetSpacing() const;

            void SyncFromLuaJsonProperties(const std::string &luaJsonPropsStr) override;

        protected:
            void UpdateAnchorTransform() override;

            void OnPropertiesShouldBeUpdatedOnLuaThread() override;

        private:
            void RecalculatePositionsForChildren();

            void SyncDataOnLuaThread();
        };
    }
}