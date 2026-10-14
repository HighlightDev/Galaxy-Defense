#pragma once

#include "IReplicatorFactory.h"

#include <stdint.h>

#include <cstdint>
#include <memory>

namespace EngineCore {
class Scene;
}

namespace EngineCore {
namespace Scripts {
class LuaScriptProcessor;

class UiBackgroundOverlayReplicatorFactory : public IReplicatorFactory {
public:
    int32_t CreateReplicator(
        const std::weak_ptr<::EngineCore::Scene>& sceneWp,
        const std::weak_ptr<LuaScriptProcessor>& luaScriptProcessorWp,
        const std::string& jsonParamsStr) const override;
};
} // namespace Scripts
} // namespace EngineCore