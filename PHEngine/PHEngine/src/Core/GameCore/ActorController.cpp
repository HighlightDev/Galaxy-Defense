#include "HumanoidPlayerController.h"

namespace EngineCore
{
   ActorController::ActorController(const std::shared_ptr<Actor>& actor)
       : m_actor(actor), m_movementComponent()
   {
   }

   ActorController::~ActorController()
   {
   }

   void ActorController::InitActorController()
   {
      assert(m_actor);
      m_movementComponent = m_actor->GetMovementComponent();
      assert(m_movementComponent);
   }

   std::shared_ptr<Actor> ActorController::GetBindedActor() const
   {
      return m_actor;
   }

   std::string ActorController::GetBindedActorName() const
   {
      return m_actor->GetName();
   }

   void ActorController::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      dataContainer.ActorControllerData.emplace_back(std::make_shared<SerializeDataPlayerController>(m_actor->GetName()));
   }
}