#pragma once

#include <memory>

#include "Core/GameCore/ScriptingCore/LuaCore.inl"

namespace Game
{

   class Actor;

   class LuaScriptExecutor_Actor
   {
      LuaWrapper mLuaInstance;

      std::string mScriptName;

      std::weak_ptr<Actor> mParentActor;

   public:

      LuaScriptExecutor_Actor(const std::string& scriptName, std::weak_ptr<Actor> parentActor);

      ~LuaScriptExecutor_Actor();

      void RegisterCallbacks();

      void RunScript();

      // Common callbacks
   };

}
