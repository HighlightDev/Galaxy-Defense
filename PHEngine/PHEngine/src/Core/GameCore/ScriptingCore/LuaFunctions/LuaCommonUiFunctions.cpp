#include "LuaCommonUiFunctions.h"
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
      LuaCommonUiFunctions::LuaCommonUiFunctions(LuaScriptExecutorBase *ownerPtr)
          : mOwnerPtr(ownerPtr)
      {
      }

      void LuaCommonUiFunctions::SetScene(const std::weak_ptr<Scene> &sceneWp)
      {
         mSceneWp = sceneWp;
      }

      void LuaCommonUiFunctions::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor)
      {
         mLuaScriptProcessor = scriptProcessor;
      }

      void LuaCommonUiFunctions::OnScriptStarted(const LuaWrapper &luaWrapper)
      {
      }

      void LuaCommonUiFunctions::OnScriptStopped(const LuaWrapper &luaWrapper)
      {
      }

      void LuaCommonUiFunctions::RegisterCallbacks(const LuaWrapper &luaWrapper)
      {
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::GetCurrentOverlayName"), std::string(void)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiFunctions::GetCurrentOverlayName, this, std::placeholders::_1), "_GetCurrentOverlayName");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::OpenOverlay"), void(std::string)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiFunctions::OpenOverlay, this, std::placeholders::_1), "_OpenOverlay");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::CloseCurrentOverlay"), void()>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiFunctions::CloseCurrentOverlay, this, std::placeholders::_1), "_CloseCurrentOverlay");
      }

      std::string LuaCommonUiFunctions::GetCurrentOverlayName(const std::tuple<> &data)
      {
         if (const auto &luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock())
         {
            return luaProcessorSp->GetOverlayManagerLuaProxy()->GetCurrentOverlayName();
         }

         return "";
      }

      void LuaCommonUiFunctions::OpenOverlay(const std::tuple<std::string> &overlayName)
      {
         const auto &overlayToOpen = std::get<0>(overlayName);

         if (const auto &luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock())
         {
            luaProcessorSp->GetOverlayManagerLuaProxy()->OpenOverlay(overlayToOpen);
         }
      }

      void LuaCommonUiFunctions::CloseCurrentOverlay(const std::tuple<>& emptyData)
      {
         if (const auto &luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock())
         {
            luaProcessorSp->GetOverlayManagerLuaProxy()->CloseCurrentOverlay();
         }
      }
   }
}
