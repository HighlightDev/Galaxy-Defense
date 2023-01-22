#include "LuaCommonEngineFunctions.h"

#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/IoCore/FolderManager.h"

#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/CommonCore/StringHash.h"

using namespace IO;
using namespace EngineCore;

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

         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetEngineObject"), EngineObject *(std::string)>::Bind(mLuaInstance, this, std::bind(&LuaCommonEngineFunctions::GetEngineObject, this, std::placeholders::_1), "_GetEngineObject");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetGOPropertyValFloat"), float(EngineObject *, std::string)>::Bind(mLuaInstance, this, std::bind(&LuaCommonEngineFunctions::GetGOPropertyValFloat, this, std::placeholders::_1), "_GetGOPropertyValFloat");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetGOPropertyValInteger"), int32_t(EngineObject *, std::string)>::Bind(mLuaInstance, this, std::bind(&LuaCommonEngineFunctions::GetGOPropertyValInteger, this, std::placeholders::_1), "_GetGOPropertyValInteger");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::SetGOPropertyValVec3"), void(EngineObject *, std::string, glm::vec3)>::Bind(mLuaInstance, this, std::bind(&LuaCommonEngineFunctions::SetGOPropertyValVec3, this, std::placeholders::_1), "_SetGOPropertyValVec3");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::SetGOPropertyValBool"), void(EngineObject *, std::string, int32_t)>::Bind(mLuaInstance, this, std::bind(&LuaCommonEngineFunctions::SetGOPropertyValBool, this, std::placeholders::_1), "_SetGOPropertyValBool");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetWindowHeight"), int32_t(void)>::Bind(mLuaInstance, this, std::bind(&LuaCommonEngineFunctions::GetWindowHeight, this, std::placeholders::_1), "_GetWindowHeight");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetWindowWidth"), int32_t(void)>::Bind(mLuaInstance, this, std::bind(&LuaCommonEngineFunctions::GetWindowWidth, this, std::placeholders::_1), "_GetWindowWidth");
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

      EngineObject *LuaCommonEngineFunctions::GetEngineObject(const std::tuple<std::string> &gameObjectName)
      {
         EngineObject *gameObject = nullptr;

         if (const auto &sceneSP = mSceneWP.lock())
         {
            gameObject = sceneSP->GetEngineObjectByName(std::get<0>(gameObjectName));
         }

         return gameObject;
      }

      float LuaCommonEngineFunctions::GetGOPropertyValFloat(const std::tuple<EngineObject *, std::string> &data)
      {
         EngineObject *gameObject = std::get<0>(data);
         assert(gameObject != nullptr);
         const auto &property = std::static_pointer_cast<EngineGOProperty<float>>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
         assert(property);
         return property->GetValue();
      }

      int32_t LuaCommonEngineFunctions::GetGOPropertyValInteger(const std::tuple<EngineObject *, std::string> &data)
      {
         EngineObject *gameObject = std::get<0>(data);
         assert(gameObject != nullptr);
         const auto &property = std::static_pointer_cast<EngineGOProperty<int32_t>>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
         assert(property);
         return property->GetValue();
      }

      void LuaCommonEngineFunctions::SetGOPropertyValVec3(const std::tuple<EngineObject *, std::string, glm::vec3> &data)
      {
         EngineObject *gameObject = std::get<0>(data);
         assert(gameObject != nullptr);
         auto property = std::static_pointer_cast<EngineGOProperty<glm::vec3>>(gameObject->GetEnginePropertyByName(std::get<1>(data)));
         assert(property);
         property->SetValue(std::get<2>(data));
      }

      void LuaCommonEngineFunctions::SetGOPropertyValBool(const std::tuple<EngineObject *, std::string, int32_t> &data)
      {
         EngineObject *gameObject = std::get<0>(data);
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
