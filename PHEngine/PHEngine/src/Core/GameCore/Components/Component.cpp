#include "Component.h"
#include "Core/GameCore/Actor.h"

#include <algorithm>

namespace Game
{

	Component::Component()
		: m_owner(nullptr)
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

   uint64_t Component::GetComponentType() const
   {
      return COMPONENT;
   }

   Actor* Component::GetBaseOwner() const 
   {
      Actor* base = this->m_owner;

      while (base && base->GetParent())
      {
         base = base->GetParent();
      }

      return base;
   }

}