#include "Component.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Scene.h"

#include <algorithm>

namespace EngineCore
{

   Component::Component(const std::string &gameObjectName)
       : GameObject(gameObjectName),
         m_owner(),
         mIsEnabled(std::make_shared<EngineGOProperty<bool>>(true, "p_isEnabled", [=](const bool &isEnabled)
                                                             { SetIsEnabled(isEnabled); })),
         mIsPostLevelInitialized(false),
         m_sceneWP()
   {
      AddEngineProperty(mIsEnabled);
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

   eComponentType Component::GetComponentType() const
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

   void Component::SetScene(const std::weak_ptr<Scene> &scene)
   {
      m_sceneWP = scene;
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
      return mIsEnabled->GetValue();
   }

   void Component::SetIsEnabled(const bool bEnabled)
   {
      if (mIsEnabled->GetValue() != bEnabled)
      {
         mIsEnabled->SetValue(bEnabled, false);
      }
   }

}