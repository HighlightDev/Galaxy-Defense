#pragma once

#include <memory>

namespace EngineCore
{
   class Actor;
   namespace Scripts
   {
      class LuaActorFunctions
      {
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
