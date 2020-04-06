#pragma once

#include <vector>
#include <memory>
#include <glm/mat4x4.hpp>

#include "GameObject.h"
#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/MovementComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/ITickable.h"

namespace Game
{

	class Actor 
      : public GameObject
      , public ITickable
	{
	private:

      std::string mName;
		std::shared_ptr<SceneComponent> m_rootComponent;
      std::shared_ptr<PhysicsComponent> m_physicsComponent;

	protected:

      std::vector<std::shared_ptr<Actor>> m_children;

		Actor* m_parent;

      std::shared_ptr<InputComponent> m_inputComponent;

      std::shared_ptr<MovementComponent> m_movementComponent;

	public:

      std::vector<std::shared_ptr<Game::Component>> m_allComponents;

		Actor(const std::string& name, std::shared_ptr<Game::SceneComponent> rootComponent = nullptr);

		virtual ~Actor();

      void PostPhysicsInitialize();

		// Tick is executed on game thread
		virtual void Tick(const float deltaTime) override;

      void AddComponent(std::shared_ptr<Component> component);

		void RemoveComponent(std::shared_ptr<Game::Component> component);

      void RemoveMovementComponent();

      void RemoveInputComponent();

		void SetParent(Actor* actor);

      void SetName(const std::string& name);

      Actor* GetParent() const;

      std::string GetName() const;

		void AttachActor(std::shared_ptr<Actor> actor);

		void DetachActor(std::shared_ptr<Actor> actor);

		inline std::shared_ptr<Game::SceneComponent> GetRootComponent() const {

			return m_rootComponent;
		}

      std::shared_ptr<Game::SceneComponent> GetBaseRootComponent() const;

      inline std::shared_ptr<InputComponent> GetInputComponent() const
      {
         return m_inputComponent;
      }

      inline std::shared_ptr<MovementComponent> GetMovementComponent() const
      {
         return m_movementComponent;
      }

		// If root component has dirty transformation -> update it and all attached actors + children components
		void UpdateRootComponentTransform();

		// If components from list is scene component -> check if it has dity transformation, and if it does -> update it
		void UpdateComponentsTransform();

      // When primitive component is removed, all primitive components which are alive and have index greater than removed component's index should do proxy index offset (-1)
      void RemoveComponentIndexOffset(size_t removedProxyIndex);
	};

}

