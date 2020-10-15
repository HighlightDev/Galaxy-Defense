#pragma once

#include <vector>
#include <memory>
#include <glm/mat4x4.hpp>

#include "GameObject.h"
#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/CharacterMovementComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/StateMachine/StateMachine.h"

using namespace EnginePhysics;

namespace Game
{

	class Actor 
      : public GameObject
      , public ITickable
	{
	private:

		std::shared_ptr<SceneComponent> m_rootComponent;

      std::shared_ptr<PhysicsComponent> m_physicsComponent;

	protected:

      // Makes all primitive components visible or not
      bool mIsVisible;

      std::vector<std::shared_ptr<Actor>> m_children;

		Actor* m_parent;

      std::shared_ptr<InputComponent> m_inputComponent;

      std::shared_ptr<CharacterMovementComponent> m_movementComponent;

      std::shared_ptr<StateMachine> mStateMachine;

	public:

      std::vector<std::shared_ptr<Game::Component>> m_allComponents;

		Actor(const std::string& gameObjectName, std::shared_ptr<Game::SceneComponent> rootComponent);

		virtual ~Actor();

      void PostPhysicsInitialize();

		// Tick is executed on game thread
		virtual void Tick(const float deltaTime) override;

      virtual void ChangeState(const std::string& stateName);

      virtual void PostLevelInit();

      void AddComponent(std::shared_ptr<Component> component);

		void RemoveComponent(std::shared_ptr<Game::Component> component);

      void RemoveMovementComponent();

      void RemoveInputComponent();

      void SetIsVisible(bool isVisible);

      inline bool IsVisible() const {
         return mIsVisible;
      }

		void SetParent(Actor* actor);

      Actor* GetParent() const;

      std::string GetName() const;

		void AttachActor(std::shared_ptr<Actor> actor);

		void DetachActor(std::shared_ptr<Actor> actor);

      void AttachStateMachine(std::shared_ptr<StateMachine> fsm);

      std::shared_ptr<StateMachine> GetStateMachine() const;

		inline std::shared_ptr<Game::SceneComponent> GetRootComponent() const {

			return m_rootComponent;
		}

      std::shared_ptr<Game::SceneComponent> GetBaseRootComponent() const;

      inline std::shared_ptr<InputComponent> GetInputComponent() const
      {
         return m_inputComponent;
      }

      inline std::shared_ptr<CharacterMovementComponent> GetMovementComponent() const
      {
         return m_movementComponent;
      }

      inline std::shared_ptr<PhysicsComponent> GetPhysicsComponent() const {

         return m_physicsComponent;
      }

		// If root component has dirty transformation -> update it and all attached actors + children components
		void UpdateRootComponentTransform();

		// If components from list is scene component -> check if it has dity transformation, and if it does -> update it
		void UpdateComponentsTransform();

      // When primitive component is removed, all primitive components which are alive and have index greater than removed component's index should do proxy index offset (-1)
      void RemoveComponentIndexOffset(size_t removedProxyIndex);

      template <typename ComponentT>
      std::shared_ptr<ComponentT> GetComponent(ComponentType type)
      {
         std::shared_ptr<ComponentT> result(nullptr);

         assert(m_allComponents.size());

         if (auto it = std::find_if(m_allComponents.begin(), m_allComponents.end(), [&](auto& compItem) { return compItem->GetComponentType() == type; });
            m_allComponents.end() != it)
         {
            result = std::static_pointer_cast<ComponentT>(*it);
         }

         return result;
      }
	};

}

