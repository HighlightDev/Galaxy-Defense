#include "PlayerController.h"

namespace Game
{
   ActorController::ActorController(std::shared_ptr<Actor> playerActor)
       : m_playerActor(playerActor)
   {
   }

   ActorController::~ActorController()
   {
   }

   std::shared_ptr<Actor> ActorController::GetBindedActor() const
   {
      return m_playerActor;
   }
}