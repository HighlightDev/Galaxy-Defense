#include "LuaActorFunctions.h"
#include "Core/GameCore/Actor.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"

using namespace EngineCore;

namespace EngineCore
{
   namespace Scripts
   {
      LuaActorFunctions::LuaActorFunctions(const std::string &scriptName, std::weak_ptr<Actor> parentActor)
          : mParentActor(parentActor)
      {
      }

      LuaActorFunctions::~LuaActorFunctions()
      {
      }

      void LuaActorFunctions::RegisterCallbacks()
      {
      }

      void LuaActorFunctions::RunScript()
      {
      }
   }
}