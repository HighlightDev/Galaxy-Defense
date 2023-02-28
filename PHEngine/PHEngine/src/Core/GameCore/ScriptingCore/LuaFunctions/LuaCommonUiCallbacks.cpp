#include "LuaCommonUiCallbacks.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"

using namespace EngineCore;
using namespace IO;

namespace EngineCore
{
   namespace Scripts
   {
      LuaCommonUiCallbacks::LuaCommonUiCallbacks(LuaScriptExecutorBase *ownerPtr)
          : mOwnerPtr(ownerPtr)
      {
      }

      void LuaCommonUiCallbacks::SetScene(const std::weak_ptr<Scene>& sceneWp)
      {
         mSceneWp = sceneWp;
      }
      
      void LuaCommonUiCallbacks::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor)
      {
         mLuaScriptProcessor = scriptProcessor;
      }

      void LuaCommonUiCallbacks::OnScriptStarted(const LuaWrapper &luaWrapper)
      {
      }

      void LuaCommonUiCallbacks::OnScriptStopped(const LuaWrapper &luaWrapper)
      {
      }

      void LuaCommonUiCallbacks::RegisterCallbacks(const LuaWrapper &luaWrapper)
      {
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiCallbacks::GetCurrentOverlayName"), std::string(void)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiCallbacks::GetCurrentOverlayName, this, std::placeholders::_1), "_GetCurrentOverlayName");
      }

      std::string LuaCommonUiCallbacks::GetCurrentOverlayName(const std::tuple<> &data)
      {
         if (const auto &luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock())
         {
            return luaProcessorSp->GetOverlayManagerLuaProxy()->GetCurrentOverlayName();
         }
         return "";
      }
   }
}
