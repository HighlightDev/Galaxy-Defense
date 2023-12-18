#pragma once
#include <tuple>
#include <memory>
#include <type_traits>
#include <string>
#include <glm/vec3.hpp>

#include "Core/GameCore/Scene.h"
#include "ILuaFunctionable.h"

using namespace EngineCore;

namespace EngineCore
{
   namespace Scripts
   {
      class LuaScriptExecutorBase;
      class LuaScriptProcessor;

      class LuaCommonEngineFunctions
          : public ILuaFunctionable
      {
      protected:
         LuaScriptExecutorBase *mOwnerPtr;

         std::weak_ptr<Scene> mSceneWp;

         std::weak_ptr<LuaScriptProcessor> mLuaScriptProcessor;

      public:
         LuaCommonEngineFunctions(LuaScriptExecutorBase *ownerPtr);

         virtual ~LuaCommonEngineFunctions() = default;

         void OnScriptStarted(const LuaWrapper &luaWrapper) override;

         void OnScriptStopped(const LuaWrapper &luaWrapper) override;

         void RegisterCallbacks(const LuaWrapper &luaWrapper) override;

         void SetScene(const std::weak_ptr<Scene>& sceneWp) override;

         void SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& scriptProcessor) override;

         void Initialize() override;

      private:
         /*Get Window height*/
         int32_t GetWindowHeight(const std::tuple<> &data);

         /*Get Window width*/
         int32_t GetWindowWidth(const std::tuple<> &data);

         /*Input callbacks*/
         bool HasPressedKeyboardButtons(const std::tuple<> &data);

         bool HasReleasedKeyboardButtons(const std::tuple<> &data);

         std::string GetKeyboardJsonData(const std::tuple<> &data);
      };
   }
}
