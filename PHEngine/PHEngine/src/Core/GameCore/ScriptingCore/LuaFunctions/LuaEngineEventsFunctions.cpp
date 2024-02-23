#include "LuaEngineEventsFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Event/PauseGameEvent.h"
#include "Core/GameCore/Event/LoadLevelEvent.h"
#include "Core/GameCore/Event/ExitGameEvent.h"
#include "Core/GameCore/Event/BroadcastEvent.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace IO;
using namespace Event;

namespace EngineCore
{
   namespace Scripts
   {
      LuaEngineEventsFunctions::LuaEngineEventsFunctions(LuaScriptExecutorBase *ownerPtr)
          : mOwnerPtr(ownerPtr)
      {
      }

      LuaEngineEventsFunctions::~LuaEngineEventsFunctions()
      {
         WindowSizeChangedLuaThreadEvent::GetInstance()->RemoveListener(WindowSizeChangedLuaThreadEvent::GetInstanceId());
      }

      void LuaEngineEventsFunctions::Initialize()
      {
         WindowSizeChangedLuaThreadEvent::GetInstance()->AddListener(shared_from_this());
      }

      void LuaEngineEventsFunctions::SetScene(const std::weak_ptr<Scene> &sceneWp)
      {
         mSceneWp = sceneWp;
      }

      void LuaEngineEventsFunctions::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor)
      {
         mLuaScriptProcessor = scriptProcessor;
      }

      void LuaEngineEventsFunctions::OnScriptStarted(const LuaWrapper &luaWrapper)
      {
      }

      void LuaEngineEventsFunctions::OnScriptStopped(const LuaWrapper &luaWrapper)
      {
      }

      void LuaEngineEventsFunctions::RegisterCallbacks(const LuaWrapper &luaWrapper)
      {
         LuaCallbackBindingHelper<Hash64_CT("LuaEngineEventsFunctions::SendPauseGameThreadEvent"), void(int32_t, bool)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaEngineEventsFunctions::SendPauseGameThreadEvent, this, std::placeholders::_1), "_SendPauseGameThreadEvent");
         LuaCallbackBindingHelper<Hash64_CT("LuaEngineEventsFunctions::SendExitGameThreadEvent"), void(int32_t)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaEngineEventsFunctions::SendExitGameThreadEvent, this, std::placeholders::_1), "_SendExitGameThreadEvent");
         LuaCallbackBindingHelper<Hash64_CT("LuaEngineEventsFunctions::SendLoadLevelGameThreadEvent"), void(int32_t, std::string)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaEngineEventsFunctions::SendLoadLevelGameThreadEvent, this, std::placeholders::_1), "_SendLoadLevelGameThreadEvent");
         LuaCallbackBindingHelper<Hash64_CT("LuaEngineEventsFunctions::SendBroadcastGameThreadEvent"), void(int32_t, std::string, std::string)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaEngineEventsFunctions::SendBroadcastGameThreadEvent, this, std::placeholders::_1), "_SendBroadcastGameThreadEvent");
      }

      void LuaEngineEventsFunctions::SendPauseGameThreadEvent(const std::tuple<int32_t /*enqueue policy*/, bool /*true: pause, false: unpause*/> &data)
      {
         const auto enqueuePolicy = std::get<0>(data);
         const auto isPause = std::get<1>(data);

         if (const auto &sceneSp = mSceneWp.lock())
         {
            static constexpr auto functionId = Hash64_CT("LuaEngineEventsFunctions::SendPauseGameThreadEvent");
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(static_cast<eEnqueueJobPolicy>(enqueuePolicy), 0, functionId, [isPause]()
                                                                              { PauseGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, isPause); });
         }
      }

      void LuaEngineEventsFunctions::SendExitGameThreadEvent(const std::tuple<int32_t /*enqueue policy*/> &data)
      {
         const auto enqueuePolicy = std::get<0>(data);

         if (const auto &sceneSp = mSceneWp.lock())
         {
            static constexpr auto functionId = Hash64_CT("LuaEngineEventsFunctions::SendExitGameThreadEvent");
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(static_cast<eEnqueueJobPolicy>(enqueuePolicy), 0, functionId, []()
                                                                              { ExitGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION); });
         }
      }

      void LuaEngineEventsFunctions::SendLoadLevelGameThreadEvent(const std::tuple<int32_t /*enqueue policy*/, std::string /*level name*/> &data)
      {
         const auto enqueuePolicy = std::get<0>(data);
         const auto levelName = std::get<1>(data);

         if (const auto &sceneSp = mSceneWp.lock())
         {
            static constexpr auto functionId = Hash64_CT("LuaEngineEventsFunctions::SendLoadLevelGameThreadEvent");
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(static_cast<eEnqueueJobPolicy>(enqueuePolicy), 0, functionId, [levelName]()
                                                                              { LoadLevelGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, levelName); });
         }
      }

      void LuaEngineEventsFunctions::SendBroadcastGameThreadEvent(const std::tuple<int32_t /*enqueue policy*/, std::string /*event header*/, std::string /*json parameters*/> &data)
      {
         const auto enqueuePolicy = std::get<0>(data);
         const auto &eventHeader = std::get<1>(data);
         const auto &eventJsonParameters = std::get<2>(data);

         if (const auto &sceneSp = mSceneWp.lock())
         {
            static constexpr auto functionId = Hash64_CT("LuaEngineEventsFunctions::SendBroadcastGameThreadEvent");
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(static_cast<eEnqueueJobPolicy>(enqueuePolicy), 0, functionId, [eventHeader, eventJsonParameters]()
                                                                              { BroadcastGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, eventHeader, eventJsonParameters); });
         }
      }

      void LuaEngineEventsFunctions::ProcessEvent(const WindowSizeChangedLuaThreadEvent::EventData_t &data)
      {
         const auto viewPortInfo = std::get<0>(data);
         nlohmann::json jsonObj;
         jsonObj["width"] = viewPortInfo.Width;
         jsonObj["height"] = viewPortInfo.Height;
         const auto &eventParams = jsonObj.dump();
         LuaFunctionInvoker<void(void *, std::string, std::string)>::Invoke(mOwnerPtr->GetLuaInstance(), "System_OnEngineEventTriggered", (void *)mOwnerPtr, std::string("WindowSizeChanged"), eventParams);
      }
   }
}
