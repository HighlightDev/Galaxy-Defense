#include "LootActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemBaseComponent.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Components/MovementComponents/LootDropMovementComponent.h"
#include "Implementation/Controllers/NavigationController.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

#include <glm/ext/matrix_projection.hpp>

namespace Game {
LootActor::LootActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent)
    : Actor(gameObjectName, rootComponent)
    , mLootCategory(eLootCategory::LOOT)
    , mLootState(eLootState::NA)
    , bIsCollecting(false)
{
}

eLootCategory LootActor::GetLootCategory() const
{
    return mLootCategory;
}

void LootActor::SetupLootTweener(
    std::shared_ptr<Tweener> tweener,
    std::shared_ptr<StateProperty<eEnginePropertyBindingType::Vec3QuadraticBezier>> idleProp,
    std::shared_ptr<StateProperty<eEnginePropertyBindingType::Vec3QuadraticBezier>> collectedProp,
    std::shared_ptr<StateProperty<eEnginePropertyBindingType::EulerAnglesRotation>> collectedRotProp)
{
    mLootTweener = tweener;
    mIdleBezierProp = idleProp;
    mCollectedBezierProp = collectedProp;
    mCollectedRotProp = collectedRotProp;

    mLootTweener->SubscribeOnStateChange(std::dynamic_pointer_cast<LootActor>(shared_from_this()));
    Actor::AttachTweener(mLootTweener);
}

void LootActor::OnTweenStateChanged(const std::string& stateName)
{
    LogInfo("LootActor::OnTweenStateChanged: name: ", GetName(), " stateName: ", stateName);
    if ("s_Collected" == stateName) {
        // Award crystals to player when the collect animation finishes
        if (mLootCategory == eLootCategory::LOOT) {
            PlayerDataProvider::GetInstance()->SetCrystalsCount(PlayerDataProvider::GetInstance()->GetCrystalsCount() + 1);
        }
        SetLootState(eLootState::IDLE);
    }
}

void LootActor::SetLootCategory(const eLootCategory lootCategory)
{
    mLootCategory = lootCategory;
}

void LootActor::SetLootState(const eLootState lootState)
{
    if (mLootState != lootState) {
        mLootState = lootState;
        bIsCollecting.store(false, std::memory_order::seq_cst); // reset collecting state when loot state changes
        SetIsEnabled(mLootState == eLootState::ACTIVE);

        const auto particleComponents = GetComponentsByType<ParticleSystemBaseComponent>();
        for (const auto& particleComponent : particleComponents) {
            if (mLootState != eLootState::ACTIVE) {
                particleComponent->ResetParticles();
                const auto movementComponents = std::dynamic_pointer_cast<LootDropMovementComponent>(GetMovementComponent());
                if (movementComponents) {
                    movementComponents->Reset();
                }
            } else {
                particleComponent->EmitParticles();
            }
        }

        // Re-enable the movement component when transitioning back to ACTIVE so it
        // is not permanently disabled after the collect fly-off animation.
        if (mLootState == eLootState::ACTIVE) {
            const auto movementComponent = std::dynamic_pointer_cast<LootDropMovementComponent>(GetMovementComponent());
            if (movementComponent) {
                movementComponent->SetIsEnabled(true);
            }
        }
    }
}

eLootState LootActor::GetLootState() const
{
    return mLootState;
}

void LootActor::Tick(const float deltaTimeSec)
{
    Actor::Tick(deltaTimeSec);
}

void LootActor::CollectLoot()
{
    ext_assert(mLootTweener, "LootActor::CollectLoot: Loot tweener is not set up for loot: " + GetName());

    if (not bIsCollecting.load(
            std::memory_order::seq_cst)) { // ignore collect attempts while the fly-off animation is already in progress
        bIsCollecting.store(true, std::memory_order::seq_cst);

        LogInfo(
            "LootActor::CollectLoot: name: ",
            GetName(),
            " state: ",
            static_cast<int>(mLootState),
            " category: ",
            static_cast<int>(mLootCategory));

        // Stop the drop/bounce movement so it doesn't fight the tweener's translation control.
        const auto movementComponent = std::dynamic_pointer_cast<LootDropMovementComponent>(GetMovementComponent());
        if (movementComponent) {
            movementComponent->SetIsEnabled(false);
        }

        // Update bezier endpoints: source = current loot position, destination = crystal tile world position.
        if (mCollectedBezierProp && mIdleBezierProp) {
            if (const auto sceneSp = GetSceneOwner().lock()) {
                const auto& mainCamera = sceneSp->GetMainCamera();
                auto tileSp = mCachedCrystalTile.lock();
                if (!tileSp) {
                    tileSp = sceneSp->TryFindUiItemInAllCanvases("CrystalStockTile");
                    mCachedCrystalTile = tileSp;
                }
                if (tileSp && mainCamera) {
                    const glm::vec3 lootPos = GetRootComponent()->GetTranslation();
                    mIdleBezierProp->Value = lootPos;

                    const glm::mat4 viewMatrix = mainCamera->GetViewMatrix();
                    const glm::mat4 projMatrix = mainCamera->GetViewProjectionInfo()->CreateProjectionMatrix();

                    // Compute the loot's window-space Z so the tile is unprojected at the same depth.
                    const glm::vec4 lootClip = projMatrix * viewMatrix * glm::vec4(lootPos, 1.0f);
                    const float windowZ = (lootClip.z / lootClip.w + 1.0f) * 0.5f;

                    const int32_t windowW = static_cast<int32_t>(
                        EngineCore::DataProviders::GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth());
                    const int32_t windowH = static_cast<int32_t>(
                        EngineCore::DataProviders::GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight());

                    // GetAbsoluteOrigin() returns bottom-left in Y-up screen coordinates, matching glm::unProject convention.
                    const glm::ivec2 tileCenter = tileSp->GetAbsoluteOrigin()
                        + glm::ivec2(static_cast<int32_t>(tileSp->GetWidth()) / 2, static_cast<int32_t>(tileSp->GetHeight()) / 2);

                    mCollectedBezierProp->Value = glm::unProject(
                        glm::vec3(tileCenter.x, tileCenter.y, windowZ),
                        viewMatrix,
                        projMatrix,
                        glm::vec4(0, 0, windowW, windowH));

                    // Arc control point: midpoint between src and dst, raised upward.
                    static constexpr float c_arcHeight = 5.0f;
                    const glm::vec3 arcControlPoint
                        = (mIdleBezierProp->Value + mCollectedBezierProp->Value) * 0.5f + glm::vec3(0.0f, c_arcHeight, 0.0f);
                    mIdleBezierProp->ControlPoint = arcControlPoint;
                    mCollectedBezierProp->ControlPoint = arcControlPoint;
                }
            }
        }

        // Fly to the crystal tile via the bezier arc; actor is disabled in OnTweenStateChanged("s_Collected").
        // Randomise the destination rotation so the crystal tumbles during the fly-off.
        if (mCollectedRotProp) {
            static constexpr float c_fullSpin = 360.0f;
            mCollectedRotProp->SetValue(
                glm::vec3(Random::Float() * c_fullSpin, Random::Float() * c_fullSpin, Random::Float() * c_fullSpin));
        }
        mLootTweener->ChangeState("s_Collected");
    }
}

void LootActor::SpawnLoot(const glm::vec3& position)
{
    LogInfo("LootActor::SpawnLoot: name: ", GetName(), " position: ", position);
    GetRootComponent()->SetTranslation(position);
    SetLootState(eLootState::ACTIVE);

    if (mLootTweener && mIdleBezierProp && mCollectedBezierProp) {
        // Bake the dynamic spawn position into the tweener state properties so the
        // bezier arc always starts from the actual current spawn point.
        static constexpr float c_collectHeightOffset = 15.0f;
        mIdleBezierProp->Value = position;
        mCollectedBezierProp->Value = glm::vec3(position.x, position.y + c_collectHeightOffset, position.z);
        mLootTweener->InitRootState();
    }
}

} // namespace Game