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

   Actor::Actor(const std::string &gameObjectName)
       : GameObject(gameObjectName),
         m_rootComponent(nullptr),
         m_physicsComponent(nullptr),
         mIsVisible(std::make_shared<EngineGOProperty<bool>>(true,
                                                             "p_isVisible",
                                                             std::make_unique<typename EngineGOProperty<bool>::Action_t>([=](const bool &visibility)
                                                                                                                         { SyncComponentsVisibility(visibility); }))),
         mIsEnabled(std::make_shared<EngineGOProperty<bool>>(true,
                                                             "p_isEnabled")),
         m_inputComponent(),
         m_movementComponent(),
         mTweener(nullptr),
         m_parent()
   {
      AddEngineProperty(mIsVisible);
      AddEngineProperty(mIsEnabled);
   }

   Actor::Actor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
       : GameObject(gameObjectName),
         m_rootComponent(rootComponent),
         m_physicsComponent(nullptr),
         mIsVisible(std::make_shared<EngineGOProperty<bool>>(true,
                                                             "p_isVisible",
                                                             std::make_unique<typename EngineGOProperty<bool>::Action_t>([=](const bool &visibility)
                                                                                                                         { SyncComponentsVisibility(visibility); }))),
         mIsEnabled(std::make_shared<EngineGOProperty<bool>>(true,
                                                             "p_isEnabled")),
         m_inputComponent(),
         m_movementComponent(),
         mTweener(nullptr),
         m_parent()
   {
      assert(m_rootComponent);
      m_rootComponent->bIsRootComponent = true;

      AddEngineProperty(mIsVisible);
      AddEngineProperty(mIsEnabled);
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

      for (const auto &childSp : m_children)
      {
         childSp->PostPhysicsInitialize();
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

      for (const auto &childSp : m_children)
      {
         childSp->PostLevelInit();
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
      data.RootCompTranslation = m_rootComponent ? m_rootComponent->GetTranslation() : glm::vec3();
      const glm::vec3 eulerAngles = m_rootComponent ? EngineMath::QuatToEulerAngles(m_rootComponent->GetRotator()) : glm::vec3(0);

      data.RootCompRotation = glm::vec3(RAD_TO_DEG(eulerAngles.x), RAD_TO_DEG(eulerAngles.y), RAD_TO_DEG(eulerAngles.z));
      data.RootCompScale = m_rootComponent ? m_rootComponent->GetScale() : glm::vec3(1);

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
               if ((component->GetComponentType() & eComponentType::SCENE_COMPONENT) == eComponentType::SCENE_COMPONENT)
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
      mIsVisible->SetValue(isVisible);
   }

   void Actor::SyncComponentsVisibility(bool isVisible)
   {
      assert(("Actor must have components.", m_allComponents.size() > 0));

      for (std::shared_ptr<Component> component : m_allComponents)
      {
         if ((component->GetComponentType() & eComponentType::PRIMITIVE_COMPONENT) == eComponentType::PRIMITIVE_COMPONENT)
         {
            std::static_pointer_cast<PrimitiveComponent>(component)->SetIsVisible(isVisible);
         }
      }

      for (const auto &spChild : m_children)
      {
         spChild->SetIsVisible(isVisible);
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
            if ((component->GetComponentType() & eComponentType::SCENE_COMPONENT) == eComponentType::SCENE_COMPONENT)
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
      if (mTweener && mIsEnabled)
      {
         mTweener->NotifyStateChangedObservers(); // if state was changed and is pending to notify - firstly do it
         mTweener->ChangeState(stateName);
      }
   }

   void Actor::Tick(const float deltaTime)
   {
      if (!mIsEnabled)
         return;

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

      for (const auto &childSp : m_children)
      {
         // tick all attached actors
         childSp->Tick(deltaTime);
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
         mTweener->NotifyStateChangedObservers();
      }
   }

   void Actor::AddComponent(std::shared_ptr<EngineCore::Component> component)
   {
      component->SetOwner(this->GetWeakFromThis());

      const uint64_t componentType = component->GetComponentType();

      if ((componentType & eComponentType::MOVEMENT_COMPONENT) == eComponentType::MOVEMENT_COMPONENT)
      {
         m_movementComponent = std::static_pointer_cast<MovementComponent>(component);
      }
      else if ((componentType & eComponentType::INPUT_COMPONENT) == eComponentType::INPUT_COMPONENT)
      {
         m_inputComponent = std::static_pointer_cast<InputComponent>(component);
      }
      else if ((componentType & eComponentType::PHYSICS_COMPONENT) == eComponentType::PHYSICS_COMPONENT)
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

   void Actor::AddChild(std::shared_ptr<Actor> actor)
   {
      actor->SetParent(GetWeakFromThis());
      m_children.push_back(actor);
   }

   void Actor::RemoveChild(const std::shared_ptr<Actor> &actor)
   {
      const auto actorIt = std::find_if(m_children.begin(), m_children.end(), [&](const auto &childSp)
                                        { return actor->GetName() == childSp->GetName(); });

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
      if (isEnabled != mIsEnabled->GetValue())
      {
         mIsEnabled->SetValue(isEnabled);

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

         for (const auto &spChild : m_children)
         {
            spChild->SetIsEnabled(isEnabled);
         }
      }
   }

   bool Actor::GetIsVisible() const
   {
      return mIsVisible->GetValue();
   }

   bool Actor::IsEnabled() const
   {
      return mIsEnabled->GetValue();
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
