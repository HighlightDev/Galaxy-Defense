#include "HumanoidPlayerController.h"

namespace EngineCore
{
   ActorController::ActorController(std::shared_ptr<Actor> playerActor)
       : m_playerActor(playerActor), m_movementComponent()
   {
   }

   ActorController::~ActorController()
   {
   }

   std::shared_ptr<Actor> ActorController::GetBindedActor() const
   {
      return m_playerActor;
   }

   std::string ActorController::GetBindedActorName() const
   {
      return m_playerActor->GetName();
   }

   void ActorController::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      dataContainer.ActorControllerData.emplace_back(std::make_shared<SerializeDataPlayerController>(m_playerActor->GetName()));
   }
}