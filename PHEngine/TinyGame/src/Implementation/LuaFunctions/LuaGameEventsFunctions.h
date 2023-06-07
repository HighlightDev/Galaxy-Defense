#pragma once

#include "Core/GameCore/ScriptingCore/LuaFunctions/ILuaFunctionable.h"
#include "Implementation/Events/MainPlayerStatusChangedEvent.h"

using namespace Event;
using namespace EngineCore::Scripts;

namespace EngineCore
{
   namespace Scripts
   {
      class LuaScriptExecutorBase;
      class LuaScriptProcessor;
   }
}

namespace Game
{
   class LuaGameEventsFunctions
       : public ILuaFunctionable,
         public LuaMainPlayerStatusChangedEvent
   {
   protected:
      ::EngineCore::Scripts::LuaScriptExecutorBase *mOwnerPtr;

      std::weak_ptr<::EngineCore::Scene> mSceneWp;

      std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> mLuaScriptProcessor;

   public:
      LuaGameEventsFunctions(::EngineCore::Scripts::LuaScriptExecutorBase *ownerPtr);

      ~LuaGameEventsFunctions();

      void SetScene(const std::weak_ptr<::EngineCore::Scene> &sceneWp) override;

      void SetLuaScriptProcessor(const std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> &scriptProcessor) override;

      void OnScriptStarted(const LuaWrapper &luaWrapper) override;

      void OnScriptStopped(const LuaWrapper &luaWrapper) override;

      void RegisterCallbacks(const LuaWrapper &luaWrapper) override;

      void ProcessEvent(const LuaMainPlayerStatusChangedEvent::EventData_t &data) override;

   private:
      int32_t GetSelectedMissileType(const std::tuple<> &data);

      std::string GetAllMissilesData(const std::tuple<>& data);
   };
}
