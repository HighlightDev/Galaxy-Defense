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
   Actor::Actor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
       : EngineObject(gameObjectName),
         m_rootComponent(rootComponent),
         m_physicsComponent(nullptr),
         mIsVisible(std::make_shared<EngineObjectProperty<bool>>(true,
                                                             "p_isVisible",
                                                             [=](const bool &visibility)
                                                             { SetIsVisible(visibility); })),
         mIsEnabled(std::make_shared<EngineObjectProperty<bool>>(true,
                                                             "p_isEnabled",
                                                             [=](const bool &isEnabled)
                                                             { SetIsEnabled(isEnabled); })),
         m_inputComponent(),
         m_movementComponent(),
         mTweeners(),
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

   void Actor::CleanUp()
   {
      for (const auto& component : m_allComponents)
      {
         component->CleanUp();
      }
   }

   void Actor::OnLevelInit()
   {
   }

   void Actor::PostLevelInit()
   {
      m_rootComponent->SetOwner(GetWeakFromThis());

      for (const auto &tweener : mTweeners)
      {
         tweener->InitRootState();
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

   bool Actor::HasEngineObjectIdInHierarchy(const uint64_t id) const
   {
      if (GetObjectId() == id)
         return true;

      for (const auto &child : m_children)
      {
         if (child->HasEngineObjectIdInHierarchy(id))
            return true;
      }

      return false;
   }

   void Actor::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      SerializeDataActor data;
      data.ActorName = EngineObject::EngineObjectName;
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

      for (const auto &tweener : mTweeners)
      {
         tweener->CollectDataForSerialization(dataContainer);
      }
   }

   void Actor::UpdateTransform()
   {
      assert(m_rootComponent);

      if (m_rootComponent->GetIsTransformationDirty())
      {
         glm::mat4 parentRelativeMatrix(1);
         if (const auto &spParent = m_parent.lock())
         {
            parentRelativeMatrix *= spParent->GetRootComponent()->GetRelativeMatrix();
         }

         m_rootComponent->UpdateRelativeMatrix(parentRelativeMatrix);
         UpdateComponentsTransform(true);

         for (const auto &child : m_children)
         {
            child->GetRootComponent()->SetIsTransformationDirty(true); // force to update children's transform
         }
      }
      else
      {
         UpdateComponentsTransform(false);
      }
   }

   void Actor::UpdateComponentsTransform(const bool bForceUpdate)
   {
      assert(m_rootComponent);

      if (m_allComponents.size())
      {
         auto parentRelativeMatrix = m_rootComponent->GetRelativeMatrix();

         for (auto &component : m_allComponents)
         {
            if ((component->GetComponentType() & eComponentType::SCENE_COMPONENT) == eComponentType::SCENE_COMPONENT)
            {
               auto sceneComp = std::static_pointer_cast<SceneComponent>(component);
               if (bForceUpdate || sceneComp->GetIsTransformationDirty())
               {
                  sceneComp->UpdateRelativeMatrix(parentRelativeMatrix);
               }
            }
         }
      }
   }

   void Actor::SetIsVisible(bool isVisible)
   {
      if (isVisible != mIsVisible->GetValue())
      {
         mIsVisible->SetValue(isVisible, false);
      }

      for (const auto &component : m_allComponents)
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

   void Actor::SetIsEnabled(bool isEnabled)
   {
      if (isEnabled != mIsEnabled->GetValue())
      {
         mIsEnabled->SetValue(isEnabled, false);
      }

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

   void Actor::ChangeTweenerState(const std::string &tweenerName, const std::string &stateName)
   {
      if (mIsEnabled->GetValue())
      {
         auto it = std::find_if(mTweeners.begin(), mTweeners.end(), [&](const auto &tweener)
                                { return tweener->GetTweenerName() == tweenerName; });
         assert(it != mTweeners.end());
         auto tweenerSp = *it;
         tweenerSp->NotifyStateChangedObservers(); // if state was changed and is pending to notify - firstly do it
         tweenerSp->ChangeState(stateName);
      }
   }

   void Actor::UnpausableTick(const float deltaTime)
   {
      if (m_physicsComponent)
      {
         m_physicsComponent->UnpausableTick(deltaTime);
      }

      if (m_rootComponent)
      {
         m_rootComponent->UnpausableTick(deltaTime);
      }

      for (auto &component : m_allComponents)
      {
         component->UnpausableTick(deltaTime);
      }

      for (const auto &childSp : m_children)
      {
         childSp->UnpausableTick(deltaTime);
      }

      if (m_inputComponent)
      {
         m_inputComponent->UnpausableTick(deltaTime);
      }

      if (m_movementComponent)
      {
         m_movementComponent->UnpausableTick(deltaTime);
      }

      if (mIsEnabled->GetValue())
      {
         for (const auto &tweener : mTweeners)
         {
            tweener->Tick(deltaTime);
            tweener->NotifyStateChangedObservers();
         }
      }
   }

   void Actor::Tick(const float deltaTime)
   {
      UpdateTransform();

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

      if (mIsEnabled->GetValue())
      {
         for (const auto &tweener : mTweeners)
         {
            tweener->Tick(deltaTime);
            tweener->NotifyStateChangedObservers();
         }
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

   std::weak_ptr<Actor> Actor::GetBaseParent() const
   {
      if (!m_parent.lock())
         return std::weak_ptr<Actor>();

      std::shared_ptr<Actor> baseParent;
      auto parentWp = m_parent;

      while (const auto &spParent = parentWp.lock())
      {
         baseParent = spParent;
         parentWp = spParent->GetParent();
      }

      return baseParent;
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

   std::shared_ptr<Actor> Actor::GetChildByObjectId(const uint64_t id) const
   {
      std::shared_ptr<Actor> resultChild;

      for (const auto &child : m_children)
      {
         if (child->GetObjectId() == id)
         {
            resultChild = child;
            break;
         }
         else
         {
            const auto &hierarchyChild = child->GetChildByObjectId(id);
            if (hierarchyChild)
            {
               resultChild = hierarchyChild;
               break;
            }
         }
      }

      return resultChild;
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

   void Actor::AttachTweener(std::shared_ptr<Tweener> newTweener)
   {
      auto it = std::find_if(mTweeners.begin(), mTweeners.end(), [=](const auto &tweener)
                             { return tweener->GetTweenerName() == newTweener->GetTweenerName(); });
      assert(it == mTweeners.end());
      newTweener->SetParentActor(this);
      mTweeners.emplace_back(newTweener);
   }

   const std::vector<std::shared_ptr<Tweener>> &Actor::GetTweeners() const
   {
      return mTweeners;
   }

   std::shared_ptr<Tweener> Actor::GetTweenerByName(const std::string &name) const
   {
      auto it = std::find_if(mTweeners.cbegin(), mTweeners.cend(), [&](const auto &tweener)
                             { return name == tweener->GetTweenerName(); });

      return it != mTweeners.cend() ? *it : nullptr;
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
      auto rootComponent = GetRootComponent();

      if (auto baseParentSp = GetBaseParent().lock())
      {
         rootComponent = baseParentSp->GetRootComponent();
      }

      return rootComponent;
   }

   std::string Actor::GetName() const
   {
      return EngineObjectName;
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
