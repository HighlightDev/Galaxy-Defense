#include "Component.h"
#include "Core/GameCore/Actor.h"

#include <algorithm>

namespace Game
{

   Component::Component(const std::string &gameObjectName)
       : GameObject(gameObjectName), m_owner(), mIsEnabled(true)
   {
   }

   Component::~Component()
   {
   }

   void Component::SetOwner(const std::weak_ptr<Actor> &ownerActor)
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

   std::weak_ptr<Actor> Component::GetBaseOwner() const
   {
      std::weak_ptr<Actor> base = m_owner;

      bool bHasParent = true;

      while (bHasParent)
      {
         if (auto spBase = base.lock())
         {
            auto parent = spBase->GetParent();
            if (parent && parent.lock())
            {
               base = parent->GetParent()
            }
            else
            {
               bHasParent = false;
            }
         }
      }

      return base;
   }

   void Component::OnPostInitialized()
   {
   }

   void Component::PostLevelInit()
   {
   }

   SerializeDataActor &Component::GetSerializeDataActor(SerializeDataContainer &dataContainer)
   {
      auto it = std::find_if(dataContainer.Actors.begin(), dataContainer.Actors.end(), [=](const SerializeDataActor &actorData)
                             { return actorData.ActorName == GetOwner()->GetName(); });
      assert(it != dataContainer.Actors.end());
      return *it;
   }

   Actor *Component::GetOwner() const
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