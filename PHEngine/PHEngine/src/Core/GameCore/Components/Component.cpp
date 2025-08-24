#include "Component.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Scene.h"

#include <algorithm>

namespace EngineCore {
Component::Component(const std::string& gameObjectName)
    : EngineObject(gameObjectName)
    , m_owner()
    , mIsEnabled(std::make_shared<EngineObjectProperty<bool>>(
          true, "p_isEnabled", [this](const bool& isEnabled) { SetIsEnabled(isEnabled); }))
    , m_sceneWP()
{
    AddEngineProperty(mIsEnabled);
}

Component::~Component()
{
}

void Component::Initialize()
{
}

void Component::CleanUp()
{
}

void Component::SetOwner(const std::weak_ptr<Actor>& ownerActor)
{
    if (!m_owner.lock()) {
        m_owner = ownerActor;
        OnSceneOwnerInitialized();
    }
}

void Component::OnSceneOwnerInitialized()
{
}

void Component::RemoveOwner()
{
    m_owner.reset();
}

eComponentType Component::GetComponentType() const
{
    return COMPONENT;
}

void Component::Tick(const float deltaTime)
{
}

std::weak_ptr<Actor> Component::GetBaseOwner() const
{
    std::weak_ptr<Actor> base = m_owner;

    bool bHasParent = true;

    while (bHasParent) {
        if (auto spBase = base.lock()) {
            auto parent = spBase->GetParent();
            if (const auto& spParent = parent.lock()) {
                base = spParent->GetParent();
            } else {
                bHasParent = false;
            }
        }
    }

    return base;
}

void Component::SetScene(const std::weak_ptr<Scene>& scene)
{
    m_sceneWP = scene;
}

void Component::OnPostInitialized()
{
}

void Component::OnPostOwnerInitialized()
{
}

void Component::PostLevelInit()
{
}

std::weak_ptr<Actor> Component::GetOwner() const
{
    return m_owner;
}

bool Component::IsEnabled() const
{
    return mIsEnabled->GetValue();
}

void Component::SetIsEnabled(const bool bEnabled)
{
    if (mIsEnabled->GetValue() != bEnabled) {
        mIsEnabled->SetValue(bEnabled, false);
    }
}

} // namespace EngineCore