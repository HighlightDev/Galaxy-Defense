#pragma once

#include <memory>

#include "Core/GameCore/ScriptingCore/LuaCore.inl"

namespace EngineCore
{
   class Actor;
   namespace Scripts
   {
      class LuaActorFunctions
      {
         LuaWrapper mLuaInstance;

         std::string mScriptName;

         std::weak_ptr<Actor> mParentActor;

      public:
         LuaActorFunctions(const std::string &scriptName, std::weak_ptr<::EngineCore::Actor> parentActor);

         ~LuaActorFunctions();

         void RegisterCallbacks();

         void RunScript();

         // Common callbacks
      };
   }
}
