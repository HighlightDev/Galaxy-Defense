#pragma once
#include <tuple>
#include <memory>
#include <type_traits>

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaCore.inl"

namespace Game
{

   class LuaScriptExecutor_EngineBase
   {
   protected:

      struct LuaCorePOD
      {
         bool HasOnStart;
         bool HasOnUpdate;
      };

   protected:

      std::weak_ptr<Scene> mSceneWP;

      LuaWrapper mLuaInstance;

      std::string mScriptName;

      LuaCorePOD mLuaCoreData;

   public:

      LuaScriptExecutor_EngineBase(const std::string& scriptName);

      virtual ~LuaScriptExecutor_EngineBase();

      virtual void RegisterCallbacks();

      virtual void RunScript();

      GameObject* ExecuteLuaCallback(const std::tuple<std::string, LuaArgDummyPlaceholder<>, LuaArgDummyPlaceholder<>>& gameObjectName);

      float ExecuteLuaCallback(const std::tuple<GameObject*, std::string, LuaArgDummyPlaceholder<float>>& data);

      int32_t ExecuteLuaCallback(const std::tuple<GameObject*, std::string, LuaArgDummyPlaceholder<int32_t>>& data);

      void ExecuteLuaCallback(const std::tuple<GameObject*, std::string, glm::vec3>& data);

      std::string GetScriptRelPath() const;

      void PostInit(std::weak_ptr<Scene> scene);
   };

}

