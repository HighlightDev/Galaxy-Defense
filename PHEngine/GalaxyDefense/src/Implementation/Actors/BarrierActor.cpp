#include "BarrierActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtx/quaternion.hpp>

using namespace EngineMath;

namespace Game {
BarrierActor::BarrierActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent)
    : Actor(gameObjectName, rootComponent)
{
}

void BarrierActor::Tick(const float deltaTimeSec)
{
    Actor::Tick(deltaTimeSec);
}

void BarrierActor::AddBarrierPillarMesh(const std::shared_ptr<StaticMeshComponent>& meshComponent)
{
    AddComponent(mBarrierPillars.emplace_back(meshComponent));
}

void BarrierActor::AddRayLineMesh(const std::shared_ptr<RuntimeGeneratedLineComponent>& rayComponent)
{
    AddComponent(mBarrierRays.emplace_back(rayComponent));
}

bool BarrierActor::TrySetBarrierPillarMeshRelativeTransform(
    const int32_t pillarIndex, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
{
    ext_assert(pillarIndex >= 0, "BarrierActor pillar index cannot be negative");
    if (mBarrierPillars.size() > pillarIndex) {
        const auto& pillarMesh = mBarrierPillars[pillarIndex];
        pillarMesh->SetTranslation(translation);
        pillarMesh->SetRotator(glm::quat(glm::vec3(DEG_TO_RAD(rotation.x), DEG_TO_RAD(rotation.y), DEG_TO_RAD(rotation.z))));
        pillarMesh->SetScale(scale);

        int32_t rayIndex = 0;
        for (const auto& raySp : mBarrierRays) {
            raySp->SetLineBeginWorldSpacePosition(GetBarrierPillarPosition(rayIndex++));
            raySp->SetLineEndWorldSpacePosition(GetBarrierPillarPosition(rayIndex));
        }
    }
    return false;
}

glm::vec3 BarrierActor::GetBarrierPillarPosition(const int32_t pillarIndex) const
{
    ext_assert(pillarIndex < mBarrierPillars.size(), "BarrierActor pillar index out of bounds");
    return mBarrierPillars[pillarIndex]->GetHierarchyAccumulatedTranslation() + GetRootComponent()->GetTranslation();
}

int32_t BarrierActor::GetBarrierPillarsCount() const
{
    return mBarrierPillars.size();
}

int32_t BarrierActor::GetBarrierRaysCount() const
{
    return mBarrierRays.size();
}

std::vector<std::shared_ptr<StaticMeshComponent>> BarrierActor::GetBarrierPillarsMeshComponents() const
{
    return mBarrierPillars;
}
} // namespace Game
