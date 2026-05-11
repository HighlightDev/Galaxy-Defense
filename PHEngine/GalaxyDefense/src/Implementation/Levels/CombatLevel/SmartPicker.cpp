#include "SmartPicker.h"

#include "CombatActorsPoolHandler.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/Physics/CollisionTestImplementation/RayCastWithFilterAdapter.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/ScreenRayCaster.h"
#include "Implementation/Actors/SpaceshipActor.h"

using namespace EnginePhysics;
using namespace EngineCore::DataProviders;
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
    const auto& displayDataProvider = GeneralSystemSettingsDataProvider::GetInstance();
    const ScreenRayCaster screenRayCaster;
    return screenRayCaster.CastRayFromScreenSpaceToWorldSpace(
        screenSpacePosition,
        glm::ivec2(displayDataProvider->GetWindowWidth() - 1, displayDataProvider->GetWindowHeight() - 1),
        camera->GetViewProjectionInfo()->CreateProjectionMatrix(),
        camera->GetViewMatrix());
}

int32_t SmartPicker::CastScreenSpaceRayIntoScene(
    const std::shared_ptr<Scene>& sceneSp,
    const std::shared_ptr<ACamera>& camera,
    const glm::ivec2& screenSpacePosition,
    const std::unordered_set<eGameObjectsType>& gameObjectTypesToIgnore) const
{
    int32_t resultId = -1;
    const auto& worldSpaceRay = CreateWorldSpaceRayFromScreenSpacePosition(camera, screenSpacePosition);

    std::vector<std::shared_ptr<PhysicsComponent>> excludedComponents;
    for (auto gameObjectTypeToIgnore : gameObjectTypesToIgnore) {
        const auto& actorsToIgnore = mCombatActorsPoolHandler->GetActorsByGameObjectType(gameObjectTypeToIgnore);
        excludedComponents.reserve(excludedComponents.size() + actorsToIgnore.size());
        std::transform(
            actorsToIgnore.cbegin(),
            actorsToIgnore.cend(),
            std::back_inserter(excludedComponents),
            [](const auto& actorToIgnore) { return actorToIgnore->GetPhysicsComponent(); });
    }

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