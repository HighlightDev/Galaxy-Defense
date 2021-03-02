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

      LuaRegisterCallback<LuaExecutor_t, void(GameObject*, std::string, glm::vec3)>::Register(mLuaInstance, "_SetGOPropertyValVec3");
   }

   void LuaScriptExecutor_EngineBase::RunScript()
   {
      const auto& folderManager = FolderManager::GetInstance();
      const bool bScriptExecuted = mLuaInstance.ExecuteScript(EngineUtility::ConvertFromRelativeToAbsolutePath(folderManager->GetScriptPath() + mScriptName));
      assert(bScriptExecuted);

      mLuaCoreData.HasOnStart = LuaGetGlobal<int64_t>::Value(mLuaInstance, "HasOnStart", -1);
      mLuaCoreData.HasOnUpdate = LuaGetGlobal<int64_t>::Value(mLuaInstance, "HasOnUpdate", -1);

      if (mLuaCoreData.HasOnStart)
      {
         LuaFunction<void(void*)>::Call(mLuaInstance, "OnStart", (void*)this);
      }
   }

   /*_GetGameObject*/
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
      const auto& property = static_cast<GenericObjectProperty<int32_t>*>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
      return property->GetValue();
   }

   /*_SetGOPropertyValVec3*/
   void LuaScriptExecutor_EngineBase::ExecuteLuaCallback(const std::tuple<GameObject*, std::string, glm::vec3>& data)
   {
      GameObject* gameObject = std::get<0>(data);
      assert(gameObject != nullptr);
      auto property = static_cast<GenericObjectProperty<glm::vec3>*>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
      property->Value = std::get<2>(data);
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
