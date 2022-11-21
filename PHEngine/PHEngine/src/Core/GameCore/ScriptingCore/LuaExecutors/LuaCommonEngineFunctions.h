#pragma once
#include <tuple>
#include <memory>
#include <type_traits>

#include "Core/GameCore/ScriptingCore/LuaScriptExecutorBase.h"

using namespace EngineCore::Scripts;

namespace EngineCore
{
   namespace Scripts
   {
      class LuaCommonEngineFunctions
          : public LuaScriptExecutorBase
      {
      public:
         LuaCommonEngineFunctions(const std::string &scriptName);

         virtual ~LuaCommonEngineFunctions();

         virtual void RegisterCallbacks();

         virtual void RunScript();

         virtual void OnUpdate(const float deltaTime);

         GameObject *GetGameObject(const std::tuple<std::string> &gameObjectName);

         float GetGOPropertyValFloat(const std::tuple<GameObject *, std::string> &data);

         int32_t GetGOPropertyValInteger(const std::tuple<GameObject *, std::string> &data);

         void SetGOPropertyValVec3(const std::tuple<GameObject *, std::string, glm::vec3> &data);

         void SetGOPropertyValBool(const std::tuple<GameObject *, std::string, int32_t> &data);

         /*Get Window height*/
         int32_t GetWindowHeight(const std::tuple<> &data);

         /*Get Window width*/
         int32_t GetWindowWidth(const std::tuple<> &data);
      };
   }
}
