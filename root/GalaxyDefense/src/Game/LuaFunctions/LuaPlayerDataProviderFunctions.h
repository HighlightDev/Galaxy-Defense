#pragma once

#include "Core/GameCore/ScriptingCore/LuaFunctions/ILuaFunctionable.h"
#include "Game/Events/MainPlayerStatusChangedEvent.h"

using namespace EngineCore::Scripts;
using namespace Event;

namespace EngineCore {
namespace Scripts {
class LuaScriptExecutorBase;
class LuaScriptProcessor;
} // namespace Scripts
} // namespace EngineCore

namespace Game {
class LevelProgressController;

class LuaPlayerDataProviderFunctions : public ILuaFunctionable,
                                       public LuaMainPlayerStatusChangedEvent,
                                       public std::enable_shared_from_this<LuaPlayerDataProviderFunctions> {
protected:
    ::EngineCore::Scripts::LuaScriptExecutorBase* mOwnerPtr;

    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> mLuaScriptProcessor;

public:
    LuaPlayerDataProviderFunctions(::EngineCore::Scripts::LuaScriptExecutorBase* ownerPtr);

    ~LuaPlayerDataProviderFunctions();

    void Initialize() override;

    void SetScene(const std::weak_ptr<::EngineCore::Scene>& sceneWp) override;

    void SetLuaScriptProcessor(const std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor>& scriptProcessor) override;

    void OnScriptStarted(const LuaWrapper& luaWrapper) override;

    void OnScriptStopped(const LuaWrapper& luaWrapper) override;

    void RegisterCallbacks(const LuaWrapper& luaWrapper) override;

private:
    std::string GetAvailableMissileTypes(const std::tuple<>& data) const;

    int32_t GetCollectedCrystalsCount(const std::tuple<>& data) const;

    std::string GetMissilePrices(const std::tuple<>& data) const;

    int32_t GetBarrierPerCellPrice(const std::tuple<>& data) const;

    void ProcessEvent(
        const LuaMainPlayerStatusChangedEvent* sender, const LuaMainPlayerStatusChangedEvent::EventData_t& data) override;
};
} // namespace Game
