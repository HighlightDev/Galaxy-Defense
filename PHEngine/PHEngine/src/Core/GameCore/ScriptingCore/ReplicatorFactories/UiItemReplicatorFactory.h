#pragma once

#include <cstdint>
#include <stdint.h>
#include <memory>

#include "IReplicatorFactory.h"

namespace EngineCore
{
    class Scene;
}

namespace EngineCore
{
    namespace Scripts
    {
        class LuaScriptProcessor;

        class UiItemReplicatorFactory
            : public IReplicatorFactory
        {
        public:
            int32_t CreateReplicator(const std::weak_ptr<::EngineCore::Scene> &sceneWp,
                                     const std::weak_ptr<LuaScriptProcessor> &luaScriptProcessorWp,
                                     const std::string& jsonParamsStr) const override;
        };
    }
} // namespace EngineCore