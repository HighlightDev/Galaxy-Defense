#pragma once
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaPlatformTraverseComponentFunctions.h"
#include "LuaScriptExecutorBase.h"

#include <memory>

namespace EngineCore {
class PlatformTraverseComponent;
}

namespace EngineCore {
namespace Scripts {
class LuaPlatformTraverseScriptExecutor : public LuaScriptExecutorBase {
    std::unique_ptr<LuaPlatformTraverseComponentFunctions> mPlatformTraverseComponentFunctions;

public:
    explicit LuaPlatformTraverseScriptExecutor(const std::string& scriptName, ::EngineCore::PlatformTraverseComponent* owner);

    void RunScript() override;

    void StopScript() override;

    void RegisterCallbacks() override;
};
} // namespace Scripts
} // namespace EngineCore