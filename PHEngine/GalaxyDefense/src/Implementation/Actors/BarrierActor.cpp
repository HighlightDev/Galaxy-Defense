#include "BarrierActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentCreators/ElectricBeamComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/UiComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/ElectricBeamComponentData.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/ComponentData/UiComponentData.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/ElectricBeamComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionBoxShape.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Implementation/Components/UiComponents/BarrierUiComponent.h"
#include "Implementation/DataProviders/GameConstants.h"

#include <glm/gtx/quaternion.hpp>

using namespace EngineMath;
using namespace EnginePhysics;

namespace Game {
BarrierActor::BarrierActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent)
    : Actor(gameObjectName, rootComponent)
    , mDamageMessageTimers()
{
}

void BarrierActor::setBarrierProtoData(const BarrierUiProtoData& protoData)
{
    mUiProtoData = protoData;
}

void BarrierActor::Tick(const float deltaTimeSec)
{
    Actor::Tick(deltaTimeSec);

    // Actor can have only one physics component, so we can directly update it here without iterating through the list of pillars
    const auto& rootTranslation = GetRootComponent()->GetTranslation();
    for (size_t i = 0; i < mPillarPhysicsComponents.size() && i < mBarrierPillars.size(); ++i) {
        if (mPillarAlive[i]) {
            const auto& pillarWorldPos = mBarrierPillars[i]->GetHierarchyAccumulatedTranslation() + rootTranslation;
            mPillarPhysicsComponents[i]->GetDescriptor()->SetMotionStateWorldTransform(
                Converter::glmToBullet(glm::quat(1, 0, 0, 0)), Converter::glmToBullet(pillarWorldPos));
            bool dirty;
            mPillarPhysicsComponents[i]->GetDescriptor()->UpdateMotionWorldTransformLocalState(dirty, deltaTimeSec);
            mPillarPhysicsComponents[i]->GetDescriptor()->ForceUpdateBroadphaseAabb();
            mUiComponents[i]->SetWorldPosition(pillarWorldPos);
        }
    }
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

    // Create individual ghost physics for this pillar
    const glm::vec3 pillarHalfExtent = scale * 0.5f;
    const auto& boxShape = std::make_shared<CollisionBoxShape>(pillarHalfExtent);
    const auto& ghostController = std::make_shared<GhostController>(sceneSp->GetPhysicsWorld(), boxShape, 0.0f);
    const auto physData = std::make_shared<PhysicsComponentData>(
        "c_barrierPhysics_" + barrierName + "_pillar_" + std::to_string(pillarIndex), ghostController);
    const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
    const auto& c_ghostPhysics
        = std::static_pointer_cast<PhysicsComponent>(sceneSp->CreateComponent_GameThread(physicsComponentCreator, physData));
    boxShape->SetParentPhysicsComponent(c_ghostPhysics);
    c_ghostPhysics->SetOwner(shared_from_this());
    c_ghostPhysics->OnPostOwnerInitialized();
    mPillarPhysicsComponents.emplace_back(c_ghostPhysics);
    mPillarAlive.emplace_back(true);
    mPillarLevels.emplace_back(mNominalPillarHealth);

    // Create per-pillar UI component
    const auto& hudCanvas = sceneSp->GetUiHandler()->GetHudCanvas();
    if (hudCanvas) {
        const auto& uiComponentCreator = std::make_shared<UiComponentCreator<BarrierUiComponent>>();
        const auto& c_uiComponent = std::static_pointer_cast<BarrierUiComponent>(sceneSp->CreateComponent_GameThread(
            uiComponentCreator,
            std::make_shared<UiComponentData>(
                "c_uiComponent_" + barrierName + "_pillar_" + std::to_string(pillarIndex), hudCanvas)));
        AddComponent(c_uiComponent);
        c_uiComponent->CreateUiElements(
            mUiProtoData.font,
            mUiProtoData.fontSize,
            mUiProtoData.text,
            mUiProtoData.color,
            mUiProtoData.lineMaxWidthHeight,
            mUiProtoData.textHorizontalAlignment,
            mUiProtoData.textVerticalAlignment);
        c_uiComponent->SetHealthBarFilledColor(0x3388FF);
        mUiComponents.emplace_back(c_uiComponent);
    }

    mDamageMessageTimers.emplace(std::make_pair(static_cast<int32_t>(pillarIndex), std::make_shared<GameThreadTimer>()));
    auto& insertedTimer = mDamageMessageTimers.at(static_cast<int32_t>(pillarIndex));
    insertedTimer->Initialize();
    insertedTimer->SetIntervalMs(Game::Constants::c_dmgTextShowDuration);
    insertedTimer->SetIsRepeat(false);
    insertedTimer->SetIsPausable(true);
    std::weak_ptr<BarrierUiComponent> weakUiComp = mUiComponents.back();
    insertedTimer->SetCallback([weakUiComp]() {
        if (auto sp = weakUiComp.lock()) {
            sp->FadeOut();
        }
    });
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
    mPillarAlive.clear();
    mPillarLevels.clear();
    mUiComponents.clear();
    mDamageMessageTimers.clear();

    for (const auto& physComp : mPillarPhysicsComponents) {
        physComp->CleanUp();
    }
    mPillarPhysicsComponents.clear();
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
        for (const auto& physComp : mPillarPhysicsComponents) {
            physComp->SetIsEnabled(false);
        }
        for (const auto& uiComp : mUiComponents) {
            uiComp->SetIsEnabled(false);
            uiComp->SetHealthBarVisibility(false);
            uiComp->SetLabelVisibility(false);
        }

        for (const auto& [timerId, timer] : mDamageMessageTimers) {
            timer->StopTimer();
        }
    } else {
        for (size_t i = 0; i < mPillarAlive.size(); ++i) {
            mPillarAlive[i] = true;
        }
        for (auto& level : mPillarLevels) {
            level.RestorePillarHealth();
        }
        for (const auto& pillar : mBarrierPillars) {
            pillar->SetIsEnabled(true);
        }
        for (const auto& ray : mBarrierRays) {
            ray->SetIsEnabled(true);
        }
        for (const auto& physComp : mPillarPhysicsComponents) {
            physComp->SetIsEnabled(true);
        }
        for (const auto& uiComp : mUiComponents) {
            uiComp->SetIsEnabled(true);
            uiComp->SetHealthBarVisibility(true);
            uiComp->SetLabelVisibility(false);
        }
        UpdateHealthBars();
    }
    SetIsEnabled(barrierState == eBarrierActivityState::ACTIVE);
}

eBarrierActivityState BarrierActor::GetState() const
{
    return mBarrierState;
}

std::vector<std::shared_ptr<PhysicsComponent>> BarrierActor::GetPillarPhysicsComponents() const
{
    return mPillarPhysicsComponents;
}

void BarrierActor::SetNominalPillarHealth(const uint32_t pillarHealth)
{
    mNominalPillarHealth = pillarHealth;
}

int32_t BarrierActor::FindPillarIndexByPhysDescriptorId(const int32_t physDescriptorId) const
{
    for (size_t i = 0; i < mPillarPhysicsComponents.size(); ++i) {
        if (mPillarPhysicsComponents[i]->GetDescriptor()->GetId() == physDescriptorId) {
            return static_cast<int32_t>(i);
        }
    }
    return -1;
}

void BarrierActor::TriggerPillarDamage(const int32_t pillarIndex, const uint32_t damage)
{
    ext_assert(pillarIndex >= 0, "BarrierActor pillar index cannot be negative");
    ext_assert(pillarIndex < mPillarLevels.size(), "BarrierActor pillar index out of bounds");

    if (!mPillarAlive[pillarIndex]) {
        return;
    }

    mPillarLevels[pillarIndex].DecreasePillarHealth(damage);
    mUiComponents[pillarIndex]->SetLabelText(std::to_string(damage));
    mUiComponents[pillarIndex]->FadeIn();
    const auto fillPercent = static_cast<float>(mPillarLevels[pillarIndex].GetPillarHealth())
        / static_cast<float>(mPillarLevels[pillarIndex].GetNominalPillarHealth());
    mUiComponents[pillarIndex]->SetHealthBarFillPercent(fillPercent);

    mDamageMessageTimers[static_cast<int32_t>(pillarIndex)]->RestartTimer();

    if (mPillarLevels[pillarIndex].GetPillarHealth() == 0) {
        DestroyPillar(pillarIndex);
    }

    if (AreAllPillarsDestroyed()) {
        SetState(eBarrierActivityState::IDLE);
    }
}

void BarrierActor::DestroyPillar(const int32_t pillarIndex)
{
    LogInfo("BarrierActor::DestroyPillar: actorId: ", GetObjectId(), ", pillarIndex: ", pillarIndex);
    mPillarAlive[pillarIndex] = false;
    mBarrierPillars[pillarIndex]->SetIsEnabled(false);
    mPillarPhysicsComponents[pillarIndex]->SetIsEnabled(false);
    mUiComponents[pillarIndex]->SetIsEnabled(false);
    mUiComponents[pillarIndex]->SetHealthBarVisibility(false);
    mUiComponents[pillarIndex]->SetLabelVisibility(false);
    mDamageMessageTimers[static_cast<int32_t>(pillarIndex)]->StopTimer();
    UpdateRaysConnectivity();
}

void BarrierActor::UpdateRaysConnectivity()
{
    // Rays connect consecutive pillars. Ray[i] connects pillar[i] to pillar[i+1].
    // A ray is only visible if both its endpoint pillars are alive.
    for (size_t rayIdx = 0; rayIdx < mBarrierRays.size(); ++rayIdx) {
        const bool startAlive = mPillarAlive[rayIdx];
        const bool endAlive = mPillarAlive[rayIdx + 1];
        mBarrierRays[rayIdx]->SetIsEnabled(startAlive && endAlive);
    }
}

void BarrierActor::UpdateHealthBars()
{
    for (size_t i = 0; i < mUiComponents.size() && i < mPillarLevels.size(); ++i) {
        const float fillPercent = static_cast<float>(mPillarLevels[i].GetPillarHealth())
            / static_cast<float>(mPillarLevels[i].GetNominalPillarHealth());
        mUiComponents[i]->SetHealthBarFillPercent(fillPercent);
    }
}

bool BarrierActor::AreAllPillarsDestroyed() const
{
    return std::all_of(mPillarAlive.cbegin(), mPillarAlive.cend(), [](bool alive) { return !alive; });
}
} // namespace Game
