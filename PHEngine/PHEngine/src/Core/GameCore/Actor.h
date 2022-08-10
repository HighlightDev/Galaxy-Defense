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

namespace EngineCore
{

   class Scene;

   class Actor
      : public GameObject
      , public ITickable
      , public ISerializable
      , public std::enable_shared_from_this<Actor>
   {
   protected:

      std::shared_ptr<SceneComponent> m_rootComponent;

      std::shared_ptr<PhysicsComponent> m_physicsComponent;

      // Makes all primitive components visible or not
      std::shared_ptr<EngineGOProperty<bool>> mIsVisible;

      /* If Actor is disabled 
       - turn off all calculations for him and physics simulation
       also it won't be visible
       */
      std::shared_ptr<EngineGOProperty<bool>> mIsEnabled;

      std::vector<std::shared_ptr<Actor>> m_children;

      std::weak_ptr<Actor> m_parent;

      std::shared_ptr<InputComponent> m_inputComponent;

      std::shared_ptr<MovementComponent> m_movementComponent;

      std::vector<std::shared_ptr<Tweener>> mTweeners;

      std::weak_ptr<Scene> mSceneOwner;

   public:

      std::vector<std::shared_ptr<EngineCore::Component>> m_allComponents;

      Actor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

      virtual ~Actor();

      std::weak_ptr<Actor> GetWeakFromThis();

      // Tick is executed on game thread
      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      virtual void ChangeTweenerState(const std::string& tweenerName, const std::string& stateName);

      virtual void OnLevelInit();

      virtual void PostLevelInit();

      virtual void PostPhysicsInitialize();

      virtual void PostPlayLevelFinished();

      void AddComponent(std::shared_ptr<Component> component);

      void RemoveComponent(std::shared_ptr<EngineCore::Component> component);

      void RemoveMovementComponent();

      void RemoveInputComponent();

      void SetIsEnabled(bool isEnabled);

      void SetIsVisible(bool isVisible);

      bool GetIsVisible() const;

      bool IsEnabled() const;

      void SetParent(const std::weak_ptr<Actor>& actor);

      void SetScene(std::weak_ptr<Scene> sceneOwner);

      std::weak_ptr<Actor> GetParent() const;

      std::weak_ptr<Actor> GetBaseParent() const;

      std::string GetName() const;

      std::weak_ptr<Scene> GetSceneOwner() const;

      bool HasGameObjectIdInHierarchy(const uint64_t id) const;

      void AddChild(std::shared_ptr<Actor> actor);

      void RemoveChild(const std::shared_ptr<Actor>& actor);

      virtual void AttachTweener(std::shared_ptr<Tweener> tweener);

      const std::vector<std::shared_ptr<Tweener>>& GetTweeners() const;

      std::shared_ptr<Tweener> GetTweenerByName(const std::string& name) const;

      std::shared_ptr<EngineCore::SceneComponent> GetRootComponent() const;

      std::shared_ptr<EngineCore::SceneComponent> GetBaseRootComponent() const;

      std::shared_ptr<InputComponent> GetInputComponent() const;

      std::shared_ptr<MovementComponent> GetMovementComponent() const;

      std::shared_ptr<PhysicsComponent> GetPhysicsComponent() const;

      // If root component has dirty transformation -> update it and all attached actors + children components
      void UpdateTransform();

      // If components from list is scene component -> check if it has dirty transformation, and if it does -> update it
      void UpdateComponentsTransform(const bool bForceUpdate);

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
   };

}

