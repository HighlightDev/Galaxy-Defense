#pragma once

#include "Core/GameCore/ScriptingCore/ReplicatorFactories/IReplicatorFactory.h"

#include <stdint.h>

#include <cstdint>
#include <memory>

namespace EngineCore {
class Scene;
namespace Scripts {
class LuaScriptProcessor;
} // namespace Scripts
} // namespace EngineCore

namespace Game {

class UiConnectionLineReplicatorFactory : public ::EngineCore::Scripts::IReplicatorFactory {
public:
    int32_t CreateReplicator(
        const std::weak_ptr<::EngineCore::Scene>& sceneWp,
        const std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor>& luaScriptProcessorWp,
        const std::string& jsonParamsStr) const override;
};

} // namespace Game
