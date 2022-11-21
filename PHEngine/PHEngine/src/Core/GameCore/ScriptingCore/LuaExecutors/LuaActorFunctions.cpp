#include "LuaActorFunctions.h"
#include "Core/GameCore/Actor.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"

using namespace EngineCore;

namespace EngineCore
{
   namespace Scripts
   {
      LuaActorFunctions::LuaActorFunctions(const std::string &scriptName, std::weak_ptr<Actor> parentActor)
          : mScriptName(scriptName), mParentActor(parentActor)
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