#include "LuaScriptExecutor_EngineBase.h"

namespace Game
{

   LuaScriptExecutor_EngineBase::LuaScriptExecutor_EngineBase(const std::string& scriptName)
      : mScriptName(scriptName)
   {
   }

   LuaScriptExecutor_EngineBase::~LuaScriptExecutor_EngineBase()
   {
   }

   void LuaScriptExecutor_EngineBase::RegisterCallbacks()
   {
      using LuaExecutor_t = LuaScriptExecutor_EngineBase;

      LuaRegisterCallback<LuaExecutor_t, GameObject*(std::string, LuaArgDummyPlaceholder<>, LuaArgDummyPlaceholder<>)>::Register(mLuaInstance, "_GetGameObject");
      LuaRegisterCallback<LuaExecutor_t, float(GameObject*, std::string, LuaArgDummyPlaceholder<float>)>::Register(mLuaInstance, "_GetGOPropertyValFloat");
      LuaRegisterCallback<LuaExecutor_t, int32_t(GameObject*, std::string, LuaArgDummyPlaceholder<int32_t>)>::Register(mLuaInstance, "_GetGOPropertyValInteger");
   }

   GameObject* LuaScriptExecutor_EngineBase::ExecuteLuaCallback(const std::tuple<std::string, LuaArgDummyPlaceholder<>, LuaArgDummyPlaceholder<>>& gameObjectName)
   {
      GameObject* gameObject = nullptr;

      if (const auto& sceneSP = mSceneWP.lock())
      {
         gameObject = sceneSP->GetGameObjectByName(std::get<0>(gameObjectName));
      }

      return gameObject;
   }

   /*_GetGOPropertyValFloat*/
   float LuaScriptExecutor_EngineBase::ExecuteLuaCallback(const std::tuple<GameObject *, std::string, LuaArgDummyPlaceholder<float>>& data)
   {
      GameObject* gameObject = std::get<0>(data);
      assert(gameObject != nullptr);
      const auto& doubleProperty = static_cast<GenericObjectProperty<float>*>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
      return doubleProperty->GetValue();
   }

   /*_GetGOPropertyValInteger*/
   int32_t LuaScriptExecutor_EngineBase::ExecuteLuaCallback(const std::tuple<GameObject *, std::string, LuaArgDummyPlaceholder<int32_t>>& data)
   {
      GameObject* gameObject = std::get<0>(data);
      assert(gameObject != nullptr);
      const auto& intProperty = static_cast<GenericObjectProperty<int32_t>*>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
      return intProperty->GetValue();
   }

   std::string LuaScriptExecutor_EngineBase::GetScriptRelPath() const
   {
      return mScriptName;
   }

   void LuaScriptExecutor_EngineBase::PostInit(std::weak_ptr<Scene> scene)
   {
      mSceneWP = scene;
   }

}
