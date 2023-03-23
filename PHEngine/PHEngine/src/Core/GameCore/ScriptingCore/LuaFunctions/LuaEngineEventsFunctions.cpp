#include "LuaEngineEventsFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Event/PauseGameThreadEvent.h"

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
      }

      void LuaEngineEventsFunctions::SendPauseGameThreadEvent(const std::tuple<int32_t /*enqueue policy*/, bool /*true: pause, false: unpause*/>& data)
      {
         const auto &enqueuePolicy = std::get<0>(data);
         const auto isPause = std::get<1>(data);

         if (const auto& sceneSp = mSceneWp.lock())
         {
            static constexpr auto functionId = Hash64_CT("LuaEngineEventsFunctions::SendPauseGameThreadEvent");
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(static_cast<eEnqueueJobPolicy>(enqueuePolicy), 0, functionId, [isPause]() { 
               PauseGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, isPause);
            });
         }
      }
   }
}
