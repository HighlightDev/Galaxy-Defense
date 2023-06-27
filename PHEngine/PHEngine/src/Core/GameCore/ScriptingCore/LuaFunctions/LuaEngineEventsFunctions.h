#pragma once
#include <tuple>
#include <string>

#include "ILuaFunctionable.h"

namespace EngineCore
{
   class Scene;
}

namespace EngineCore
{
   namespace Scripts
   {
      class LuaScriptExecutorBase;
      class LuaScriptProcessor;

      class LuaEngineEventsFunctions
          : public ILuaFunctionable
      {
      protected:
         LuaScriptExecutorBase *mOwnerPtr;

         std::weak_ptr<::EngineCore::Scene> mSceneWp;

         std::weak_ptr<LuaScriptProcessor> mLuaScriptProcessor;

      public:
         LuaEngineEventsFunctions(LuaScriptExecutorBase *ownerPtr);

         void SetScene(const std::weak_ptr<Scene> &sceneWp) override;

         void SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor) override;

         void OnScriptStarted(const LuaWrapper &luaWrapper) override;

         void OnScriptStopped(const LuaWrapper &luaWrapper) override;

         void RegisterCallbacks(const LuaWrapper &luaWrapper) override;

      private:
         void SendPauseGameThreadEvent(const std::tuple<int32_t /*enqueue policy*/, bool /*true: pause, false: unpause*/> &data);

         void SendExitGameThreadEvent(const std::tuple<int32_t/*enqueue policy*/>& data);

         void SendLoadLevelGameThreadEvent(const std::tuple<int32_t /*enqueue policy*/, std::string/*level name*/>& data);
      };
   }
}
