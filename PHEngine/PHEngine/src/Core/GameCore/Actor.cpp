#include "Actor.h" 
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/Components/ComponentType.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeData.h"
#include "Core/GameCore/Scene.h"

#include <glm/gtc/quaternion.hpp>

namespace Game
{

   Actor::Actor(const std::string& gameObjectName, std::shared_ptr<Game::SceneComponent> rootComponent)
      : GameObject(gameObjectName)
      , m_rootComponent(rootComponent)
      , m_physicsComponent(nullptr)
      , mIsVisible(true)
      , m_inputComponent(nullptr)
      , m_movementComponent(nullptr)
      , mStateMachine(nullptr)
      , m_parent(nullptr)
   {
      assert(m_rootComponent);

      m_rootComponent->bIsRootComponent = true;
   }

	Actor::~Actor()
	{
	}

   void Actor::PostPhysicsInitialize()
   {
      if (m_physicsComponent)
      {
         m_physicsComponent->PostPhysicsInit();
      }

      for (const auto& child : m_children)
      {
         child->PostPhysicsInitialize();
      }
   }

   void Actor::PostLevelInit()
   {
      if (mStateMachine)
         mStateMachine->InitRootState();

      for (const auto& comp : m_allComponents)
      {
         comp->PostLevelInit();
      }

      for (auto& actor : m_children)
      {
         // tick all attached actors
         actor->PostLevelInit();
      }

      if (m_physicsComponent)
         m_physicsComponent->PostLevelInit();

      if (m_inputComponent)
         m_inputComponent->PostLevelInit();

      if (m_movementComponent)
         m_movementComponent->PostLevelInit();
   }

   void Actor::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      SerializeDataActor data;
      data.ActorName = GameObject::GameObjectName;
      data.RootCompTranslation = m_rootComponent->GetTranslation();

      constexpr float radToDeg = 180.f / 3.14159f;
      data.RootCompRotation = glm::eulerAngles(m_rootComponent->GetRotator()) * radToDeg;
      data.RootCompScale = m_rootComponent->GetScale();

      dataContainer.Actors.emplace_back(data);

      for (const auto& component : m_allComponents)
      {
         component->CollectDataForSerialization(dataContainer);
      }

      if (m_physicsComponent)
      {
         m_physicsComponent->CollectDataForSerialization(dataContainer);
      }

      if (m_inputComponent)
      {
         m_inputComponent->CollectDataForSerialization(dataContainer);
      }

      if (mStateMachine)
      {
         mStateMachine->CollectDataForSerialization(dataContainer);
      }
   }
  
	void Actor::UpdateRootComponentTransform()
	{
		if (m_rootComponent)
		{
			// Root component and all attached objects to this actor must update their transforms

			if (m_rootComponent->GetIsTransformationDirty())
			{
				// Update root component with parent transform matrix
				{
					glm::mat4 parentRelativeMatrix(1);	// identity matrix
       
					if (m_parent)
						parentRelativeMatrix = m_parent->GetRootComponent()->GetRelativeMatrix();
               
					m_rootComponent->UpdateRelativeMatrix(parentRelativeMatrix);
				}

				// Update all components that have transformation
				glm::mat4 rootRelativeMatrix = m_rootComponent->GetRelativeMatrix();
				for (auto& component : m_allComponents)
				{
					if ((component->GetComponentType() & ComponentType::SCENE_COMPONENT) == ComponentType::SCENE_COMPONENT)
					{
						SceneComponent* sceneComp = static_cast<SceneComponent*>(component.get());
						sceneComp->UpdateRelativeMatrix(rootRelativeMatrix);
					}
				}
			}
			else // If root component wasn't updated then just check if component has dirty transform
			{
				UpdateComponentsTransform();
			}
		}
      else
      {
         UpdateComponentsTransform();
      }
	}

   void Actor::SetIsVisible(bool isVisible)
   {
      assert(("Actor must have components.", m_allComponents.size() > 0));

      if (isVisible != mIsVisible)
      {
         mIsVisible = isVisible;
         for (std::shared_ptr<Component> component : m_allComponents)
         {
            if ((component->GetComponentType() & ComponentType::PRIMITIVE_COMPONENT) == ComponentType::PRIMITIVE_COMPONENT)
            {
               static_cast<PrimitiveComponent*>(component.get())->SetIsVisible(isVisible);
            }
         }

         for (const auto& childActor : m_children)
         {
            childActor->SetIsVisible(isVisible);
         }
      }
   }

	void Actor::UpdateComponentsTransform() 
	{
		if (m_allComponents.size() > 0)
		{
			glm::mat4 parentRelativeMatrix(1);	// identity matrix

			if (m_parent)
				parentRelativeMatrix = m_parent->GetRootComponent()->GetRelativeMatrix();

			// Update all components that have transformation

         if (m_rootComponent)
         {
            parentRelativeMatrix = m_rootComponent->GetRelativeMatrix();
         }
       
			for (auto& component : m_allComponents)
			{
				if ((component->GetComponentType() & ComponentType::SCENE_COMPONENT) == ComponentType::SCENE_COMPONENT)
				{
					SceneComponent* sceneComp = static_cast<SceneComponent*>(component.get());
					if (sceneComp->GetIsTransformationDirty())
					{
						sceneComp->UpdateRelativeMatrix(parentRelativeMatrix);
					}
				}
			}
		}
	}

   void Actor::RemoveComponentIndexOffset(size_t removedProxyIndex)
   {
      for (auto& childActor : m_children)
      {
         RemoveComponentIndexOffset(removedProxyIndex);
      }

      for (auto& component : m_allComponents)
      {
         if ((component->GetComponentType() & ComponentType::PRIMITIVE_COMPONENT) == ComponentType::PRIMITIVE_COMPONENT)
         {
            PrimitiveComponent* compPtr = static_cast<PrimitiveComponent*>(component.get());
            if (compPtr->PrimitiveProxyComponentId > removedProxyIndex)
               --compPtr->PrimitiveProxyComponentId;
         }
      }
   }

   void Actor::ChangeState(const std::string& stateName)
   {
      if (mStateMachine)
         mStateMachine->ChangeState(stateName);
   }

	void Actor::Tick(const float deltaTime)
	{
      UpdateRootComponentTransform();

      // Update physics
      if (m_physicsComponent)
      {
         m_physicsComponent->Tick(deltaTime);
      }

		m_rootComponent->Tick(deltaTime);

		for (auto& component : m_allComponents)
		{
			// tick all children components
			component->Tick(deltaTime);
		}

		for (auto& actor : m_children)
		{
			// tick all attached actors
			actor->Tick(deltaTime);
		}

      if (m_inputComponent)
         m_inputComponent->Tick(deltaTime);

      if (m_movementComponent)
         m_movementComponent->Tick(deltaTime);

      if (mStateMachine)
         mStateMachine->Tick(deltaTime);
	}

	void Actor::AddComponent(std::shared_ptr<Game::Component> component)
	{
      component->SetOwner(this);

      if (component->GetComponentType() == ComponentType::CHARACTER_MOVEMENT_COMPONENT)
      {
         m_movementComponent = std::static_pointer_cast<CharacterMovementComponent>(component);
      }
      else if (component->GetComponentType() == ComponentType::INPUT_COMPONENT)
      {
         m_inputComponent = std::static_pointer_cast<InputComponent>(component);
      }
      else if (component->GetComponentType() == ComponentType::PHYSICS_COMPONENT)
      {
         m_physicsComponent = std::static_pointer_cast<PhysicsComponent>(component);
      }
      else
      {
         m_allComponents.push_back(component);
      }
	}

	void Actor::RemoveComponent(std::shared_ptr<Game::Component> component)
	{
		auto componentIt = std::find(m_allComponents.begin(), m_allComponents.end(), component);
		if (componentIt != m_allComponents.end())
		{
         component->RemoveOwner();
			m_allComponents.erase(componentIt);
		}
	}

   void Actor::RemoveMovementComponent()
   {
      m_movementComponent = std::shared_ptr<CharacterMovementComponent>(nullptr);
   }

   void Actor::RemoveInputComponent()
   {
      m_inputComponent = std::shared_ptr<InputComponent>(nullptr);
   }

	void Actor::SetParent(Actor* actor)
	{
		m_parent = actor;
	}

   void Actor::SetScene(std::weak_ptr<Scene> sceneOwner)
   {
      mSceneOwner = sceneOwner;
   }

   Actor* Actor::GetParent() const
   {
      return m_parent;
   }

   std::weak_ptr<Scene> Actor::GetSceneOwner() const
   {
      return mSceneOwner;
   }

	void Actor::AttachActor(std::shared_ptr<Actor> actor)
	{
		actor->SetParent(this);
		m_children.push_back(actor);
	}

	void Actor::DetachActor(std::shared_ptr<Actor> actor)
	{
		auto actorIt = std::find(m_children.begin(), m_children.end(), actor);
		if (actorIt != m_children.end())
		{
			actor->SetParent(nullptr);
			m_children.erase(actorIt);
		}
	}

   void Actor::AttachStateMachine(std::shared_ptr<StateMachine> fsm)
   {
      assert((!mStateMachine, "Current state machine member was already attached."));
      mStateMachine = fsm;
      mStateMachine->SetParentActor(this);
   }

   std::shared_ptr<StateMachine> Actor::GetStateMachine() const
   {
      return mStateMachine;
   }

   std::shared_ptr<SceneComponent> Actor::GetBaseRootComponent() const
   {
      std::shared_ptr<SceneComponent> rootComponent;

      const Actor* ptrActor = this;
      while (m_parent)
      {
         ptrActor = m_parent;
      }

      rootComponent = ptrActor->GetRootComponent();
      return rootComponent;
   }

   std::string Actor::GetName() const {
      return GameObjectName;
   }

}
