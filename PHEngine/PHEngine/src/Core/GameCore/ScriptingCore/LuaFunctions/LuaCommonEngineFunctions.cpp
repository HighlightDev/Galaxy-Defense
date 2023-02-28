#include "LuaCommonEngineFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/CommonCore/Assertion.h"

using namespace EngineCore;
using namespace IO;

namespace EngineCore
{
   namespace Scripts
   {
      LuaCommonEngineFunctions::LuaCommonEngineFunctions(LuaScriptExecutorBase *ownerPtr)
          : mOwnerPtr(ownerPtr)
      {
      }

      void LuaCommonEngineFunctions::SetScene(const std::weak_ptr<Scene> &sceneWp)
      {
         mSceneWp = sceneWp;
      }

      void LuaCommonEngineFunctions::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor)
      {
         mLuaScriptProcessor = scriptProcessor;
      }

      void LuaCommonEngineFunctions::OnScriptStarted(const LuaWrapper &luaWrapper)
      {
      }

      void LuaCommonEngineFunctions::OnScriptStopped(const LuaWrapper &luaWrapper)
      {
      }

      void LuaCommonEngineFunctions::RegisterCallbacks(const LuaWrapper &luaWrapper)
      {
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetEngineObject"), EngineObject *(std::string)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonEngineFunctions::GetEngineObject, this, std::placeholders::_1), "_GetEngineObject");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetGOPropertyValFloat"), float(EngineObject *, std::string)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonEngineFunctions::GetGOPropertyValFloat, this, std::placeholders::_1), "_GetGOPropertyValFloat");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetGOPropertyValInteger"), int32_t(EngineObject *, std::string)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonEngineFunctions::GetGOPropertyValInteger, this, std::placeholders::_1), "_GetGOPropertyValInteger");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::SetGOPropertyValVec3"), void(EngineObject *, std::string, glm::vec3)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonEngineFunctions::SetGOPropertyValVec3, this, std::placeholders::_1), "_SetGOPropertyValVec3");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::SetGOPropertyValBool"), void(EngineObject *, std::string, int32_t)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonEngineFunctions::SetGOPropertyValBool, this, std::placeholders::_1), "_SetGOPropertyValBool");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetWindowHeight"), int32_t(void)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonEngineFunctions::GetWindowHeight, this, std::placeholders::_1), "_GetWindowHeight");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetWindowWidth"), int32_t(void)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonEngineFunctions::GetWindowWidth, this, std::placeholders::_1), "_GetWindowWidth");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::HasPressedKeyboardButtons"), bool(void)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonEngineFunctions::HasPressedKeyboardButtons, this, std::placeholders::_1), "_HasPressedKeyboardButtons");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::HasReleasedKeyboardButtons"), bool(void)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonEngineFunctions::HasReleasedKeyboardButtons, this, std::placeholders::_1), "_HasReleasedKeyboardButtons");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetKeyboardJsonData"), std::string(void)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonEngineFunctions::GetKeyboardJsonData, this, std::placeholders::_1), "_GetKeyboardJsonData");
      }

      EngineObject *LuaCommonEngineFunctions::GetEngineObject(const std::tuple<std::string> &gameObjectName)
      {
         EngineObject *gameObject = nullptr;

         if (const auto &sceneSP = mSceneWp.lock())
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

      bool LuaCommonEngineFunctions::HasPressedKeyboardButtons(const std::tuple<> &data)
      {
         if (const auto &luaProcessorSp = mLuaScriptProcessor.lock())
         {
            return luaProcessorSp->GetEngineInputLuaProxy()->GetIsPressedKeyboardKeys();
         }
         return false;
      }

      bool LuaCommonEngineFunctions::HasReleasedKeyboardButtons(const std::tuple<> &data)
      {
         if (const auto &luaProcessorSp = mLuaScriptProcessor.lock())
         {
            return luaProcessorSp->GetEngineInputLuaProxy()->GetIsReleasedKeyboardKeys();
         }
         return false;
      }

      std::string LuaCommonEngineFunctions::GetKeyboardJsonData(const std::tuple<> &data)
      {
         if (const auto &luaProcessorSp = mLuaScriptProcessor.lock())
         {
            return luaProcessorSp->GetEngineInputLuaProxy()->GetKeyboardJsonData();
         }
         return "";
      }
   }
}
