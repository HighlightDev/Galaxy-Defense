#pragma once

#include "UiItemBase.h"

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

        class UiItem : public UiItemBase
        {
        public:
            explicit UiItem(const std::weak_ptr<UiCanvas> &parentCanvas, const std::weak_ptr<IUiTransformable> &parent);

            void OnRegistered() override;
            
            void OnUnregistered() override;

            std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;
        };
    }
}