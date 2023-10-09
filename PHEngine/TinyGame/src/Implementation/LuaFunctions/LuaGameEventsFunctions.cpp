#include "LuaGameEventsFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Implementation/StatusTypes.h"
#include "Implementation/MissileType.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"

#include <json/json.hpp>

using namespace EngineCore;

namespace Game
{
   LuaGameEventsFunctions::LuaGameEventsFunctions(LuaScriptExecutorBase *ownerPtr)
       : mOwnerPtr(ownerPtr)
   {
   }

   LuaGameEventsFunctions::~LuaGameEventsFunctions()
   {
      LuaMainPlayerStatusChangedEvent::GetInstance()->RemoveListener(LuaMainPlayerStatusChangedEvent::GetInstanceId());
   }

   void LuaGameEventsFunctions::Initialize()
   {
      LuaMainPlayerStatusChangedEvent::GetInstance()->AddListener(shared_from_this());
   }

   void LuaGameEventsFunctions::SetScene(const std::weak_ptr<Scene> &sceneWp)
   {
      mSceneWp = sceneWp;
   }

   void LuaGameEventsFunctions::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor)
   {
      mLuaScriptProcessor = scriptProcessor;
   }

   void LuaGameEventsFunctions::OnScriptStarted(const LuaWrapper &luaWrapper)
   {
   }

   void LuaGameEventsFunctions::OnScriptStopped(const LuaWrapper &luaWrapper)
   {
   }

   void LuaGameEventsFunctions::RegisterCallbacks(const LuaWrapper &luaWrapper)
   {
      LuaCallbackBindingHelper<Hash64_CT("LuaGameEventsFunctions::GetSelectedMissileType"), int32_t(void)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaGameEventsFunctions::GetSelectedMissileType, this, std::placeholders::_1), "_GetSelectedMissileType");
      LuaCallbackBindingHelper<Hash64_CT("LuaGameEventsFunctions::GetAllMissilesData"), std::string(void)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaGameEventsFunctions::GetAllMissilesData, this, std::placeholders::_1), "_GetAllMissilesData");
      LuaCallbackBindingHelper<Hash64_CT("LuaGameEventsFunctions::GetEnemySpaceshipsCountDestroyedByPlayer"), int32_t(void)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaGameEventsFunctions::GetEnemySpaceshipsCountDestroyedByPlayer, this, std::placeholders::_1), "_GetEnemySpaceshipsCountDestroyedByPlayer");
   }

   void LuaGameEventsFunctions::ProcessEvent(const LuaMainPlayerStatusChangedEvent::EventData_t &data)
   {
      nlohmann::json jsonObj;
      jsonObj["player_status_type"] = static_cast<int32_t>(std::get<0>(data));
      const auto &eventParams = jsonObj.dump();
      LuaFunctionInvoker<void(void *, std::string, std::string)>::Invoke(mOwnerPtr->GetLuaInstance(), "System_OnGameEventTriggered", (void *)mOwnerPtr, std::string("PlayerStatusChanged"), eventParams);
   }

   int32_t LuaGameEventsFunctions::GetSelectedMissileType(const std::tuple<> &data) const
   {
      return static_cast<int32_t>(PlayerDataProvider::GetInstance()->GetSelectedMissileType());
   }

   std::string LuaGameEventsFunctions::GetAllMissilesData(const std::tuple<> &data) const
   {
      const auto &dataProvider = PlayerDataProvider::GetInstance();
      std::unordered_map<eMissileType, size_t> allMissilesData;
      const std::vector<eMissileType> availableMissileTypes = dataProvider->GetAvailableMissileTypes();
      for (const auto availableMissileType : availableMissileTypes)
      {
         allMissilesData[static_cast<eMissileType>(availableMissileType)] = dataProvider->GetMissilesCount(static_cast<eMissileType>(availableMissileType));
      }
      nlohmann::json jsonObj;
      jsonObj["all_missiles_data"] = allMissilesData;
      return jsonObj.dump();
   }

   int32_t LuaGameEventsFunctions::GetEnemySpaceshipsCountDestroyedByPlayer(const std::tuple<> &data) const
   {
      return static_cast<int32_t>(PlayerDataProvider::GetInstance()->GetDestroyedEnemySpaceshipsCount());
   }
}
