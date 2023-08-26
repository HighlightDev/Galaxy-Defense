#include "HumanoidPlayerController.h"

namespace EngineCore
{
   ActorController::ActorController(const std::shared_ptr<Actor> &actor)
       : m_actorWp(actor),
         m_movementComponentWp()
   {
   }

   ActorController::~ActorController()
   {
   }

   void ActorController::Initialize()
   {
      const auto &actorSp = m_actorWp.lock();
      assert(actorSp);
      assert(actorSp->GetMovementComponent());
      m_movementComponentWp = actorSp->GetMovementComponent();
   }

   void ActorController::CleanUp()
   {
   }

   std::weak_ptr<Actor> ActorController::GetBindedActor() const
   {
      return m_actorWp;
   }

   std::string ActorController::GetBindedActorName() const
   {
      if (const auto &actorSp = m_actorWp.lock())
      {
         return actorSp->GetName();
      }
      return "";
   }

   void ActorController::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      dataContainer.ActorControllerData.emplace_back(std::make_shared<SerializeDataPlayerController>(GetBindedActorName()));
   }
}