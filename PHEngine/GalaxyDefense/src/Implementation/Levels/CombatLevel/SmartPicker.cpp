#include "SmartPicker.h"

#include "CombatActorsPoolHandler.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Physics/CollisionTestImplementation/RayCastWithFilterAdapter.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Scene.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/UtilityCore/ScreenRayCaster.h"
#include "Implementation/Actors/SpaceshipActor.h"

using namespace EnginePhysics;
using namespace EngineCore;
using namespace IO;

namespace Game {
SmartPicker::SmartPicker(const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler)
    : mCombatActorsPoolHandler(combatActorsPoolHandler)
{
}

glm::vec3 SmartPicker::CreateWorldSpaceRayFromScreenSpacePosition(
    const std::shared_ptr<ACamera>& camera, const glm::ivec2& screenSpacePosition) const
{
    const auto& displayDataProvider = DisplayDeviceDataProvider::GetInstance();
    const ScreenRayCaster screenRayCaster;
    return screenRayCaster.CastRayFromScreenSpaceToWorldSpace(
        screenSpacePosition,
        glm::ivec2(displayDataProvider->GetWindowWidth() - 1, displayDataProvider->GetWindowHeight() - 1),
        camera->GetViewProjectionInfo()->CreateProjectionMatrix(),
        camera->GetViewMatrix());
}

int32_t SmartPicker::CastScreenSpaceRayIntoScene(
    const std::shared_ptr<Scene>& sceneSp, const std::shared_ptr<ACamera>& camera, const glm::ivec2& screenSpacePosition)
{
    int32_t resultId = -1;
    const auto& worldSpaceRay = CreateWorldSpaceRayFromScreenSpacePosition(camera, screenSpacePosition);

    const auto& enemySpaceships = mCombatActorsPoolHandler->GetEnemySpaceshipActors();
    std::vector<std::shared_ptr<PhysicsComponent>> excludedComponents;
    excludedComponents.reserve(enemySpaceships.size());
    std::transform(
        enemySpaceships.cbegin(), enemySpaceships.cend(), std::back_inserter(excludedComponents), [](const auto& enemySpaceship) {
            return enemySpaceship->GetPhysicsComponent();
        });
    auto rayCast = RayCastWithFilterAdapter(excludedComponents);
    const auto &rayCastStartPos = camera->GetEyeVector(),
               rayCastEndPos = glm::vec3(camera->GetEyeVector() + worldSpaceRay * 1000.0f);
    rayCast.RayTest(sceneSp->GetPhysicsWorld(), rayCastStartPos, rayCastEndPos);
    if (rayCast.IsRayHitCollision()) {
        if (const auto& collidedPhysDescriptor = rayCast.GetCollisionHitPhysicsDescriptor()) {
            resultId = collidedPhysDescriptor->GetOwnerActorEngineObjectId();
        }
    }

    return resultId;
}
} // namespace Game