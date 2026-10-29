#include "LuaPlayerDataProviderFunctions.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Game/Constants/GameConstants.h"
#include "Game/DataProviders/PlayerDataProvider.h"

#include <nlohmann/json.hpp>

using namespace EngineCore;

namespace Game {
LuaPlayerDataProviderFunctions::LuaPlayerDataProviderFunctions(LuaScriptExecutorBase* ownerPtr)
    : mOwnerPtr(ownerPtr)
{
}

LuaPlayerDataProviderFunctions::~LuaPlayerDataProviderFunctions()
{
    LuaMainPlayerStatusChangedEvent::GetInstance()->RemoveListener(LuaMainPlayerStatusChangedEvent::GetInstanceId());
}

void LuaPlayerDataProviderFunctions::Initialize()
{
    LuaMainPlayerStatusChangedEvent::GetInstance()->AddListener(
        std::static_pointer_cast<LuaMainPlayerStatusChangedEvent>(shared_from_this()));
}

void LuaPlayerDataProviderFunctions::SetScene(const std::weak_ptr<Scene>& sceneWp)
{
    mSceneWp = sceneWp;
}

void LuaPlayerDataProviderFunctions::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& scriptProcessor)
{
    mLuaScriptProcessor = scriptProcessor;
}

void LuaPlayerDataProviderFunctions::OnScriptStarted(const LuaWrapper& luaWrapper)
{
}

void LuaPlayerDataProviderFunctions::OnScriptStopped(const LuaWrapper& luaWrapper)
{
}

void LuaPlayerDataProviderFunctions::RegisterCallbacks(const LuaWrapper& luaWrapper)
{
    LuaCallbackBindingHelper<Hash64_CT("LuaPlayerDataProviderFunctions::GetAvailableMissileTypes"), std::string()>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaPlayerDataProviderFunctions::GetAvailableMissileTypes, this, std::placeholders::_1),
        "_GetAvailableMissileTypes");

    LuaCallbackBindingHelper<Hash64_CT("LuaPlayerDataProviderFunctions::GetCollectedCrystalsCount"), int32_t(void)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaPlayerDataProviderFunctions::GetCollectedCrystalsCount, this, std::placeholders::_1),
        "_GetCollectedCrystalsCount");

    LuaCallbackBindingHelper<Hash64_CT("LuaPlayerDataProviderFunctions::GetMissilePrices"), std::string()>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaPlayerDataProviderFunctions::GetMissilePrices, this, std::placeholders::_1),
        "_GetMissilePrices");
    LuaCallbackBindingHelper<Hash64_CT("LuaPlayerDataProviderFunctions::GetBarrierPerCellPrice"), int32_t()>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaPlayerDataProviderFunctions::GetBarrierPerCellPrice, this, std::placeholders::_1),
        "_GetBarrierPerCellPrice");
}

std::string LuaPlayerDataProviderFunctions::GetAvailableMissileTypes(const std::tuple<>& data) const
{
    const auto& avlMissiles = PlayerDataProvider::GetInstance()->GetAvailableMissileTypes();
    nlohmann::json jsonObj;
    jsonObj["all_missiles_data"] = avlMissiles;
    return jsonObj.dump();
}

int32_t LuaPlayerDataProviderFunctions::GetCollectedCrystalsCount(const std::tuple<>& data) const
{
    return PlayerDataProvider::GetInstance()->GetCrystalsCount();
}

std::string LuaPlayerDataProviderFunctions::GetMissilePrices(const std::tuple<>& data) const
{
    nlohmann::json obj;
    obj["missile_prices"] = Game::Constants::c_missileTypePrices;
    return obj.dump();
}

int32_t LuaPlayerDataProviderFunctions::GetBarrierPerCellPrice(const std::tuple<>& data) const
{
    return (int32_t)Game::Constants::c_barrierPerCellPrice;
}

void LuaPlayerDataProviderFunctions::ProcessEvent(
    const LuaMainPlayerStatusChangedEvent* sender, const LuaMainPlayerStatusChangedEvent::EventData_t& data)
{
    if (mOwnerPtr->IsLuaStateReady()) {
        LuaFunctionInvoker<void(void*, std::string, std::string)>::Invoke(
            mOwnerPtr->GetLuaInstance(),
            "System_OnGameEventTriggered",
            (void*)mOwnerPtr,
            std::string("PlayerStatusChanged"),
            std::get<0>(data));
    }
}

} // namespace Game
