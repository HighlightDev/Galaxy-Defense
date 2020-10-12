#include "LuaScriptExecutor_EngineBase.h"

namespace Game
{

   LuaScriptExecutor_EngineBase::LuaScriptExecutor_EngineBase(std::weak_ptr<Scene> scene, const std::string& scriptName)
      : mSceneWP(scene)
      , mScriptName(scriptName)
   {
   }

   LuaScriptExecutor_EngineBase::~LuaScriptExecutor_EngineBase()
   {
   }

   void LuaScriptExecutor_EngineBase::RegisterCallbacks()
   {
      using LuaExecutor_t = LuaScriptExecutor_EngineBase;

      LuaRegisterCallback<LuaExecutor_t, GameObject*(std::string, LuaArgDummyPlaceholder, LuaArgDummyPlaceholder)>::Register(mLuaInstance, "_GetGameObject");
   }

   GameObject* LuaScriptExecutor_EngineBase::ExecuteLuaCallback(const std::tuple<std::string, LuaArgDummyPlaceholder, LuaArgDummyPlaceholder>& gameObjectName)
   {
      GameObject* gameObject = nullptr;

      if (const auto& sceneSP = mSceneWP.lock())
      {
         gameObject = sceneSP->GetGameObjectByName(std::get<0>(gameObjectName));
      }

      return gameObject;
   }

}
