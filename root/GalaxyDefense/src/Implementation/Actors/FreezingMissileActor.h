#pragma once

#include "Core/GameCore/BoundingBox3D.h"
#include "Implementation/Actors/MissileActor.h"

#include <functional>
#include <memory>
#include <unordered_map>

using namespace EngineCore;

namespace EngineCore {
class Actor;
}

namespace Game {
class MissileExplosionVisitorBase;
class CombatActorsPoolHandler;
class SpaceStationActor;

class FreezingMissileActor : public MissileActor {

    std::weak_ptr<SpaceStationActor> mSpaceshipWhoSpawnedMeWp;

    float explosionTime{0.0f};

public:
    FreezingMissileActor(
        const std::string& gameObjectName,
        const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
        const std::shared_ptr<CombatActorsPoolHandler>& poolHandler);

    void TriggerSpawn(
        const glm::vec3& position,
        const glm::vec3& direction,
        const float yawDegrees,
        const eDamageDealerType ownerType,
        const std::shared_ptr<SpaceStationActor>& spawnerActor) override;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void TriggerExplosion() override;

    void TriggerExplosionFinished() override;

    void TriggerDisabled() override;

    std::shared_ptr<MissileExplosionVisitorBase> CreateMissileExplosionVisitor() override;
};
} // namespace Game