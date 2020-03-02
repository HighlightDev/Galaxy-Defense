#include "Actor.h"
#include "Core/GameCore/Components/PrimitiveComponent.h"
#include "Core/GameCore/Components/ComponentType.h"

namespace Game
{

	Actor::Actor(const std::string& name, std::shared_ptr<Game::SceneComponent> rootComponent)
      : mName(name)
      , m_rootComponent(rootComponent)
      , m_inputComponent(nullptr)
      , m_movementComponent(nullptr)
	{
      m_parent = nullptr;
      m_rootComponent->bIsRootComponent = true;
	}

	Actor::~Actor()
	{
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
					if ((component->GetComponentType() & SCENE_COMPONENT) == SCENE_COMPONENT)
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

	void Actor::UpdateComponentsTransform() 
	{
		if (m_allComponents.size() > 0)
		{
			glm::mat4 parentRelativeMatrix(1);	// identity matrix

			if (m_parent)
				parentRelativeMatrix = m_parent->GetRootComponent()->GetRelativeMatrix();

			// Update all components that have transformation
         glm::mat4 rootRelativeMatrix(1);
         rootRelativeMatrix *= parentRelativeMatrix;

         if (m_rootComponent)
         {
            rootRelativeMatrix = std::move(m_rootComponent->GetRelativeMatrix());
         }
         
			for (auto& component : m_allComponents)
			{
				if ((component->GetComponentType() & SCENE_COMPONENT) == SCENE_COMPONENT)
				{
					SceneComponent* sceneComp = static_cast<SceneComponent*>(component.get());
					if (sceneComp->GetIsTransformationDirty())
					{
						sceneComp->UpdateRelativeMatrix(rootRelativeMatrix);
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
         if ((component->GetComponentType() & PRIMITIVE_COMPONENT) == PRIMITIVE_COMPONENT)
         {
            PrimitiveComponent* compPtr = static_cast<PrimitiveComponent*>(component.get());
            if (compPtr->PrimitiveProxyComponentId > removedProxyIndex)
               --compPtr->PrimitiveProxyComponentId;
         }
      }
   }

	void Actor::Tick(const float deltaTime)
	{
		UpdateRootComponentTransform();

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
	}

   void Actor::AddInputComponent(std::shared_ptr<Game::Component> inputComponent)
   {
      inputComponent->SetOwner(this);
      m_inputComponent = std::static_pointer_cast<InputComponent>(inputComponent);
   }

   void Actor::AddMovementComponent(std::shared_ptr<Game::Component> movementComponent)
   {
      movementComponent->SetOwner(this);
      m_movementComponent = std::static_pointer_cast<MovementComponent>(movementComponent);
   }

	void Actor::AddComponent(std::shared_ptr<Game::Component> component)
	{
      component->SetOwner(this);
		m_allComponents.push_back(component);
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
      m_movementComponent = std::shared_ptr<MovementComponent>(nullptr);
   }

   void Actor::RemoveInputComponent()
   {
      m_inputComponent = std::shared_ptr<InputComponent>(nullptr);
   }

	void Actor::SetParent(Actor* actor)
	{
		m_parent = actor;
	}

   Actor* Actor::GetParent() const
   {
      return m_parent;
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
      return mName;
   }

   void Actor::SetName(const std::string& name)
   {
      mName = name;
   }

}
