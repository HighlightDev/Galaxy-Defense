#include "AsteroidActor.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/MovementComponent.h"

namespace Game
{
    AsteroidActor::AsteroidActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : SpaceObjectActor(gameObjectName, rootComponent),
          mModifiersHandler(std::make_unique<ModifiersHandler>())
    {
    }

    void AsteroidActor::Tick(const float deltaTime)
    {
        SpaceObjectActor::Tick(deltaTime);

        mModifiersHandler->Tick(deltaTime);
    }

    void AsteroidActor::TriggerSpawn(const glm::vec3 &position)
    {
        mActivityState = eSpaceObjectActivityState::ACTIVE;
        SetIsEnabled(true);
        GetMovementComponent()->Teleport(position);
    }

    void AsteroidActor::TriggerDisabled()
    {
        mActivityState = eSpaceObjectActivityState::IDLE;
        SetIsEnabled(false);
        mModifiersHandler->RemoveAllModifiers();
    }

    void AsteroidActor::AddModifier(const std::shared_ptr<IModifiable> &modifier)
    {
        mModifiersHandler->AddModifier(modifier);
    }

    bool AsteroidActor::HasModifier(const eModifierType modifierType, const uint64_t creatorObjectId) const
    {
        return mModifiersHandler->HasModifier(modifierType, creatorObjectId);
    }

    bool AsteroidActor::HasModifier(const eModifierType modifierType) const
    {
        return mModifiersHandler->HasModifier(modifierType);
    }

    std::shared_ptr<IModifiable> AsteroidActor::GetModifier(const eModifierType modifierType) const
    {
        return mModifiersHandler->GetModifier(modifierType);
    }

    void AsteroidActor::RemoveModifier(const eModifierType modifierType, const uint64_t creatorObjectId)
    {
        mModifiersHandler->RemoveModifier(modifierType, creatorObjectId);
    }
}