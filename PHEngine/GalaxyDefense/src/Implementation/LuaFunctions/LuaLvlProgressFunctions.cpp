#include "LuaLvlProgressFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"

#include "Implementation/LevelProgressSystem/LevelProgressController.h"
#include "Implementation/LevelProgressSystem/LevelRequirementTrackerJsonFactories/LevelRequirementsTrackerFactory.h"

#include <json/json.hpp>

using namespace EngineCore;

namespace Game
{
   LuaLvlProgressFunctions::LuaLvlProgressFunctions(LuaScriptExecutorBase *ownerPtr,
                                                    const std::shared_ptr<LevelProgressController> &lvlProgressController)
       : mOwnerPtr(ownerPtr),
         mLevelProgressController(lvlProgressController)
   {
   }

   LuaLvlProgressFunctions::~LuaLvlProgressFunctions()
   {
   }

   void LuaLvlProgressFunctions::Initialize()
   {
   }

   void LuaLvlProgressFunctions::SetScene(const std::weak_ptr<Scene> &sceneWp)
   {
      mSceneWp = sceneWp;
   }

   void LuaLvlProgressFunctions::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor)
   {
      mLuaScriptProcessor = scriptProcessor;
   }

   void LuaLvlProgressFunctions::OnScriptStarted(const LuaWrapper &luaWrapper)
   {
   }

   void LuaLvlProgressFunctions::OnScriptStopped(const LuaWrapper &luaWrapper)
   {
   }

   void LuaLvlProgressFunctions::RegisterCallbacks(const LuaWrapper &luaWrapper)
   {
      LuaCallbackBindingHelper<Hash64_CT("LuaLvlProgressFunctions::SetLevelProgressStagesQueue"), void(std::string)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaLvlProgressFunctions::SetLevelProgressStagesQueue, this, std::placeholders::_1), "_SetLevelProgressStagesQueue");
      LuaCallbackBindingHelper<Hash64_CT("LuaLvlProgressFunctions::GetCurrentProgressRequirementsCount"), int32_t()>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaLvlProgressFunctions::GetCurrentProgressRequirementsCount, this, std::placeholders::_1), "_GetCurrentProgressRequirementsCount");
      LuaCallbackBindingHelper<Hash64_CT("LuaLvlProgressFunctions::GetCurrentProgressStageName"), std::string()>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaLvlProgressFunctions::GetCurrentProgressStageName, this, std::placeholders::_1), "_GetCurrentProgressStageName");
      LuaCallbackBindingHelper<Hash64_CT("LuaLvlProgressFunctions::GetCurrentProgressStageRequirementTrackers"), std::string()>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaLvlProgressFunctions::GetCurrentProgressStageRequirementTrackers, this, std::placeholders::_1), "_GetCurrentProgressStageRequirementTrackers");
   }

   void LuaLvlProgressFunctions::SetLevelProgressStagesQueue(const std::tuple<std::string /*level progress stages queue json*/> &data)
   {
      const auto &jsonStr = std::get<0>(data);
      const auto &rootObj = nlohmann::json::parse(jsonStr);
      std::shared_ptr<LevelProgressStage> levelProgressStageSp = nullptr;

      for (const auto stageJsonRoot : rootObj)
      {
         const auto &trackerFactory = std::make_shared<LevelRequirementsTrackerFactory>();
         const auto stageName = stageJsonRoot.at("name").get<std::string>();
         levelProgressStageSp = std::make_shared<LevelProgressStage>(stageName);

         const auto trackersJsonRoot = stageJsonRoot.at("trackers");
         for (const auto trackerJsonRoot : trackersJsonRoot)
         {
            const auto &reqTrackerType = trackerJsonRoot.at("type");
            levelProgressStageSp->AddLevelProgressRequirementTracker(trackerFactory->CreateLevelRequirementTracker(reqTrackerType, trackerJsonRoot));
         }
      }

      assert(levelProgressStageSp);
      mLevelProgressController->AddLevelProgressStage(levelProgressStageSp);
   }

   int32_t LuaLvlProgressFunctions::GetCurrentProgressRequirementsCount(const std::tuple<> &data) const
   {
      return mLevelProgressController->GetCurrentProgressRequirementsCount();
   }

   std::string LuaLvlProgressFunctions::GetCurrentProgressStageName(const std::tuple<> &data) const
   {
      return mLevelProgressController->GetCurrentProgressStageName();
   }

   std::string LuaLvlProgressFunctions::GetCurrentProgressStageRequirementTrackers(const std::tuple<> &data)
   {
      const auto &requirementTrackers = mLevelProgressController->GetLevelProgressRequirementTrackers();

      if (requirementTrackers.size())
      {
         nlohmann::json requirementTrackersJsonObj;

         int32_t trackerIndex = 0;
         for (const auto &tracker : requirementTrackers)
         {
            requirementTrackersJsonObj[trackerIndex++] = tracker->SerializeParameters();
         }

         return requirementTrackersJsonObj.dump();
      }
      return "";
   }
}
