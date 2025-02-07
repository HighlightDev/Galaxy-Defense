#pragma once

#include "Core/GameCore/ScriptingCore/LuaFunctions/ILuaFunctionable.h"

using namespace EngineCore::Scripts;

namespace EngineCore {
namespace Scripts {
class LuaScriptExecutorBase;
class LuaScriptProcessor;
} // namespace Scripts
} // namespace EngineCore

namespace Game {
class LevelProgressController;

class LuaLvlProgressFunctions : public ILuaFunctionable, public std::enable_shared_from_this<LuaLvlProgressFunctions> {
protected:
    ::EngineCore::Scripts::LuaScriptExecutorBase* mOwnerPtr;

    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> mLuaScriptProcessor;

    std::shared_ptr<::Game::LevelProgressController> mLevelProgressController;

public:
    LuaLvlProgressFunctions(
        ::EngineCore::Scripts::LuaScriptExecutorBase* ownerPtr,
        const std::shared_ptr<::Game::LevelProgressController>& lvlProgressController);

    ~LuaLvlProgressFunctions();

    void Initialize() override;

    void SetScene(const std::weak_ptr<::EngineCore::Scene>& sceneWp) override;

    void SetLuaScriptProcessor(const std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor>& scriptProcessor) override;

    void OnScriptStarted(const LuaWrapper& luaWrapper) override;

    void OnScriptStopped(const LuaWrapper& luaWrapper) override;

    void RegisterCallbacks(const LuaWrapper& luaWrapper) override;

private:
    void SetLevelProgressStagesQueue(const std::tuple<std::string /*level progress stages queue json*/>& data);

    int32_t GetCurrentProgressRequirementsCount(const std::tuple<>& data) const;

    std::string GetCurrentProgressStageName(const std::tuple<>& data) const;

    std::string GetCurrentProgressStageRequirementTrackers(const std::tuple<>& data);
};
} // namespace Game
