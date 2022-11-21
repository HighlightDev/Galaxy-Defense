#include "LuaCommonEngineFunctions.h"

#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/IoCore/FolderManager.h"

#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"

using namespace IO;

namespace EngineCore
{
   namespace Scripts
   {
      LuaCommonEngineFunctions::LuaCommonEngineFunctions(const std::string &scriptName)
          : LuaScriptExecutorBase()
      {
         SetScript(scriptName);
      }

      LuaCommonEngineFunctions::~LuaCommonEngineFunctions()
      {
      }

      void LuaCommonEngineFunctions::RegisterCallbacks()
      {
         using LuaExecutor_t = LuaCommonEngineFunctions;

         LuaCallbackBindingHelper<0, GameObject *(std::string)>::Bind(mLuaInstance, this, std::bind(&LuaCommonEngineFunctions::GetGameObject, this, std::placeholders::_1), "_GetGameObject");
         LuaCallbackBindingHelper<1, float(GameObject *, std::string)>::Bind(mLuaInstance, this, std::bind(&LuaCommonEngineFunctions::GetGOPropertyValFloat, this, std::placeholders::_1), "_GetGOPropertyValFloat");
         LuaCallbackBindingHelper<2, int32_t(GameObject *, std::string)>::Bind(mLuaInstance, this, std::bind(&LuaCommonEngineFunctions::GetGOPropertyValInteger, this, std::placeholders::_1), "_GetGOPropertyValInteger");
         LuaCallbackBindingHelper<3, void(GameObject *, std::string, glm::vec3)>::Bind(mLuaInstance, this, std::bind(&LuaCommonEngineFunctions::SetGOPropertyValVec3, this, std::placeholders::_1), "_SetGOPropertyValVec3");
         LuaCallbackBindingHelper<4, void(GameObject *, std::string, int32_t)>::Bind(mLuaInstance, this, std::bind(&LuaCommonEngineFunctions::SetGOPropertyValBool, this, std::placeholders::_1), "_SetGOPropertyValBool");
         LuaCallbackBindingHelper<5, int32_t(void)>::Bind(mLuaInstance, this, std::bind(&LuaCommonEngineFunctions::GetWindowHeight, this, std::placeholders::_1), "_GetWindowHeight");
         LuaCallbackBindingHelper<6, int32_t(void)>::Bind(mLuaInstance, this, std::bind(&LuaCommonEngineFunctions::GetWindowWidth, this, std::placeholders::_1), "_GetWindowWidth");
      }

      void LuaCommonEngineFunctions::RunScript()
      {
         const auto &folderManager = FolderManager::GetInstance();
         const bool bScriptExecuted = mLuaInstance.ExecuteScript(folderManager->GetScriptPath() + mScriptName);
         assert(bScriptExecuted);

         mLuaCoreData.HasOnStart = GetLuaGlobalVariable<int64_t>::Value(mLuaInstance, "HasOnStart", -1);
         mLuaCoreData.HasOnUpdate = GetLuaGlobalVariable<int64_t>::Value(mLuaInstance, "HasOnUpdate", -1);

         if (mLuaCoreData.HasOnStart)
         {
            LuaFunctionInvoker<void(void *)>::Invoke(mLuaInstance, "System_OnStart", (void *)this);
         }
      }

      void LuaCommonEngineFunctions::OnUpdate(const float deltaTime)
      {
         if (mLuaCoreData.HasOnUpdate)
         {
            LuaFunctionInvoker<void(void *, float)>::Invoke(mLuaInstance, "System_OnUpdate", (void *)this, deltaTime);
         }
      }

      GameObject *LuaCommonEngineFunctions::GetGameObject(const std::tuple<std::string> &gameObjectName)
      {
         GameObject *gameObject = nullptr;

         if (const auto &sceneSP = mSceneWP.lock())
         {
            gameObject = sceneSP->GetGameObjectByName(std::get<0>(gameObjectName));
         }

         return gameObject;
      }

      float LuaCommonEngineFunctions::GetGOPropertyValFloat(const std::tuple<GameObject *, std::string> &data)
      {
         GameObject *gameObject = std::get<0>(data);
         assert(gameObject != nullptr);
         const auto &property = std::static_pointer_cast<EngineGOProperty<float>>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
         assert(property);
         return property->GetValue();
      }

      int32_t LuaCommonEngineFunctions::GetGOPropertyValInteger(const std::tuple<GameObject *, std::string> &data)
      {
         GameObject *gameObject = std::get<0>(data);
         assert(gameObject != nullptr);
         const auto &property = std::static_pointer_cast<EngineGOProperty<int32_t>>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
         assert(property);
         return property->GetValue();
      }

      void LuaCommonEngineFunctions::SetGOPropertyValVec3(const std::tuple<GameObject *, std::string, glm::vec3> &data)
      {
         GameObject *gameObject = std::get<0>(data);
         assert(gameObject != nullptr);
         auto property = std::static_pointer_cast<EngineGOProperty<glm::vec3>>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
         assert(property);
         property->SetValue(std::get<2>(data));
      }

      void LuaCommonEngineFunctions::SetGOPropertyValBool(const std::tuple<GameObject *, std::string, int32_t> &data)
      {
         GameObject *gameObject = std::get<0>(data);
         assert(gameObject != nullptr);
         auto property = std::static_pointer_cast<EngineGOProperty<bool>>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
         assert(property);
         property->SetValue(static_cast<bool>(std::get<2>(data)));
      }

      int32_t LuaCommonEngineFunctions::GetWindowHeight(const std::tuple<> &data)
      {
         return DisplayDeviceDataProvider::GetInstance()->GetWindowHeight();
      }

      int32_t LuaCommonEngineFunctions::GetWindowWidth(const std::tuple<> &data)
      {
         return DisplayDeviceDataProvider::GetInstance()->GetWindowWidth();
      }
   }
}
