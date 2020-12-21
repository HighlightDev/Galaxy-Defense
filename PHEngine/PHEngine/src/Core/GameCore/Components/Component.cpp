#include "Component.h"
#include "Core/GameCore/Actor.h"

#include <algorithm>

namespace Game
{

	Component::Component(const std::string& gameObjectName)
		: GameObject(gameObjectName)
      , m_owner(nullptr)
      , mIsEnabled(true)
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

   SerializeDataActor& Component::GetSerializeDataActor(SerializeDataContainer& dataContainer)
   {
      auto it = std::find_if(dataContainer.Actors.begin(), dataContainer.Actors.end(), [=](const SerializeDataActor& actorData) { return actorData.ActorName == GetOwner()->GetName(); });
      assert(it != dataContainer.Actors.end());
      return *it;
   }

   Actor* Component::GetOwner() const
   {
      return m_owner;
   }

   bool Component::IsEnabled() const
   {
      return mIsEnabled;
   }

   void Component::SetIsEnabled(const bool bEnabled)
   {
      mIsEnabled = bEnabled;
   }

}