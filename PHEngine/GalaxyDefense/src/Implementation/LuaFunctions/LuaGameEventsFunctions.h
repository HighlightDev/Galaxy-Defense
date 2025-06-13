#pragma once

#include "Core/GameCore/ScriptingCore/LuaFunctions/ILuaFunctionable.h"
#include "Implementation/Events/ChangeGameModeEvent.h"
#include "Implementation/Events/LevelAreaBBChangedEvent.h"
#include "Implementation/Events/LevelProgressChangedEvent.h"
#include "Implementation/Events/MainPlayerStatusChangedEvent.h"

using namespace Event;
using namespace EngineCore::Scripts;

namespace EngineCore {
namespace Scripts {
class LuaScriptExecutorBase;
class LuaScriptProcessor;
} // namespace Scripts
} // namespace EngineCore

namespace Game {
class LuaGameEventsFunctions : public ILuaFunctionable,
                               public LuaMainPlayerStatusChangedEvent,
                               public LuaLevelProgressChangedEvent,
                               public LuaChangeGameModeEvent,
                               public LevelAreaBBChangedLuaThreadEvent,
                               public std::enable_shared_from_this<LuaGameEventsFunctions> {
protected:
    ::EngineCore::Scripts::LuaScriptExecutorBase* mOwnerPtr;

    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> mLuaScriptProcessor;

public:
    LuaGameEventsFunctions(::EngineCore::Scripts::LuaScriptExecutorBase* ownerPtr);

    ~LuaGameEventsFunctions();

    void Initialize() override;

    void SetScene(const std::weak_ptr<::EngineCore::Scene>& sceneWp) override;

    void SetLuaScriptProcessor(const std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor>& scriptProcessor) override;

    void OnScriptStarted(const LuaWrapper& luaWrapper) override;

    void OnScriptStopped(const LuaWrapper& luaWrapper) override;

    void RegisterCallbacks(const LuaWrapper& luaWrapper) override;

    void ProcessEvent(
        const LuaMainPlayerStatusChangedEvent* sender, const LuaMainPlayerStatusChangedEvent::EventData_t& data) override;

    void ProcessEvent(const LuaLevelProgressChangedEvent* sender, const LuaLevelProgressChangedEvent::EventData_t& data) override;

    void ProcessEvent(const LuaChangeGameModeEvent* sender, const LuaChangeGameModeEvent::EventData_t& data) override;

    void ProcessEvent(
        const LevelAreaBBChangedLuaThreadEvent* sender, const LevelAreaBBChangedLuaThreadEvent::EventData_t& data) override;

private:
    int32_t GetSelectedMissileType(const std::tuple<>& data) const;

    std::string GetAllMissilesData(const std::tuple<>& data) const;

    int32_t GetEnemySpaceshipsCountDestroyedByPlayer(const std::tuple<>& data) const;

    float GetEditorLevelAreaBoundingBoxWidth(const std::tuple<>& data) const;

    float GetEditorLevelAreaBoundingBoxLength(const std::tuple<>& data) const;

    void SendChangeGameModeGameThreadEvent(const std::tuple<int32_t /*enqueue policy*/, int32_t /*game mode type*/>& data);

    void SendChangeEditModeGameThreadEvent(const std::tuple<int32_t /*enqueue policy*/, int32_t /*edit mode type*/>& data);
};
} // namespace Game
