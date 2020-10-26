#pragma once
#include <tuple>
#include <memory>

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

      GameObject* ExecuteLuaCallback(const std::tuple<std::string, LuaArgDummyPlaceholder, LuaArgDummyPlaceholder>& gameObjectName);

      std::string GetScriptRelPath() const;

      void PostInit(std::weak_ptr<Scene> scene);
   };

}

