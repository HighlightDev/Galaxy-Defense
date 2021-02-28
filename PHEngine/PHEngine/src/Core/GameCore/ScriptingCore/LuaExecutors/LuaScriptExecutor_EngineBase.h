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

      std::weak_ptr<Scene> mSceneWP;

      LuaWrapper mLuaInstance;

      std::string mScriptName;

   public:

      LuaScriptExecutor_EngineBase(const std::string& scriptName);

      virtual ~LuaScriptExecutor_EngineBase();

      virtual void RegisterCallbacks();

      GameObject* ExecuteLuaCallback(const std::tuple<std::string, LuaArgDummyPlaceholder<>, LuaArgDummyPlaceholder<>>& gameObjectName);

      float ExecuteLuaCallback(const std::tuple<GameObject*, std::string, LuaArgDummyPlaceholder<float>>& data);
      int32_t ExecuteLuaCallback(const std::tuple<GameObject*, std::string, LuaArgDummyPlaceholder<int32_t>>& data);

      std::string GetScriptRelPath() const;

      void PostInit(std::weak_ptr<Scene> scene);
   };

}

