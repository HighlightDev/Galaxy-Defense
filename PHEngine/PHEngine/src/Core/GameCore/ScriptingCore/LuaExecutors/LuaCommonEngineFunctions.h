#pragma once
#include <tuple>
#include <memory>
#include <type_traits>

#include "Core/GameCore/ScriptingCore/LuaScriptExecutorBase.h"

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

         void RegisterCallbacks() override;

         void StopScript() override;

         void OnUpdate(const float deltaTime) override;

         EngineObject *GetEngineObject(const std::tuple<std::string> &gameObjectName);

         float GetGOPropertyValFloat(const std::tuple<EngineObject *, std::string> &data);

         int32_t GetGOPropertyValInteger(const std::tuple<EngineObject *, std::string> &data);

         void SetGOPropertyValVec3(const std::tuple<EngineObject *, std::string, glm::vec3> &data);

         void SetGOPropertyValBool(const std::tuple<EngineObject *, std::string, int32_t> &data);

         /*Get Window height*/
         int32_t GetWindowHeight(const std::tuple<> &data);

         /*Get Window width*/
         int32_t GetWindowWidth(const std::tuple<> &data);
      };
   }
}
