#include "PlayerActor.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/StateMachine/FSMParser.h"
#include "Core/IoCore/FolderManager.h"

namespace Labyrinth
{

   PlayerActor::PlayerActor(const std::string& name, std::shared_ptr<Game::SceneComponent> rootComponent)
      : Actor(name, rootComponent)
   {
   }

   void PlayerActor::PostConstructorInitialize()
   {
      Actor::PostConstructorInitialize();

      InitStateMachine();
   }

   void PlayerActor::InitStateMachine()
   {
      std::shared_ptr<SkeletalMeshComponent> comp = GetComponent<SkeletalMeshComponent>(SKELETAL_MESH_COMPONENT);

      FSMParser fsmParser;
      mStateMachine = fsmParser.ParseFSMDescriptor(IO::FolderManager::GetInstance()->GetFSMPath() + "playerAnimation.fsm");

      auto animationPropBinding = std::static_pointer_cast<AnimationPropertyBinding>(mStateMachine->GetPropertyBindingByName("animationBinding"));

      animationPropBinding->SetBindingProperties(comp->GetSrcAnimationNamePtr(), comp->GetDstAnimationNamePtr(),
         comp->GetSrcAnimationTimePtr(), comp->GetDstAnimationTimePtr(), comp->GetIsTransitionEnabledPtr(), comp->GetTransitionValuePtr());

      mStateMachine->InitRootState();
   }

}
