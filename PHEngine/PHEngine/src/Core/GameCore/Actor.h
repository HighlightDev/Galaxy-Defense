#pragma once

#include <vector>
#include <memory>
#include <type_traits>

#include "GameObject.h"
#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/MovementComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Serialize/ISerializable.h"

using namespace EnginePhysics;

namespace Game
{

   class Scene;

   class Actor
      : public GameObject
      , public ITickable
      , public ISerializable
      , std::enable_shared_from_this<Actor>
   {
   private:

      std::shared_ptr<SceneComponent> m_rootComponent;

      std::shared_ptr<PhysicsComponent> m_physicsComponent;

      // Makes all primitive components visible or not
      EngineGOProperty<bool> mIsVisible;

   protected:

      /* If Actor is disabled 
       - turn off all calculations for him and physics simulation
       also it won't be visible
       */
      bool mIsEnabled;

      std::vector<std::weak_ptr<Actor>> m_children;

      std::weak_ptr<Actor> m_parent;

      std::shared_ptr<InputComponent> m_inputComponent;

      std::shared_ptr<MovementComponent> m_movementComponent;

      std::shared_ptr<Tweener> mTweener;

      std::weak_ptr<Scene> mSceneOwner;

   public:

      std::vector<std::shared_ptr<Game::Component>> m_allComponents;

      Actor(const std::string& gameObjectName, std::shared_ptr<Game::SceneComponent> rootComponent);

      virtual ~Actor();

      std::weak_ptr<Actor> GetWeakFromThis();

      void PostPhysicsInitialize();

      // Tick is executed on game thread
      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      virtual void ChangeState(const std::string& stateName);

      virtual void PostLevelInit();

      void AddComponent(std::shared_ptr<Component> component);

      void RemoveComponent(std::shared_ptr<Game::Component> component);

      void RemoveMovementComponent();

      void RemoveInputComponent();

      void SetIsEnabled(bool isEnabled);

      void SetIsVisible(bool isVisible);

      bool GetIsVisible() const;

      bool IsEnabled() const;

      void SetParent(const std::weak_ptr<Actor>& actor);

      void SetScene(std::weak_ptr<Scene> sceneOwner);

      std::weak_ptr<Actor> GetParent() const;

      std::string GetName() const;

      std::weak_ptr<Scene> GetSceneOwner() const;

      void AttachActor(std::shared_ptr<Actor> actor);

      void DetachActor(std::shared_ptr<Actor> actor);

      void AttachTweener(std::shared_ptr<Tweener> tweener);

      std::shared_ptr<Tweener> GetTweener() const;

      std::shared_ptr<Game::SceneComponent> GetRootComponent() const;

      std::shared_ptr<Game::SceneComponent> GetBaseRootComponent() const;

      std::shared_ptr<InputComponent> GetInputComponent() const;

      std::shared_ptr<MovementComponent> GetMovementComponent() const;

      std::shared_ptr<PhysicsComponent> GetPhysicsComponent() const;

      // If root component has dirty transformation -> update it and all attached actors + children components
      void UpdateRootComponentTransform();

      // If components from list is scene component -> check if it has dity transformation, and if it does -> update it
      void UpdateComponentsTransform();

      template <typename ComponentT>
      typename std::enable_if<std::is_base_of<Component, ComponentT>::value,
         std::vector<std::shared_ptr<ComponentT>>>::type GetComponentsByType()
      {
         std::vector<std::shared_ptr<ComponentT>> result;

         for (const auto& comp : m_allComponents)
         {
            auto seekComp = std::dynamic_pointer_cast<ComponentT>(comp);

            if (seekComp != nullptr)
            {
               result.push_back(seekComp);
            }
         }

         return result;
      }

      private:

         void SyncComponentsVisibility(bool isVisible);
   };

}

