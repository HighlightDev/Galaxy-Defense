#include "Actor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentType.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/EngineMath.h"

#include <functional>

using namespace EngineMath;

namespace EngineCore {
Actor::Actor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent)
    : EngineObject(gameObjectName)
    , m_rootComponent(rootComponent)
    , m_physicsComponent(nullptr)
    , mIsVisible(std::make_shared<EngineObjectProperty<bool>>(
          true, "p_isVisible", [this](const bool& visibility) { SetIsVisible(visibility); }))
    , mIsEnabled(std::make_shared<EngineObjectProperty<bool>>(
          true, "p_isEnabled", [this](const bool& isEnabled) { SetIsEnabled(isEnabled); }))
    , m_inputComponent()
    , m_movementComponent()
    , mTweeners()
    , m_parent()
{
    ext_assert(m_rootComponent, "Actor root component is null in Actor constructor");
    m_rootComponent->SetIsRootComponent(true);

    AddEngineProperty(mIsVisible);
    AddEngineProperty(mIsEnabled);
    LogInfo("Actor::ctor: id: ", GetObjectId(), ", name: ", GetName());
}

Actor::~Actor()
{
    LogInfo("Actor::dctor: id: ", GetObjectId(), ", name: ", GetName());
}

void Actor::PostPhysicsInitialize()
{
    for (const auto& childSp : m_children) {
        childSp->PostPhysicsInitialize();
    }
}

void Actor::PostPlayLevelFinished()
{
}

void Actor::CleanUp()
{
    LogInfo("Actor::CleanUp: id: ", GetObjectId(), ", name: ", GetName());
    for (const auto& tweener : mTweeners) {
        tweener->CleanUp();
    }
    mTweeners.clear();

    m_rootComponent->CleanUp();
    if (m_physicsComponent) {
        m_physicsComponent->CleanUp();
    }

    if (m_inputComponent) {
        m_inputComponent->CleanUp();
    }

    if (m_movementComponent) {
        m_movementComponent->CleanUp();
    }

    for (const auto& component : m_allComponents) {
        component->CleanUp();
    }
    m_allComponents.clear();

    for (const auto& childActor : m_children) {
        childActor->CleanUp();
    }
    m_children.clear();
}

void Actor::OnLevelInit()
{
}

void Actor::PostLevelInit()
{
    for (const auto& comp : m_allComponents) {
        comp->PostLevelInit();
    }

    for (const auto& childSp : m_children) {
        childSp->PostLevelInit();
    }

    if (m_physicsComponent) {
        m_physicsComponent->PostLevelInit();
    }

    if (m_inputComponent) {
        m_inputComponent->PostLevelInit();
    }

    if (m_movementComponent) {
        m_movementComponent->PostLevelInit();
    }
}

bool Actor::HasEngineObjectIdInHierarchy(const int32_t id) const
{
    if (GetObjectId() == id)
        return true;

    for (const auto& child : m_children) {
        if (child->HasEngineObjectIdInHierarchy(id))
            return true;
    }

    return false;
}

void Actor::UpdateTransform()
{
    ext_assert(m_rootComponent, "Actor root component is null in UpdateTransform");

    if (m_rootComponent->GetIsTransformationDirty()) {
        glm::mat4 parentWorldMatrix(1);
        if (const auto& spParent = m_parent.lock()) {
            parentWorldMatrix *= spParent->GetRootComponent()->GetWorldMatrix();
        }

        m_rootComponent->UpdateWorldMatrix(parentWorldMatrix);
        UpdateComponentsTransform(true);

        for (const auto& child : m_children) {
            child->GetRootComponent()->SetIsTransformationDirty(true); // force to update children's transform
        }
    } else {
        UpdateComponentsTransform(false);
    }
}

void Actor::UpdateComponentsTransform(const bool bForceUpdate)
{
    ext_assert(m_rootComponent, "Actor root component is null in UpdateComponentsTransform");

    if (m_allComponents.size()) {
        auto parentWorldMatrix = m_rootComponent->GetWorldMatrix();

        for (auto& component : m_allComponents) {
            if ((component->GetComponentType() & eComponentType::SCENE_COMPONENT) == eComponentType::SCENE_COMPONENT) {
                auto sceneComp = std::static_pointer_cast<SceneComponent>(component);
                if (bForceUpdate || sceneComp->GetIsTransformationDirty()) {
                    sceneComp->UpdateWorldMatrix(parentWorldMatrix);
                }
            }
        }
    }
}

void Actor::SetIsVisible(bool isVisible)
{
    if (isVisible != mIsVisible->GetValue()) {
        mIsVisible->SetValue(isVisible, false);
    }

    for (const auto& component : m_allComponents) {
        if ((component->GetComponentType() & eComponentType::PRIMITIVE_COMPONENT) == eComponentType::PRIMITIVE_COMPONENT) {
            std::static_pointer_cast<PrimitiveComponent>(component)->SetIsVisible(isVisible);
        }
    }

    for (const auto& spChild : m_children) {
        spChild->SetIsVisible(isVisible);
    }
}

void Actor::SetIsEnabled(bool isEnabled)
{
    if (isEnabled != mIsEnabled->GetValue()) {
        mIsEnabled->SetValue(isEnabled, false);
    }

    for (const auto& component : m_allComponents) {
        component->SetIsEnabled(isEnabled);
    }

    if (m_inputComponent) {
        m_inputComponent->SetIsEnabled(isEnabled);
    }

    if (m_movementComponent) {
        m_movementComponent->SetIsEnabled(isEnabled);
    }

    if (m_physicsComponent) {
        m_physicsComponent->SetIsEnabled(isEnabled);
    }

    for (const auto& spChild : m_children) {
        spChild->SetIsEnabled(isEnabled);
    }
}

void Actor::ChangeTweenerState(const std::string& tweenerName, const std::string& stateName)
{
    if (mIsEnabled->GetValue()) {
        auto it = std::find_if(
            mTweeners.begin(), mTweeners.end(), [&](const auto& tweener) { return tweener->GetTweenerName() == tweenerName; });
        if (it != mTweeners.end()) {
            auto tweenerSp = *it;
            tweenerSp->NotifyStateChangedObservers(); // if state was changed and is pending to notify - firstly do it
            tweenerSp->ChangeState(stateName);
            bLastTweenerChangeWasValid = true;
        } else {
            if (bLastTweenerChangeWasValid) {
                bLastTweenerChangeWasValid = false;
                LogInfo("Actor::ChangeTweenerState: Warning: missing tweener ", tweenerName);
            }
        }
    }
}

void Actor::UnpausableTick(const float deltaTimeSec)
{
    if (m_physicsComponent) {
        m_physicsComponent->UnpausableTick(deltaTimeSec);
    }

    if (m_rootComponent) {
        m_rootComponent->UnpausableTick(deltaTimeSec);
    }

    for (auto& component : m_allComponents) {
        component->UnpausableTick(deltaTimeSec);
    }

    for (const auto& childSp : m_children) {
        childSp->UnpausableTick(deltaTimeSec);
    }

    if (m_inputComponent) {
        m_inputComponent->UnpausableTick(deltaTimeSec);
    }

    if (m_movementComponent) {
        m_movementComponent->UnpausableTick(deltaTimeSec);
    }

    for (const auto& tweener : mTweeners) {
        tweener->UnpausableTick(deltaTimeSec);
    }
}

void Actor::Tick(const float deltaTimeSec)
{
    UpdateTransform();

    // Update physics
    if (m_physicsComponent && m_physicsComponent->IsEnabled()) {
        m_physicsComponent->Tick(deltaTimeSec);
    }

    if (m_rootComponent && m_rootComponent->IsEnabled()) {
        m_rootComponent->Tick(deltaTimeSec);
    }

    for (auto& component : m_allComponents) {
        if (component->IsEnabled()) {
            // tick all children components
            component->Tick(deltaTimeSec);
        }
    }

    for (const auto& childSp : m_children) {
        // tick all attached actors
        childSp->Tick(deltaTimeSec);
    }

    if (m_inputComponent && m_inputComponent->IsEnabled()) {
        m_inputComponent->Tick(deltaTimeSec);
    }

    if (m_movementComponent && m_movementComponent->IsEnabled()) {
        m_movementComponent->Tick(deltaTimeSec);
    }

    for (const auto& tweener : mTweeners) {
        tweener->Tick(deltaTimeSec);
        tweener->NotifyStateChangedObservers();
    }
}

void Actor::AddComponent(std::shared_ptr<EngineCore::Component> component)
{
    component->SetOwner(shared_from_this());
    component->OnPostOwnerInitialized();

    const uint64_t componentType = component->GetComponentType();

    if ((componentType & eComponentType::MOVEMENT_COMPONENT) == eComponentType::MOVEMENT_COMPONENT) {
        m_movementComponent = std::static_pointer_cast<MovementComponent>(component);
    } else if ((componentType & eComponentType::INPUT_COMPONENT) == eComponentType::INPUT_COMPONENT) {
        m_inputComponent = std::static_pointer_cast<InputComponent>(component);
    } else if ((componentType & eComponentType::PHYSICS_COMPONENT) == eComponentType::PHYSICS_COMPONENT) {
        m_physicsComponent = std::static_pointer_cast<PhysicsComponent>(component);
    } else {
        m_allComponents.push_back(component);
    }
}

void Actor::RemoveComponent(std::shared_ptr<EngineCore::Component> component)
{
    auto componentIt = std::find(m_allComponents.begin(), m_allComponents.end(), component);
    if (componentIt != m_allComponents.end()) {
        component->RemoveOwner();
        component->CleanUp();
        m_allComponents.erase(componentIt);
    }
}

void Actor::RemoveMovementComponent()
{
    if (m_movementComponent) {
        m_movementComponent->CleanUp();
    }
    m_movementComponent.reset();
}

void Actor::RemoveInputComponent()
{
    if (m_inputComponent) {
        m_inputComponent->CleanUp();
    }
    m_inputComponent.reset();
}

void Actor::RemovePhysicsComponent()
{
    if (m_physicsComponent) {
        m_physicsComponent->CleanUp();
    }
    m_physicsComponent.reset();
}

void Actor::SetParent(const std::weak_ptr<Actor>& actor)
{
    m_parent = actor;
}

void Actor::SetScene(std::weak_ptr<Scene> sceneOwner)
{
    if (!mSceneOwner.lock()) {
        mSceneOwner = sceneOwner;
        OnSceneOwnerInitialized();
    }
}

void Actor::OnSceneOwnerInitialized()
{
    m_rootComponent->SetOwner(shared_from_this());

    for (const auto& tweener : mTweeners) {
        tweener->InitRootState();
    }
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

    while (const auto& spParent = parentWp.lock()) {
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
    actor->SetParent(shared_from_this());
    m_children.push_back(actor);
}

std::shared_ptr<Actor> Actor::GetChildByObjectId(const int32_t id) const
{
    std::shared_ptr<Actor> resultChild;

    for (const auto& child : m_children) {
        if (child->GetObjectId() == id) {
            resultChild = child;
            break;
        } else {
            const auto& hierarchyChild = child->GetChildByObjectId(id);
            if (hierarchyChild) {
                resultChild = hierarchyChild;
                break;
            }
        }
    }

    return resultChild;
}

void Actor::RemoveChild(const std::shared_ptr<Actor>& actor)
{
    const auto actorIt = std::find_if(
        m_children.begin(), m_children.end(), [&](const auto& childSp) { return actor->GetName() == childSp->GetName(); });

    if (actorIt != m_children.end()) {
        actor->SetParent(std::weak_ptr<Actor>());
        m_children.erase(actorIt);
    }
}

void Actor::AttachTweener(std::shared_ptr<Tweener> newTweener)
{
    auto it = std::find_if(mTweeners.begin(), mTweeners.end(), [=](const auto& tweener) {
        return tweener->GetTweenerName() == newTweener->GetTweenerName();
    });
    ext_assert(it == mTweeners.end(), "Tweener with the same name already attached to Actor");
    newTweener->SetParentActor(shared_from_this());
    mTweeners.emplace_back(newTweener);
}

const std::vector<std::shared_ptr<Tweener>>& Actor::GetTweeners() const
{
    return mTweeners;
}

std::shared_ptr<Tweener> Actor::GetTweenerByName(const std::string& name) const
{
    auto it = std::find_if(
        mTweeners.cbegin(), mTweeners.cend(), [&](const auto& tweener) { return name == tweener->GetTweenerName(); });

    return it != mTweeners.cend() ? *it : nullptr;
}

std::shared_ptr<Tweener> Actor::GetTweenerById(const int32_t id) const
{
    auto it = std::find_if(mTweeners.cbegin(), mTweeners.cend(), [id](const auto& tweener) { return tweener->GetId() == id; });

    return it != mTweeners.cend() ? *it : nullptr;
}

bool Actor::IsVisible() const
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

    if (auto baseParentSp = GetBaseParent().lock()) {
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

} // namespace EngineCore
