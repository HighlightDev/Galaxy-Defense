#include "Actor.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/Components/ComponentType.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeData.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/EngineMath.h"

#include <functional>

using namespace EngineMath;

namespace EngineCore
{

   Actor::Actor(const std::string &gameObjectName, std::shared_ptr<EngineCore::SceneComponent> rootComponent)
       : GameObject(gameObjectName),
         m_rootComponent(rootComponent),
         m_physicsComponent(nullptr),
         mIsVisible(EngineGOProperty<bool>(true,
                                           "IsVisible",
                                           std::make_unique<typename EngineGOProperty<bool>::Action_t>([=](const bool &visibility)
                                                                                                       { SyncComponentsVisibility(visibility); }))),
         mIsEnabled(true),
         m_inputComponent(),
         m_movementComponent(),
         mTweener(nullptr),
         m_parent()
   {
      assert(m_rootComponent);

      AddEngineProperty(mIsVisible);

      m_rootComponent->bIsRootComponent = true;
   }

   Actor::~Actor()
   {
   }

   std::weak_ptr<Actor> Actor::GetWeakFromThis()
   {
      return shared_from_this();
   }

   void Actor::PostPhysicsInitialize()
   {
      if (m_physicsComponent)
      {
         m_physicsComponent->PostPhysicsInit();
      }

      for (const auto &childWp : m_children)
      {
         if (const auto &childSp = childWp.lock())
         {
            childSp->PostPhysicsInitialize();
         }
      }
   }

   void Actor::PostPlayLevelFinished()
   {
   }

   void Actor::PostLevelInit()
   {
      if (mTweener)
      {
         mTweener->InitRootState();
      }

      for (const auto &comp : m_allComponents)
      {
         comp->PostLevelInit();
      }

      for (const auto &childWp : m_children)
      {
         if (const auto &childSp = childWp.lock())
         {
            childSp->PostLevelInit();
         }
      }

      if (m_physicsComponent)
      {
         m_physicsComponent->PostLevelInit();
      }

      if (m_inputComponent)
      {
         m_inputComponent->PostLevelInit();
      }

      if (m_movementComponent)
      {
         m_movementComponent->PostLevelInit();
      }
   }

   void Actor::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      SerializeDataActor data;
      data.ActorName = GameObject::GameObjectName;
      data.RootCompTranslation = m_rootComponent->GetTranslation();
      const glm::vec3 eulerAngles = EngineMath::QuatToEulerAngles(m_rootComponent->GetRotator());

      data.RootCompRotation = glm::vec3(RAD_TO_DEG(eulerAngles.x), RAD_TO_DEG(eulerAngles.y), RAD_TO_DEG(eulerAngles.z));
      data.RootCompScale = m_rootComponent->GetScale();

      dataContainer.Actors.emplace_back(data);

      for (const auto &component : m_allComponents)
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

      if (m_movementComponent)
      {
         m_movementComponent->CollectDataForSerialization(dataContainer);
      }

      if (mTweener)
      {
         mTweener->CollectDataForSerialization(dataContainer);
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
               glm::mat4 parentRelativeMatrix(1); // identity matrix
               if (const auto &spParent = m_parent.lock())
               {
                  parentRelativeMatrix = spParent->GetRootComponent()->GetRelativeMatrix();
               }

               m_rootComponent->UpdateRelativeMatrix(parentRelativeMatrix);
            }

            // Update all components that have transformation
            glm::mat4 rootRelativeMatrix = m_rootComponent->GetRelativeMatrix();

            for (auto &component : m_allComponents)
            {
               if ((component->GetComponentType() & ComponentType::SCENE_COMPONENT) == ComponentType::SCENE_COMPONENT)
               {
                  SceneComponent *sceneComp = static_cast<SceneComponent *>(component.get());
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
      mIsVisible.SetValue(isVisible);
   }

   void Actor::SyncComponentsVisibility(bool isVisible)
   {
      assert(("Actor must have components.", m_allComponents.size() > 0));

      for (std::shared_ptr<Component> component : m_allComponents)
      {
         if ((component->GetComponentType() & ComponentType::PRIMITIVE_COMPONENT) == ComponentType::PRIMITIVE_COMPONENT)
         {
            std::static_pointer_cast<PrimitiveComponent>(component)->SetIsVisible(isVisible);
         }
      }

      for (const auto &wpChild : m_children)
      {
         if (const auto &spChild = wpChild.lock())
         {
            spChild->SetIsVisible(isVisible);
         }
      }
   }

   void Actor::UpdateComponentsTransform()
   {
      if (m_allComponents.size() > 0)
      {
         glm::mat4 parentRelativeMatrix(1); // identity matrix

         if (const auto &spParent = m_parent.lock())
         {
            parentRelativeMatrix = spParent->GetRootComponent()->GetRelativeMatrix();
         }

         // Update all components that have transformation

         if (m_rootComponent)
         {
            parentRelativeMatrix = m_rootComponent->GetRelativeMatrix();
         }

         for (auto &component : m_allComponents)
         {
            if ((component->GetComponentType() & ComponentType::SCENE_COMPONENT) == ComponentType::SCENE_COMPONENT)
            {
               SceneComponent *sceneComp = static_cast<SceneComponent *>(component.get());
               if (sceneComp->GetIsTransformationDirty())
               {
                  sceneComp->UpdateRelativeMatrix(parentRelativeMatrix);
               }
            }
         }
      }
   }

   void Actor::ChangeTweenState(const std::string &stateName)
   {
      if (mTweener)
         mTweener->ChangeState(stateName);
   }

   void Actor::Tick(const float deltaTime)
   {
      UpdateRootComponentTransform();

      // Update physics
      if (m_physicsComponent && m_physicsComponent->IsEnabled())
      {
         m_physicsComponent->Tick(deltaTime);
      }

      if (m_rootComponent && m_rootComponent->IsEnabled())
      {
         m_rootComponent->Tick(deltaTime);
      }

      for (auto &component : m_allComponents)
      {
         if (component->IsEnabled())
         {
            // tick all children components
            component->Tick(deltaTime);
         }
      }

      for (const auto &childWp : m_children)
      {
         if (const auto &childSp = childWp.lock())
         {
            // tick all attached actors
            childSp->Tick(deltaTime);
         }
      }

      if (m_inputComponent && m_inputComponent->IsEnabled())
      {
         m_inputComponent->Tick(deltaTime);
      }

      if (m_movementComponent && m_movementComponent->IsEnabled())
      {
         m_movementComponent->Tick(deltaTime);
      }

      if (mTweener && mIsEnabled)
      {
         mTweener->Tick(deltaTime);
      }
   }

   void Actor::AddComponent(std::shared_ptr<EngineCore::Component> component)
   {
      component->SetOwner(this->GetWeakFromThis());

      const uint64_t componentType = component->GetComponentType();

      if ((componentType & ComponentType::MOVEMENT_COMPONENT) == ComponentType::MOVEMENT_COMPONENT)
      {
         m_movementComponent = std::static_pointer_cast<MovementComponent>(component);
      }
      else if ((componentType & ComponentType::INPUT_COMPONENT) == ComponentType::INPUT_COMPONENT)
      {
         m_inputComponent = std::static_pointer_cast<InputComponent>(component);
      }
      else if ((componentType & ComponentType::PHYSICS_COMPONENT) == ComponentType::PHYSICS_COMPONENT)
      {
         m_physicsComponent = std::static_pointer_cast<PhysicsComponent>(component);
      }
      else
      {
         m_allComponents.push_back(component);
      }
   }

   void Actor::RemoveComponent(std::shared_ptr<EngineCore::Component> component)
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

   void Actor::SetParent(const std::weak_ptr<Actor> &actor)
   {
      m_parent = actor;
   }

   void Actor::SetScene(std::weak_ptr<Scene> sceneOwner)
   {
      mSceneOwner = sceneOwner;
   }

   std::weak_ptr<Actor> Actor::GetParent() const
   {
      return m_parent;
   }

   std::weak_ptr<Scene> Actor::GetSceneOwner() const
   {
      return mSceneOwner;
   }

   void Actor::AttachActor(std::shared_ptr<Actor> actor)
   {
      actor->SetParent(GetWeakFromThis());
      m_children.push_back(actor);
   }

   void Actor::DetachActor(std::shared_ptr<Actor> actor)
   {
      const auto actorIt = std::find_if(m_children.begin(), m_children.end(), [&](const auto &childWp)
                                        {
         if (const auto& childSp = childWp.lock())
         {
            return actor->GetName() == childSp->GetName();
         } 
         return false; });

      if (actorIt != m_children.end())
      {
         actor->SetParent(std::weak_ptr<Actor>());
         m_children.erase(actorIt);
      }
   }

   void Actor::AttachTweener(std::shared_ptr<Tweener> tweener)
   {
      assert((!mTweener, "Tweener was already attached."));
      mTweener = tweener;
      mTweener->SetParentActor(this);
   }

   std::shared_ptr<Tweener> Actor::GetTweener() const
   {
      return mTweener;
   }

   void Actor::SetIsEnabled(bool isEnabled)
   {
      assert(("Actor must have components.", m_allComponents.size() > 0));

      if (isEnabled != mIsEnabled)
      {
         mIsEnabled = isEnabled;

         for (const auto &component : m_allComponents)
         {
            component->SetIsEnabled(isEnabled);
         }

         if (m_inputComponent)
         {
            m_inputComponent->SetIsEnabled(isEnabled);
         }

         if (m_movementComponent)
         {
            m_movementComponent->SetIsEnabled(isEnabled);
         }

         if (m_physicsComponent)
         {
            m_physicsComponent->SetIsEnabled(isEnabled);
         }

         for (const auto &wpChild : m_children)
         {
            if (const auto &spChild = wpChild.lock())
            {
               spChild->SetIsEnabled(isEnabled);
            }
         }
      }
   }

   bool Actor::GetIsVisible() const
   {
      return mIsVisible;
   }

   bool Actor::IsEnabled() const
   {
      return mIsEnabled;
   }

   std::shared_ptr<SceneComponent> Actor::GetBaseRootComponent() const
   {
      std::shared_ptr<const Actor> baseParent = shared_from_this();
      auto parentWp = m_parent;

      while (const auto &spParent = parentWp.lock())
      {
         baseParent = spParent;
         parentWp = spParent->GetParent();
      }

      return baseParent->GetRootComponent();
   }

   std::string Actor::GetName() const
   {
      return GameObjectName;
   }

   std::shared_ptr<EngineCore::SceneComponent> Actor::GetRootComponent() const
   {
      return m_rootComponent;
   }

   std::shared_ptr<InputComponent> Actor::GetInputComponent() const
   {
      return m_inputComponent;
   }

   std::shared_ptr<MovementComponent> Actor::GetMovementComponent() const
   {
      return m_movementComponent;
   }

   std::shared_ptr<PhysicsComponent> Actor::GetPhysicsComponent() const
   {
      return m_physicsComponent;
   }

}
