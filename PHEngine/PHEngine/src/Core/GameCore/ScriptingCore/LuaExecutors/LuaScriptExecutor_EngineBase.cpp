#include "LuaScriptExecutor_EngineBase.h"

#include "Core/IoCore/DisplayDeviceDataProvider.h"

using namespace IO;

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
      LuaRegisterCallback<LuaExecutor_t, void(GameObject*, std::string, int32_t)>::Register(mLuaInstance, "_SetGOPropertyValBool");

      LuaRegisterCallback<LuaExecutor_t, int32_t(LuaArgDummyPlaceholder<int32_t>)>::Register(mLuaInstance, "_GetWindowHeight");
      LuaRegisterCallback<LuaExecutor_t, int32_t(LuaArgDummyPlaceholder<int32_t>, LuaArgDummyPlaceholder<int32_t>)>::Register(mLuaInstance, "_GetWindowWidth");
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
         LuaFunction<void(void*)>::Call(mLuaInstance, "System_OnStart", (void*)this);
      }
   }

   void LuaScriptExecutor_EngineBase::OnUpdate(const float deltaTime)
   {
      if (mLuaCoreData.HasOnUpdate)
      {
         LuaFunction<void(void*, float)>::Call(mLuaInstance, "System_OnUpdate", (void*)this, deltaTime);
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
   float LuaScriptExecutor_EngineBase::ExecuteLuaCallback(const std::tuple<GameObject*, std::string, LuaArgDummyPlaceholder<float>>& data)
   {
      GameObject* gameObject = std::get<0>(data);
      assert(gameObject != nullptr);
      const auto& property = static_cast<EngineGOProperty<float>*>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
      assert(property);
      return property->GetValue();
   }

   /*_GetGOPropertyValInteger*/
   int32_t LuaScriptExecutor_EngineBase::ExecuteLuaCallback(const std::tuple<GameObject*, std::string, LuaArgDummyPlaceholder<int32_t>>& data)
   {
      GameObject* gameObject = std::get<0>(data);
      assert(gameObject != nullptr);
      const auto& property = static_cast<EngineGOProperty<int32_t>*>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
      assert(property);
      return property->GetValue();
   }

   /*_SetGOPropertyValVec3*/
   void LuaScriptExecutor_EngineBase::ExecuteLuaCallback(const std::tuple<GameObject*, std::string, glm::vec3>& data)
   {
      GameObject* gameObject = std::get<0>(data);
      assert(gameObject != nullptr);
      auto property = static_cast<EngineGOProperty<glm::vec3>*>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
      assert(property);
      property->SetValue(std::get<2>(data));
   }

   /*_SetGOPropertyValBool*/
   void LuaScriptExecutor_EngineBase::ExecuteLuaCallback(const std::tuple<GameObject*, std::string, int32_t>& data)
   {
      GameObject* gameObject = std::get<0>(data);
      assert(gameObject != nullptr);
      auto property = static_cast<EngineGOProperty<bool>*>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
      assert(property);
      property->SetValue(static_cast<bool>(std::get<2>(data)));
   }

   /*Get Window height*/
   int32_t LuaScriptExecutor_EngineBase::ExecuteLuaCallback(const std::tuple<LuaArgDummyPlaceholder<int32_t>>& data)
   {
     return DisplayDeviceDataProvider::GetInstance()->GetWindowHeight();
   }

   /*Get Window width*/
   int32_t LuaScriptExecutor_EngineBase::ExecuteLuaCallback(const std::tuple<LuaArgDummyPlaceholder<int32_t>, LuaArgDummyPlaceholder<int32_t>>& data)
   {
      return DisplayDeviceDataProvider::GetInstance()->GetWindowWidth();
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
