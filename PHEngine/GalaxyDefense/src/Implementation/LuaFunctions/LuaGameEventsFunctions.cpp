#include "LuaGameEventsFunctions.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Implementation/DataProviders/LevelDataProvider.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"
#include "Implementation/EditModeTypeEnum.h"
#include "Implementation/Events/ChangeEditModeEvent.h"
#include "Implementation/Events/ChangeGameModeEvent.h"
#include "Implementation/GameModeTypeEnum.h"
#include "Implementation/MissileType.h"
#include "Implementation/StatusTypes.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace Game {
LuaGameEventsFunctions::LuaGameEventsFunctions(LuaScriptExecutorBase* ownerPtr)
    : mOwnerPtr(ownerPtr)
{
}

LuaGameEventsFunctions::~LuaGameEventsFunctions()
{
    LuaMainPlayerStatusChangedEvent::GetInstance()->RemoveListener(LuaMainPlayerStatusChangedEvent::GetInstanceId());
    LuaLevelProgressChangedEvent::GetInstance()->RemoveListener(LuaLevelProgressChangedEvent::GetInstanceId());
    LuaChangeGameModeEvent::GetInstance()->RemoveListener(LuaChangeGameModeEvent::GetInstanceId());
}

void LuaGameEventsFunctions::Initialize()
{
    LuaMainPlayerStatusChangedEvent::GetInstance()->AddListener(shared_from_this());
    LuaLevelProgressChangedEvent::GetInstance()->AddListener(shared_from_this());
    LuaChangeGameModeEvent::GetInstance()->AddListener(shared_from_this());
}

void LuaGameEventsFunctions::SetScene(const std::weak_ptr<Scene>& sceneWp)
{
    mSceneWp = sceneWp;
}

void LuaGameEventsFunctions::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& scriptProcessor)
{
    mLuaScriptProcessor = scriptProcessor;
}

void LuaGameEventsFunctions::OnScriptStarted(const LuaWrapper& luaWrapper)
{
}

void LuaGameEventsFunctions::OnScriptStopped(const LuaWrapper& luaWrapper)
{
}

void LuaGameEventsFunctions::RegisterCallbacks(const LuaWrapper& luaWrapper)
{
    LuaCallbackBindingHelper<Hash64_CT("LuaGameEventsFunctions::GetSelectedMissileType"), int32_t(void)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaGameEventsFunctions::GetSelectedMissileType, this, std::placeholders::_1),
        "_GetSelectedMissileType");
    LuaCallbackBindingHelper<Hash64_CT("LuaGameEventsFunctions::GetAllMissilesData"), std::string(void)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaGameEventsFunctions::GetAllMissilesData, this, std::placeholders::_1),
        "_GetAllMissilesData");
    LuaCallbackBindingHelper<Hash64_CT("LuaGameEventsFunctions::GetEnemySpaceshipsCountDestroyedByPlayer"), int32_t(void)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaGameEventsFunctions::GetEnemySpaceshipsCountDestroyedByPlayer, this, std::placeholders::_1),
        "_GetEnemySpaceshipsCountDestroyedByPlayer");
    LuaCallbackBindingHelper<Hash64_CT("LuaGameEventsFunctions::GetEditorLevelAreaBoundingBoxWidth"), float(void)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaGameEventsFunctions::GetEditorLevelAreaBoundingBoxWidth, this, std::placeholders::_1),
        "_GetEditorLevelAreaBoundingBoxWidth");
    LuaCallbackBindingHelper<Hash64_CT("LuaGameEventsFunctions::GetEditorLevelAreaBoundingBoxLength"), float(void)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaGameEventsFunctions::GetEditorLevelAreaBoundingBoxLength, this, std::placeholders::_1),
        "_GetEditorLevelAreaBoundingBoxLength");
    LuaCallbackBindingHelper<Hash64_CT("LuaGameEventsFunctions::SendChangeGameModeGameThreadEvent"), void(int32_t, int32_t)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaGameEventsFunctions::SendChangeGameModeGameThreadEvent, this, std::placeholders::_1),
            "_SendChangeGameModeGameThreadEvent");
    LuaCallbackBindingHelper<Hash64_CT("LuaGameEventsFunctions::SendChangeEditModeGameThreadEvent"), void(int32_t, int32_t)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaGameEventsFunctions::SendChangeEditModeGameThreadEvent, this, std::placeholders::_1),
            "_SendChangeEditModeGameThreadEvent");
}

void LuaGameEventsFunctions::ProcessEvent(
    const LuaMainPlayerStatusChangedEvent* sender, const LuaMainPlayerStatusChangedEvent::EventData_t& data)
{
    LuaFunctionInvoker<void(void*, std::string, std::string)>::Invoke(
        mOwnerPtr->GetLuaInstance(),
        "System_OnGameEventTriggered",
        (void*)mOwnerPtr,
        std::string("PlayerStatusChanged"),
        std::get<1>(data));
}

void LuaGameEventsFunctions::ProcessEvent(
    const LuaLevelProgressChangedEvent* sender, const LuaLevelProgressChangedEvent::EventData_t& data)
{
    LuaFunctionInvoker<void(void*, std::string, std::string)>::Invoke(
        mOwnerPtr->GetLuaInstance(),
        "System_OnGameEventTriggered",
        (void*)mOwnerPtr,
        std::string("LevelProgressChanged"),
        std::get<1>(data));
}

void LuaGameEventsFunctions::ProcessEvent(const LuaChangeGameModeEvent* sender, const LuaChangeGameModeEvent::EventData_t& data)
{
    nlohmann::json gameModeJson;
    gameModeJson["game_mode_type"] = std::to_string(static_cast<int32_t>(std::get<0>(data)));

    LuaFunctionInvoker<void(void*, std::string, int32_t)>::Invoke(
        mOwnerPtr->GetLuaInstance(),
        "System_OnGameEventTriggered",
        (void*)mOwnerPtr,
        std::string("GameModeChanged"),
        gameModeJson.dump());
}

void LuaGameEventsFunctions::ProcessEvent(
    const LevelAreaBBChangedLuaThreadEvent* sender, const LevelAreaBBChangedLuaThreadEvent::EventData_t& data)
{
    const auto& levelBb2d = std::get<0>(data);
    const auto& halfExtent = levelBb2d.GetHalfExtent();
    const auto width = halfExtent.x * 2.0f;
    const auto length = halfExtent.y * 2.0f;

    nlohmann::json jsonObj;
    jsonObj["width"] = width;
    jsonObj["lenght"] = length;
    LuaFunctionInvoker<void(void*, std::string, int32_t)>::Invoke(
        mOwnerPtr->GetLuaInstance(),
        "System_OnGameEventTriggered",
        (void*)mOwnerPtr,
        std::string("EditLevelAreaBoundingBoxChanged"),
        jsonObj.dump());
}

int32_t LuaGameEventsFunctions::GetSelectedMissileType(const std::tuple<>& data) const
{
    return static_cast<int32_t>(PlayerDataProvider::GetInstance()->GetSelectedMissileType());
}

std::string LuaGameEventsFunctions::GetAllMissilesData(const std::tuple<>& data) const
{
    const auto& dataProvider = PlayerDataProvider::GetInstance();
    std::unordered_map<eMissileType, size_t> allMissilesData;
    const std::vector<eMissileType> availableMissileTypes = dataProvider->GetAvailableMissileTypes();
    for (const auto availableMissileType : availableMissileTypes) {
        allMissilesData[static_cast<eMissileType>(availableMissileType)]
            = dataProvider->GetMissilesCount(static_cast<eMissileType>(availableMissileType));
    }
    nlohmann::json jsonObj;
    jsonObj["all_missiles_data"] = allMissilesData;
    return jsonObj.dump();
}

int32_t LuaGameEventsFunctions::GetEnemySpaceshipsCountDestroyedByPlayer(const std::tuple<>& data) const
{
    return static_cast<int32_t>(PlayerDataProvider::GetInstance()->GetDestroyedEnemySpaceshipsCount());
}

float LuaGameEventsFunctions::GetEditorLevelAreaBoundingBoxWidth(const std::tuple<>& data) const
{
    return LevelDataProvider::GetInstance()->GetEditorLevelAreaBoundingBox().GetHalfExtent().x * 2.0f;
}

float LuaGameEventsFunctions::GetEditorLevelAreaBoundingBoxLength(const std::tuple<>& data) const
{
    return LevelDataProvider::GetInstance()->GetEditorLevelAreaBoundingBox().GetHalfExtent().y * 2.0f;
}

void LuaGameEventsFunctions::SendChangeGameModeGameThreadEvent(
    const std::tuple<int32_t /*enqueue policy*/, int32_t /*game mode type*/>& data)
{
    const auto enqueuePolicy = std::get<0>(data);
    const auto gameModeType = std::get<1>(data);

    if (const auto& sceneSp = mSceneWp.lock()) {
        static constexpr auto functionId = Hash64_CT("LuaGameEventsFunctions::SendChangeGameModeGameThreadEvent");
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            static_cast<eEnqueueJobPolicy>(enqueuePolicy),
            0,
            functionId,
            [gameModeType](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                ChangeGameModeEvent::GetInstance()->SendEvent(
                    eExecutionOrder::POST_EXECUTION, static_cast<eGameModeType>(gameModeType));
            });
    }

    LuaChangeGameModeEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, static_cast<eGameModeType>(gameModeType));
}

void LuaGameEventsFunctions::SendChangeEditModeGameThreadEvent(
    const std::tuple<int32_t /*enqueue policy*/, int32_t /*edit mode type*/>& data)
{
    const auto enqueuePolicy = std::get<0>(data);
    const auto editModeType = std::get<1>(data);

    if (const auto& sceneSp = mSceneWp.lock()) {
        static constexpr auto functionId = Hash64_CT("LuaGameEventsFunctions::SendChangeEditModeGameThreadEvent");
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            static_cast<eEnqueueJobPolicy>(enqueuePolicy),
            0,
            functionId,
            [editModeType](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                ChangeEditModeEvent::GetInstance()->SendEvent(
                    eExecutionOrder::POST_EXECUTION, static_cast<eEditModeType>(editModeType));
            });
    }
}
} // namespace Game
