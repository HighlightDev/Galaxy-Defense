#include "BackgroundSpaceObjectActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/Random.h"
#include "Core/UtilityCore/EngineMath.h"

namespace Game {
BackgroundSpaceObjectActor::BackgroundSpaceObjectActor(
    const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent)
    : SpaceObjectActor(gameObjectName, rootComponent)
    , mBackgroundSpaceObjectBillboard()
{
}

void BackgroundSpaceObjectActor::PostLevelInit()
{
    SpaceObjectActor::PostLevelInit();

    mBackgroundSpaceObjectBillboard = GetComponentsByType<BillboardComponent>().back();
}

void BackgroundSpaceObjectActor::TriggerSpawn(const glm::vec3& position)
{
    mActivityState = eSpaceObjectActivityState::ACTIVE;
    SetIsEnabled(true);
    GetMovementComponent()->Teleport(position);
}

void BackgroundSpaceObjectActor::TriggerDisabled()
{
    mActivityState = eSpaceObjectActivityState::IDLE;
    SetIsEnabled(false);
}

void BackgroundSpaceObjectActor::SetBillboardExtentSize(const float size)
{
    mBackgroundSpaceObjectBillboard->SetBillboardExtent(size);
}
} // namespace Game