#include "Component.h"
#include "Core/GameCore/Actor.h"

#include <algorithm>

namespace Game
{

	Component::Component(const std::string& gameObjectName)
		: GameObject(gameObjectName)
      , m_owner(nullptr)
	{
	}

	Component::~Component()
	{
	}

	void Component::SetOwner(Actor* ownerActor)
	{
		m_owner = ownerActor;
	}

   void Component::RemoveOwner()
   {
      m_owner = nullptr;
   }

   ComponentType Component::GetComponentType() const
   {
      return COMPONENT;
   }

   Actor* Component::GetBaseOwner() const 
   {
      Actor* base = m_owner;

      while (base && base->GetParent())
      {
         base = base->GetParent();
      }

      return base;
   }

   void Component::PostLevelInit() {

   }

}