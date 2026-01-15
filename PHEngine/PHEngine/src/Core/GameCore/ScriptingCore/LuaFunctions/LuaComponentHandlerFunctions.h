#pragma once
#include "Core/GameCore/Scene.h"
#include "ILuaFunctionable.h"

#include <memory>
#include <unordered_map>

using namespace EngineCore;

namespace EngineCore {
namespace Scripts {
class LuaScriptExecutorBase;
class LuaScriptProcessor;

class LuaComponentHandlerFunctions : public ILuaFunctionable {
    LuaScriptExecutorBase* mOwnerPtr;

    std::weak_ptr<Scene> mSceneWp;

    std::weak_ptr<LuaScriptProcessor> mLuaScriptProcessor;

public:
    LuaComponentHandlerFunctions(LuaScriptExecutorBase* ownerPtr);

    ~LuaComponentHandlerFunctions();

    void Initialize() override;

    void SetScene(const std::weak_ptr<Scene>& sceneWp) override;

    void SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& scriptProcessor) override;

    void OnScriptStarted(const LuaWrapper& luaWrapper) override;

    void OnScriptStopped(const LuaWrapper& luaWrapper) override;

    void RegisterCallbacks(const LuaWrapper& luaWrapper) override;

private:
    int32_t GetLuaProxyIdForComponent(const std::tuple<std::string /*component name*/>& data);

    bool InvokeFunction(const std::tuple<int32_t, std::string, std::string>& data);
};
} // namespace Scripts
} // namespace EngineCore
