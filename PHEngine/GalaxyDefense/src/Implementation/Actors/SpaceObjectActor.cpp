#include "SpaceObjectActor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"

namespace Game
{
    SpaceObjectActor::SpaceObjectActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : Actor(gameObjectName, rootComponent),
          mModifiersHandler(std::make_unique<ModifiersHandler>())
    {
    }

    bool SpaceObjectActor::IsInsideLevel(const BoundingBox3D &boundingBox) const
    {
        return EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), GetRootComponent()->GetTranslation());
    }

    void SpaceObjectActor::Tick(const float deltaTime)
    {
        Actor::Tick(deltaTime);

        mModifiersHandler->Tick(deltaTime);
    }

    void SpaceObjectActor::TriggerSpawn(const glm::vec3 &position)
    {
        mActivityState = eSpaceObjectActivityState::ACTIVE;
        SetIsEnabled(true);
        GetMovementComponent()->Teleport(position);
    }

    void SpaceObjectActor::TriggerDisabled()
    {
        mActivityState = eSpaceObjectActivityState::IDLE;
        SetIsEnabled(false);
        mModifiersHandler->RemoveAllModifiers();
    }

    eSpaceObjectActivityState SpaceObjectActor::GetActivityState() const
    {
        return mActivityState;
    }

    glm::vec3 SpaceObjectActor::GetWorldPosition() const
    {
        return m_rootComponent->GetTranslation();
    }

    void SpaceObjectActor::AddModifier(const std::shared_ptr<IModifiable> &modifier)
    {
        mModifiersHandler->AddModifier(modifier);
    }

    bool SpaceObjectActor::HasModifier(const eModifierType modifierType, const int32_t creatorObjectId) const
    {
        return mModifiersHandler->HasModifier(modifierType, creatorObjectId);
    }

    bool SpaceObjectActor::HasModifier(const eModifierType modifierType) const
    {
        return mModifiersHandler->HasModifier(modifierType);
    }

    std::shared_ptr<IModifiable> SpaceObjectActor::GetModifier(const eModifierType modifierType) const
    {
        return mModifiersHandler->GetModifier(modifierType);
    }

    void SpaceObjectActor::RemoveModifier(const eModifierType modifierType, const int32_t creatorObjectId)
    {
        mModifiersHandler->RemoveModifier(modifierType, creatorObjectId);
    }
}