#include "Component.h"
#include "Core/GameCore/Actor.h"

#include <algorithm>

namespace EngineCore
{

   Component::Component(const std::string &gameObjectName)
       : GameObject(gameObjectName), m_owner(), mIsEnabled(true), mIsPostLevelInitialized(false)
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
      m_owner.reset();
   }

   ComponentType Component::GetComponentType() const
   {
      return COMPONENT;
   }

   void Component::Tick(const float deltaTime)
   {
      if (!mIsPostLevelInitialized)
      {
         // this call is necessary to resolve issue which could be observed in case component
         // had been created AFTER level was initialized
         PostLevelInit();
      }
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
            if (const auto &spParent = parent.lock())
            {
               base = spParent->GetParent();
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
      mIsPostLevelInitialized = true;
   }

   SerializeDataActor &Component::GetSerializeDataActor(SerializeDataContainer &dataContainer)
   {
      auto it = std::find_if(dataContainer.Actors.begin(), dataContainer.Actors.end(), [=](const SerializeDataActor &actorData)
                             {
         bool bFindResult = false;
         if (const auto& spOwner = GetOwner().lock())
         {
            bFindResult = actorData.ActorName == spOwner->GetName();
         }
         return bFindResult; });
      assert(it != dataContainer.Actors.end());
      return *it;
   }

   std::weak_ptr<Actor> Component::GetOwner() const
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