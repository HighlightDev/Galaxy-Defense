#include "BarrierActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentCreators/ElectricBeamComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/ElectricBeamComponentData.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/ElectricBeamComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionBoxShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionCompoundShape.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"

#include <BulletPhys/btBulletCollisionCommon.h>
#include <glm/gtx/quaternion.hpp>

using namespace EngineMath;
using namespace EnginePhysics;

namespace Game {
BarrierActor::BarrierActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent)
    : Actor(gameObjectName, rootComponent)
{
}

void BarrierActor::Tick(const float deltaTimeSec)
{
    Actor::Tick(deltaTimeSec);
}

void BarrierActor::SetBarrierMaterials(
    const std::shared_ptr<::Graphics::IMaterial>& pillarMaterial, const std::shared_ptr<::Graphics::IMaterial>& rayMaterial)
{
    mPillarMaterial = pillarMaterial;
    mRayMaterial = rayMaterial;
}

void BarrierActor::CreateNewBarrierPillar(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
    const auto& sceneSp = GetSceneOwner().lock();
    ext_assert(sceneSp, "Scene pointer is null in BarrierActor::CreateNewBarrierPillar");
    ext_assert(mPillarMaterial, "Pillar material is null in BarrierActor::CreateNewBarrierPillar");

    const auto pillarIndex = mBarrierPillars.size();
    const auto& barrierName = GetEngineObjectName();

    const auto d_mesh = std::make_shared<MeshComponentData>(
        "c_barrier_mesh_" + barrierName + "_pillar_" + std::to_string(pillarIndex),
        "ufo.obj",
        position,
        rotation,
        scale,
        mPillarMaterial);
    const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
    const auto& c_mesh
        = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
    AddBarrierPillarMesh(c_mesh);

    if (pillarIndex > 0 && mRayMaterial) {
        const auto rayIndex = mBarrierRays.size();
        const auto d_ray = std::make_shared<ElectricBeamComponentData>(
            "c_barrier_mesh_" + barrierName + "_ray_" + std::to_string(rayIndex),
            glm::vec3(0),
            glm::vec3(0),
            1.0f,
            3,
            1.0f,
            0.05f,
            mRayMaterial);
        const auto& rayComponentCreator = std::make_shared<ElectricBeamComponentCreator<ElectricBeamComponent>>();
        const auto& c_ray
            = std::static_pointer_cast<ElectricBeamComponent>(sceneSp->CreateComponent_GameThread(rayComponentCreator, d_ray));
        c_ray->SetSortOrderValue(200 + static_cast<int32_t>(rayIndex));
        AddRayLineMesh(c_ray);
    }

    TrySetBarrierPillarMeshRelativeTransform(static_cast<int32_t>(pillarIndex), position, rotation, scale);

    // Add collision box child to compound shape; create shape & physics component if first pillar
    const glm::vec3 pillarHalfExtent = scale * 0.5f;
    if (!mCompoundShape) {
        mCompoundShape = std::make_shared<CollisionCompoundShape>();
    }
    const auto& childBoxShape = std::make_shared<CollisionBoxShape>(pillarHalfExtent);
    mCompoundShape->AddChildShape(
        NoScaleEulerRotationTransform(glm::vec3(position.x, position.y, -position.z), glm::vec3(0)), childBoxShape);

    if (!GetPhysicsComponent()) {
        const auto& ghostController = std::make_shared<GhostController>(sceneSp->GetPhysicsWorld(), mCompoundShape, 0.0f);
        const auto physData = std::make_shared<PhysicsComponentData>("c_barrierPhysics_" + barrierName, ghostController);
        const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
        const auto& c_ghostPhysics = sceneSp->CreateComponent_GameThread(physicsComponentCreator, physData);
        AddComponent(c_ghostPhysics);
    }
}

void BarrierActor::RemoveAllBarrierPillars()
{
    const auto& sceneSp = GetSceneOwner().lock();

    for (const auto& pillar : mBarrierPillars) {
        RemoveComponent(pillar);
        if (sceneSp) {
            sceneSp->RemoveComponent(pillar);
        }
    }
    for (const auto& ray : mBarrierRays) {
        RemoveComponent(ray);
        if (sceneSp) {
            sceneSp->RemoveComponent(ray);
        }
    }
    mBarrierPillars.clear();
    mBarrierRays.clear();

    if (mCompoundShape) {
        mCompoundShape->RemoveAllChildShapes();
    }
}

void BarrierActor::AddBarrierPillarMesh(const std::shared_ptr<StaticMeshComponent>& meshComponent)
{
    AddComponent(mBarrierPillars.emplace_back(meshComponent));
}

void BarrierActor::AddRayLineMesh(const std::shared_ptr<ElectricBeamComponent>& rayComponent)
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

        if (mCompoundShape) {
            const auto& childShapes = mCompoundShape->GetChildShapes();
            if (pillarIndex < static_cast<int32_t>(childShapes.size())) {
                btTransform childTransform;
                childTransform.setIdentity();
                childTransform.setOrigin(Converter::glmToBullet(glm::vec3(translation.x, translation.y, -translation.z)));
                childTransform.setRotation(Converter::glmToBullet(
                    glm::quat(glm::vec3(DEG_TO_RAD(rotation.x), DEG_TO_RAD(rotation.y), DEG_TO_RAD(rotation.z)))));
                auto* compoundBtShape = static_cast<btCompoundShape*>(mCompoundShape->GetCollisionShape());
                compoundBtShape->updateChildTransform(pillarIndex, childTransform, true);
            }
        }

        int32_t rayIndex = 0;
        for (const auto& raySp : mBarrierRays) {
            raySp->SetStartWorldPosition(GetBarrierPillarPosition(rayIndex++));
            raySp->SetEndWorldPosition(GetBarrierPillarPosition(rayIndex));
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

void BarrierActor::SetState(const eBarrierActivityState barrierState)
{
    mBarrierState = barrierState;
    if (mBarrierState == eBarrierActivityState::IDLE) {
        for (const auto& pillar : mBarrierPillars) {
            pillar->SetIsEnabled(false);
        }
        for (const auto& ray : mBarrierRays) {
            ray->SetIsEnabled(false);
        }
    }
    SetIsEnabled(barrierState == eBarrierActivityState::ACTIVE);
}

eBarrierActivityState BarrierActor::GetState() const
{
    return mBarrierState;
}

void BarrierActor::SetCompoundShape(const std::shared_ptr<CollisionCompoundShape>& compoundShape)
{
    mCompoundShape = compoundShape;
}
} // namespace Game
