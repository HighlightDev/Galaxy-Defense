#pragma once
#include "LuaScriptExecutorBase.h"

#include <memory>

namespace EngineCore {
namespace Scripts {
class LuaEngineScriptExecutor : public LuaScriptExecutorBase {
public:
    explicit LuaEngineScriptExecutor(const std::string& scriptName);
};
} // namespace Scripts
} // namespace EngineCore