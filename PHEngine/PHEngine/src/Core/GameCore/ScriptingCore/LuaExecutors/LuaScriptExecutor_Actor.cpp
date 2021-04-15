#include "LuaScriptExecutor_Actor.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/Tweener/TweenBindingAttachmentBuilder.h"

namespace Game
{

   LuaScriptExecutor_Actor::LuaScriptExecutor_Actor(const std::string& scriptName, std::weak_ptr<Actor> parentActor)
      : mScriptName(scriptName)
      , mParentActor(parentActor)
   {

   }

   LuaScriptExecutor_Actor::~LuaScriptExecutor_Actor()
   {
   }

   void LuaScriptExecutor_Actor::RegisterCallbacks()
   {

   }

   void LuaScriptExecutor_Actor::RunScript()
   {

   }

}