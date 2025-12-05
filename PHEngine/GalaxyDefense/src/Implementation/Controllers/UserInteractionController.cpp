#include "UserInteractionController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/Input/KeyboardBindings.h"
#include "Core/GameCore/Input/MouseBindings.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/GraphicsCore/SceneProxy/MainCameraSceneProxy.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/ScreenRayCaster.h"
#include "Implementation/ActorLeveling/LevelAttributeDataProvider.h"
#include "Implementation/Actors/SpaceStationActor.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"
#include "Implementation/Events/MainPlayerStatusChangedEvent.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"
#include "Implementation/Levels/CombatLevel/SmartPicker.h"

#include <json/json.hpp>

#include <algorithm>

using namespace IO;
using namespace Graphics;
using namespace EngineCore::DataProviders;
using namespace Resources;

namespace Game {
UserInteractionController::UserInteractionController(const std::weak_ptr<Scene>& sceneWp)
    : mSceneWp(sceneWp)
    , mLevelBounds()
    , mInputComponent(std::make_unique<InputComponent>(std::make_shared<ComponentData>("GameFlowController_InputComponent")))
    , mMainSceneCamera()
    , mProjectileMarkerActor(std::make_shared<Actor>(
          "MissileProjectileActor",
          std::make_shared<SceneComponent>("MissileProjectileRootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f))))
    , mGhostTowerActor(std::make_shared<Actor>(
          "GhostTowerActor",
          std::make_shared<SceneComponent>("GhostTowerActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f))))
    , mRemoveTowerMarkerActor(std::make_shared<Actor>(
          "RemoveTowerMarkerActor",
          std::make_shared<SceneComponent>("RemoveTowerMarkerActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f))))
    , mReadyToShootTimer(std::make_shared<GameThreadTimer>())
    , mReloadPlacementTower(std::make_shared<GameThreadTimer>())
    , mGhostTowerBlendColorProperty(std::make_shared<EngineObjectProperty<glm::vec3>>(glm::vec3(0.0f), "p_blendColor"))
    , mRemoveTowerMarkerBlendColorProperty(
          std::make_shared<EngineObjectProperty<glm::vec3>>(glm::vec3(0.0f), "p_transparency_color_filler"))
{
    mGhostTowerActor->AddEngineProperty(mGhostTowerBlendColorProperty);
    mRemoveTowerMarkerActor->AddEngineProperty(mRemoveTowerMarkerBlendColorProperty);

    mReadyToShootTimer->Initialize();
    mReadyToShootTimer->SetIsPausable(true);
    mReadyToShootTimer->SetIsRepeat(false);
    mReadyToShootTimer->SetIntervalMs(500);

    mReloadPlacementTower->Initialize();
    mReloadPlacementTower->SetIsPausable(true);
    mReloadPlacementTower->SetIsRepeat(false);
    mReloadPlacementTower->SetIntervalMs(200);
}

void UserInteractionController::SetUserInteractionType(const eUserInteractionType interactionType)
{
    mInteractionType = interactionType;
    switch (interactionType) {

    case eUserInteractionType::TOWER_PLACE_SELECTION:
        mGhostTowerActor->SetIsEnabled(true);
        mTowerPlacementGridActor->SetIsEnabled(true);
        mPlacementAllowedAreaActor->SetIsEnabled(true);
        mRemoveTowerMarkerActor->SetIsEnabled(false);
        break;
    case eUserInteractionType::TOWER_REMOVEMENT_SELECTION:
        mRemoveTowerMarkerActor->SetIsEnabled(true);
        mTowerPlacementGridActor->SetIsEnabled(true);
        mPlacementAllowedAreaActor->SetIsEnabled(true);
        mGhostTowerActor->SetIsEnabled(false);
        break;
    case eUserInteractionType::IDLE:
    default:
        mTowerPlacementGridActor->SetIsEnabled(false);
        mPlacementAllowedAreaActor->SetIsEnabled(false);
        mGhostTowerActor->SetIsEnabled(false);
        mRemoveTowerMarkerActor->SetIsEnabled(false);
        break;
    }
}

UserInteractionController::~UserInteractionController()
{
    ChangeGameModeEvent::GetInstance()->RemoveListener(ChangeGameModeEvent::GetInstanceId());
    BroadcastGameThreadEvent::GetInstance()->RemoveListener(BroadcastGameThreadEvent::GetInstanceId());
}

void UserInteractionController::Initialize()
{
    ChangeGameModeEvent::GetInstance()->AddListener(shared_from_this());
    BroadcastGameThreadEvent::GetInstance()->AddListener(shared_from_this());

    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in UserInteractionController::Initialize");
    const auto& mainCameraSp = std::dynamic_pointer_cast<ThirdPersonCamera>(sceneSp->GetMainCamera());
    ext_assert(mainCameraSp, "Failed to cast main camera to ThirdPersonCamera");
    mMainSceneCamera = mainCameraSp;

    sceneSp->AddActor(mProjectileMarkerActor);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& missileProjectileMaterial
        = materialParser.ParseMaterialDescriptor("EditorPickerMaterial.m");
    sceneSp->RegisterMaterialInstance(missileProjectileMaterial);
    MaterialPropertySetter::SetMaterialPropertyValue(missileProjectileMaterial, "opacity", 1.0f);
    MaterialPropertySetter::SetMaterialPropertyValue(missileProjectileMaterial, "color", glm::vec3(1.0f, 0.0f, 0.0f));
    MaterialPropertySetter::SetMaterialPropertyValue(missileProjectileMaterial, sceneSp, "GT_DeltaSec", "gt_timeSec");

    const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(false);
    const auto& d_mesh = std::make_shared<MeshComponentData>(
        "MissileProjectileMeshComponent",
        "plane.obj",
        glm::vec3(),
        glm::vec3(),
        glm::vec3(mTowerCellSize, 1.0f, mTowerCellSize),
        "",
        missileProjectileMaterial);
    const auto& c_mesh
        = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
    c_mesh->SetSortOrderValue(1);
    mProjectileMarkerActor->AddComponent(c_mesh);
    mProjectileMarkerActor->SetIsEnabled(false);

    InitializeTowerGrid();
    InitializePlacementAllowedArea();
    InitializeGhostTower();
    InitializeRemoveTowerMarker();
    SetUserInteractionType(eUserInteractionType::IDLE);
}

void UserInteractionController::SetTowersData(
    const std::unordered_map<std::string, std::tuple<glm::vec3 /*position*/, glm::vec3 /*scale*/>>& towersData)
{
    ext_assert(towersData.size(), "Towers data is empty in UserInteractionController::SetTowersData");
    mTowerPlacementCells.reserve(towersData.size());
    std::transform(towersData.cbegin(), towersData.cend(), std::back_inserter(mTowerPlacementCells), [](const auto& towerData) {
        return std::get<0>(towerData.second);
    });
    mTowerCellSize = std::get<1>(towersData.begin()->second).x;
}

void UserInteractionController::SetLevelBounds(const BoundingBox3D& levelBounds)
{
    mLevelBounds = levelBounds;
}

void UserInteractionController::OnPreLevelInit()
{
}

void UserInteractionController::OnLevelInit()
{
    ext_assert(mCombatActorsPoolHandler, "Combat actors pool handler is null in OnLevelInit");
    mSmartPicker = std::make_shared<SmartPicker>(mCombatActorsPoolHandler);
}

void UserInteractionController::OnPostLevelInit()
{
    Initialize();
}

void UserInteractionController::PostPlayLevelFinished()
{
}

void UserInteractionController::CleanUp()
{
}

bool UserInteractionController::IsTowerPositionValid(const glm::vec3 position) const
{
    const glm::vec2& cellBoundingBoxOrigin
        = mLevelPlacementGrid->GetNearestToPositionTowerCellBoundingBox(glm::vec2(position.x, position.z)).GetOrigin();
    bool isPlaceAllowedForTower = std::any_of(
        mTowerPlacementCells.cbegin(), mTowerPlacementCells.cend(), [this, &cellBoundingBoxOrigin](const auto& validPosition) {
            const auto& validCellBoundingBox
                = mLevelPlacementGrid->GetNearestToPositionTowerCellBoundingBox(glm::vec2(validPosition.x, validPosition.z));

            return EngineMath::TestPointInAABB(
                validCellBoundingBox.GetMin(), validCellBoundingBox.GetMax(), cellBoundingBoxOrigin);
        });

    if (isPlaceAllowedForTower) {
        isPlaceAllowedForTower = GetSpaceStationAtPosition(position) == nullptr;
    }

    return isPlaceAllowedForTower;
}

std::shared_ptr<SpaceStationActor> UserInteractionController::GetSpaceStationAtPosition(const glm::vec3& position) const
{
    const glm::vec2& cellBoundingBoxOrigin
        = mLevelPlacementGrid->GetNearestToPositionTowerCellBoundingBox(glm::vec2(position.x, position.z)).GetOrigin();
    const auto& spaceStationActors = mCombatActorsPoolHandler->GetSpaceStationActors();

    auto foundSpaceStationIt = std::find_if(
        spaceStationActors.cbegin(), spaceStationActors.cend(), [this, &cellBoundingBoxOrigin](const auto& alreadyPlacedTower) {
            const auto& alreadyPlacedTowerPosition = alreadyPlacedTower->GetRootComponent()->GetTranslation();
            const auto& invalidCellBoundingBox = mLevelPlacementGrid->GetNearestToPositionTowerCellBoundingBox(
                glm::vec2(alreadyPlacedTowerPosition.x, alreadyPlacedTowerPosition.z));

            return EngineMath::TestPointInAABB(
                invalidCellBoundingBox.GetMin(), invalidCellBoundingBox.GetMax(), cellBoundingBoxOrigin);
        });
    return foundSpaceStationIt != spaceStationActors.cend()
            && eSpaceStationActivityState::ACTIVE == (*foundSpaceStationIt)->GetState()
        ? *foundSpaceStationIt
        : nullptr;
}

void UserInteractionController::Tick(const float deltaTimeSec)
{
    if (eGameModeType::SPACE_STATION_PLACEMENT == mCurrentGameModeType) {
        ProcessSpaceStationPlacementStage();
    } else if (eGameModeType::COMBAT == mCurrentGameModeType) {
        ProcessSpaceStationPlacementStage();
        ProcessCombatStage();
    }
}

void UserInteractionController::ProcessSpaceStationPlacementStage()
{
    const auto& sceneSp = mSceneWp.lock();
    const auto& sceneCameraSp = mMainSceneCamera.lock();
    if (!sceneCameraSp || !sceneSp) {
        return;
    }

    const auto& mouseBindings = mInputComponent->GetMouseBindings();

    if (mouseBindings->IsMouseMoveEventDirty()) {
        const auto& mouseMoveEvent = mouseBindings->FlushMouseMoveEvent();
        const glm::ivec2& screenSpacePosition = glm::ivec2(mouseMoveEvent.x, mouseMoveEvent.y);

        if (eUserInteractionType::TOWER_PLACE_SELECTION == mInteractionType
            || eUserInteractionType::TOWER_REMOVEMENT_SELECTION == mInteractionType) {
            const glm::vec4 planeAtOrigin = glm::vec4(0, 1, 0, 0);
            const auto& worldSpaceRay
                = mSmartPicker->CreateWorldSpaceRayFromScreenSpacePosition(sceneCameraSp, screenSpacePosition);
            const float tParam = EngineMath::RaycastPlane(sceneCameraSp->GetEyeVector(), worldSpaceRay, planeAtOrigin);
            if (tParam >= 0.0f) {
                const auto& placementPosition = sceneCameraSp->GetEyeVector() + (worldSpaceRay * tParam);
                const auto& cellOriginPosition
                    = mLevelPlacementGrid
                          ->GetNearestToPositionTowerCellBoundingBox(glm::vec2(placementPosition.x, placementPosition.z))
                          .GetOrigin();
                const auto cellPositionVec3 = glm::vec3(cellOriginPosition.x, 0.0f, cellOriginPosition.y);

                if (eUserInteractionType::TOWER_PLACE_SELECTION == mInteractionType) {
                    mGhostTowerActor->GetRootComponent()->SetTranslation(cellPositionVec3);

                    const glm::vec3 ghostTowerPositionValidationColor
                        = IsTowerPositionValid(cellPositionVec3) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
                    mGhostTowerBlendColorProperty->SetValue(ghostTowerPositionValidationColor);
                } else if (eUserInteractionType::TOWER_REMOVEMENT_SELECTION == mInteractionType) {
                    const glm::vec3 removeTowerPositionValidationColor
                        = GetSpaceStationAtPosition(placementPosition) == nullptr ? glm::vec3(0.3) : glm::vec3(1);
                    mRemoveTowerMarkerBlendColorProperty->SetValue(removeTowerPositionValidationColor);
                    const glm::vec3 movedUpPosition = placementPosition + EngineMath::AXIS_UP * 3.0f;
                    const auto ndcSpacePosition = sceneCameraSp->GetConvertedToNDCSpacePosition(glm::vec4(movedUpPosition, 1.0f));
                    mRemoveTowerMarkerActor->GetRootComponent()->SetTranslation(
                        glm::vec3(ndcSpacePosition.x, ndcSpacePosition.y, 0.0f));
                }
            }
        }
    }

    if (mouseBindings->GetKeyState(eMouseKeys::MouseButtonLeft) == KeyState::PRESSED) {
        if (!mReloadPlacementTower->IsRunning()
            && (eUserInteractionType::TOWER_PLACE_SELECTION == mInteractionType
                || eUserInteractionType::TOWER_REMOVEMENT_SELECTION == mInteractionType)) {
            const glm::vec4 planeAtOrigin = glm::vec4(0, 1, 0, 0);
            const auto& lastMousePosition = mouseBindings->GetLastMouseCursorPosition();
            const glm::ivec2& screenSpacePosition = glm::ivec2(lastMousePosition.x, lastMousePosition.y);
            const auto& worldSpaceRay
                = mSmartPicker->CreateWorldSpaceRayFromScreenSpacePosition(sceneCameraSp, screenSpacePosition);
            const float tParam = EngineMath::RaycastPlane(sceneCameraSp->GetEyeVector(), worldSpaceRay, planeAtOrigin);
            if (tParam >= 0.0f) {
                const auto& placementPosition = sceneCameraSp->GetEyeVector() + (worldSpaceRay * tParam);
                const auto& cellOriginPosition
                    = mLevelPlacementGrid
                          ->GetNearestToPositionTowerCellBoundingBox(glm::vec2(placementPosition.x, placementPosition.z))
                          .GetOrigin();
                const auto cellPositionVec3 = glm::vec3(cellOriginPosition.x, 0.0f, cellOriginPosition.y);

                if (eUserInteractionType::TOWER_PLACE_SELECTION == mInteractionType && IsTowerPositionValid(cellPositionVec3)) {
                    ext_assert(mTowerMissileType != eMissileType::NONE, "Tower missile type is NONE");
                    const auto spaceStationSp = mCombatActorsPoolHandler->GetFreeSpaceStationActor();
                    ext_assert(spaceStationSp, "Failed to get free space station actor");
                    spaceStationSp->GetRootComponent()->SetTranslation(cellPositionVec3);
                    spaceStationSp->SetSpaceStationLevel(std::make_shared<SpaceStationLevel>(
                        mTowerMissileType,
                        1,
                        LevelAttributeDataProvider::GetRadiusForMissileTypeAtLevel(mTowerMissileType, 1),
                        LevelAttributeDataProvider::GetCooldownForMissileTypeAtLevel(mTowerMissileType, 1)));
                    spaceStationSp->SetState(eSpaceStationActivityState::ACTIVE);
                    SetUserInteractionType(eUserInteractionType::IDLE);
                } else if (eUserInteractionType::TOWER_REMOVEMENT_SELECTION == mInteractionType) {
                    const auto& spaceStationSp = GetSpaceStationAtPosition(cellPositionVec3);
                    if (spaceStationSp) {
                        spaceStationSp->SetState(eSpaceStationActivityState::IDLE);
                    }
                }
                nlohmann::json root;
                root["towers_count"] = std::to_string(
                    mCombatActorsPoolHandler->GetSpaceStationsCountWithState(eSpaceStationActivityState::ACTIVE));
                TriggerPlayerStatusChangedEvent(eMainPlayerStatusType::TOWERS_COUNT_CHANGED, root.dump());
                mReloadPlacementTower->StartTimer();
            }
        }
    }
}

void UserInteractionController::ProcessCombatStage()
{
    const auto& sceneSp = mSceneWp.lock();
    const auto& sceneCameraSp = mMainSceneCamera.lock();
    if (!sceneCameraSp || !sceneSp) {
        return;
    }

    const auto& mouseBindings = mInputComponent->GetMouseBindings();

    if (mouseBindings->GetKeyState(eMouseKeys::MouseButtonLeft) == KeyState::PRESSED) {
        const auto& mousePosition = mouseBindings->GetLastMouseCursorPosition();
        const glm::ivec2& screenSpacePosition = glm::ivec2(mousePosition.x, mousePosition.y);
        const int32_t collidedObjectId = mSmartPicker->CastScreenSpaceRayIntoScene(sceneSp, sceneCameraSp, screenSpacePosition);

        if (-1 != collidedObjectId
            && eGameObjectsType::SPACE_STATION == mCombatActorsPoolHandler->GetGameObjectTypeByActorId(collidedObjectId)) {
            mSelectedSpaceStationId = collidedObjectId;
            PlayerDataProvider::GetInstance()->SetSelectedTowerId(mSelectedSpaceStationId);
        } else if (!mProjectileMarkerActor->IsEnabled()) {
            mSelectedSpaceStationId = -1;
            PlayerDataProvider::GetInstance()->SetSelectedTowerId(-1);
        } else if (mProjectileMarkerActor->IsEnabled() && mShootCallback && !mReadyToShootTimer->IsRunning()) {
            mShootCallback();
            mReadyToShootTimer->StartTimer();
        }
    } else if (mProjectileMarkerActor->IsEnabled()) {
        const auto& mousePosition = mouseBindings->GetLastMouseCursorPosition();
        const glm::ivec2& screenSpacePosition = glm::ivec2(mousePosition.x, mousePosition.y);
        const auto& worldSpaceRay = mSmartPicker->CreateWorldSpaceRayFromScreenSpacePosition(sceneCameraSp, screenSpacePosition);

        const glm::vec4 planeAtOrigin = glm::vec4(0, 1, 0, 0);
        const float tParam = EngineMath::RaycastPlane(sceneCameraSp->GetEyeVector(), worldSpaceRay, planeAtOrigin);
        if (tParam >= 0.0f) {
            const auto& placementPosition = sceneCameraSp->GetEyeVector() + (worldSpaceRay * tParam);
            mProjectileMarkerActor->GetRootComponent()->SetTranslation(placementPosition);
        }
    }
}

int32_t UserInteractionController::GetSelectedSpaceStationId() const
{
    return mSelectedSpaceStationId;
}

void UserInteractionController::ProcessEvent(
    const ChangeGameModeEvent* sender, const typename ChangeGameModeEvent::EventData_t& data)
{
    const eGameModeType newGameModeType = std::get<0>(data);
    if (mCurrentGameModeType != newGameModeType) {
        mCurrentGameModeType = newGameModeType;

        if (eGameModeType::COMBAT == mCurrentGameModeType) {
            SetUserInteractionType(eUserInteractionType::IDLE);
        }
    }
}

void UserInteractionController::ProcessEvent(
    const BroadcastGameThreadEvent* sender, const typename BroadcastGameThreadEvent::EventData_t& data)
{
    const auto eventHeader = std::get<0>(data);
    const auto jsonParamsStr = std::get<1>(data);
    if ("CombatLevelEvents" == eventHeader) {
        const auto jsonRoot = nlohmann::json::parse(jsonParamsStr);
        if (jsonRoot.at("action").get<std::string>() == "tower_grid_visibility") {
            const bool isTowerGridVisible = jsonRoot.at("visible").get<bool>();
            mTowerPlacementGridActor->SetIsEnabled(isTowerGridVisible);
            mPlacementAllowedAreaActor->SetIsEnabled(isTowerGridVisible);
        } else if (jsonRoot.at("action").get<std::string>() == "ghost_tower_visibility") {
            const bool isGhostTowerVisible = jsonRoot.at("visible").get<bool>();
            if (isGhostTowerVisible) {
                const auto towerTypeStr = jsonRoot.at("tower_type").get<std::string>();
                mTowerMissileType = MissileTypeFromString(towerTypeStr);
            }
            SetUserInteractionType(
                isGhostTowerVisible ? eUserInteractionType::TOWER_PLACE_SELECTION : eUserInteractionType::IDLE);
        } else if (jsonRoot.at("action").get<std::string>() == "remove_tower_marker_visibility") {
            const bool isTowerEraserMarkerVisible = jsonRoot.at("visible").get<bool>();
            mTowerMissileType = eMissileType::NONE;
            SetUserInteractionType(
                isTowerEraserMarkerVisible ? eUserInteractionType::TOWER_REMOVEMENT_SELECTION : eUserInteractionType::IDLE);
        }
    }
}

void UserInteractionController::SetOnShootCallback(const std::function<void()>& callback)
{
    mShootCallback = callback;
}

void UserInteractionController::SetActorsPoolHandler(const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler)
{
    mCombatActorsPoolHandler = combatActorsPoolHandler;
}

void UserInteractionController::ShowMissileProjectile()
{
    ext_assert(eGameModeType::COMBAT == mCurrentGameModeType, "Cannot show missile projectile in non-combat game mode");
    mProjectileMarkerActor->SetIsEnabled(true);
}

void UserInteractionController::HideMissileProjectile()
{
    mProjectileMarkerActor->SetIsEnabled(false);
}

glm::vec3 UserInteractionController::GetProjectileMarkerPosition() const
{
    return mProjectileMarkerActor->GetRootComponent()->GetTranslation();
}

void UserInteractionController::InitializeTowerGrid()
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in InitializeTowerGrid");
    const auto halfExtent = mLevelBounds.GetHalfExtent().x;
    mLevelPlacementGrid = std::make_unique<LevelPlacementGrid>(BoundingBox2D<glm::vec2>(glm::vec2(), glm::vec2(halfExtent)));
    mTowerPlacementGridActor = std::make_shared<Actor>(
        "TowerPlacementGridActor",
        std::make_shared<SceneComponent>("TowerPlacementGridActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f)));
    sceneSp->AddActor(mTowerPlacementGridActor);
    const glm::ivec2 towerGridColumnsAndRowsCount = mLevelPlacementGrid->GetTowerGridColumnsAndRowsCount();
    const int32_t columnsLineCount = towerGridColumnsAndRowsCount.x + 1;
    const int32_t rowsLineCount = towerGridColumnsAndRowsCount.y + 1;

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& lineMaterial = materialParser.ParseMaterialDescriptor("TowerPlacementGridMaterial.m");
    sceneSp->RegisterMaterialInstance(lineMaterial);
    MaterialPropertySetter::SetMaterialPropertyValue(lineMaterial, "opacity", 0.5f);
    MaterialPropertySetter::SetMaterialPropertyValue(lineMaterial, "color", glm::vec3(0.5f, 0.5f, 1.0f));

    const auto& rtMeshComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedLineComponent>>();
    const auto& levelAreaBoundingBox = mLevelPlacementGrid->GetTowerLevelAreaBoundingBox();

    const int32_t leftSideColumnsCount = columnsLineCount / 2;
    const int32_t rightSideColumnsCount = columnsLineCount - leftSideColumnsCount;
    static constexpr float grid_elevation_bias = 0.01f;
    const auto gridCellSize = mLevelPlacementGrid->GetGridCellSizeForTower();
    for (int32_t columnIdx = 0; columnIdx < columnsLineCount; ++columnIdx) {
        const auto& d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>(
            std::string("c_towerGridColumnLineMesh_" + std::to_string(columnIdx)),
            4,
            glm::vec3(),
            glm::vec3(),
            glm::vec3(1),
            "",
            lineMaterial);
        const auto& c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(
            sceneSp->CreateComponent_GameThread(rtMeshComponentCreator, d_mesh));
        c_mesh->SetCanBloomBeApplied(false);
        const auto& lineBegin = glm::vec3(
            levelAreaBoundingBox.GetMin().x + columnIdx * gridCellSize, -grid_elevation_bias, levelAreaBoundingBox.GetMin().y);
        const auto& lineEnd = glm::vec3(
            levelAreaBoundingBox.GetMin().x + columnIdx * gridCellSize, -grid_elevation_bias, levelAreaBoundingBox.GetMax().y);
        c_mesh->SetSortOrderValue(10);
        c_mesh->SetLineBeginWorldSpacePosition(lineBegin);
        c_mesh->SetLineEndWorldSpacePosition(lineEnd);
        c_mesh->SetLineWidth(0.3f);
        mTowerPlacementGridActor->AddComponent(c_mesh);
    }

    for (int32_t rowIdx = 0; rowIdx < rowsLineCount; ++rowIdx) {
        const auto& d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>(
            std::string("c_towerGridRowLineMesh_" + std::to_string(rowIdx)),
            4,
            glm::vec3(0),
            glm::vec3(0.0),
            glm::vec3(1),
            "",
            lineMaterial);
        const auto& c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(
            sceneSp->CreateComponent_GameThread(rtMeshComponentCreator, d_mesh));
        c_mesh->SetCanBloomBeApplied(false);
        const auto& lineBegin = glm::vec3(
            levelAreaBoundingBox.GetMin().x,
            -grid_elevation_bias * 2.0f,
            levelAreaBoundingBox.GetMin().y + rowIdx * gridCellSize);
        const auto& lineEnd = glm::vec3(
            levelAreaBoundingBox.GetMax().x,
            -grid_elevation_bias * 2.0f,
            levelAreaBoundingBox.GetMin().y + rowIdx * gridCellSize);
        c_mesh->SetSortOrderValue(-10);
        c_mesh->SetLineBeginWorldSpacePosition(lineBegin);
        c_mesh->SetLineEndWorldSpacePosition(lineEnd);
        c_mesh->SetLineWidth(0.3f);
        mTowerPlacementGridActor->AddComponent(c_mesh);
    }
}

void UserInteractionController::InitializePlacementAllowedArea()
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in InitializePlacementAllowedArea");
    mPlacementAllowedAreaActor = std::make_shared<Actor>(
        "PlacementAllowedAreaActor",
        std::make_shared<SceneComponent>("PlacementAllowedAreaActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f)));
    sceneSp->AddActor(mPlacementAllowedAreaActor);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& placementAllowedAreaMaterial
        = materialParser.ParseMaterialDescriptor("AlbedoColorWithOpacityMaterial.m");
    sceneSp->RegisterMaterialInstance(placementAllowedAreaMaterial);
    MaterialPropertySetter::SetMaterialPropertyValue(placementAllowedAreaMaterial, "opacity", 0.2f);
    MaterialPropertySetter::SetMaterialPropertyValue(placementAllowedAreaMaterial, "color", glm::vec3(0.0f, 1.0f, 0.2f));

    const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(false);

    for (const auto& placementCellPosition : mTowerPlacementCells) {
        const auto& realCellOriginPositionVec2
            = mLevelPlacementGrid
                  ->GetNearestToPositionTowerCellBoundingBox(glm::vec2(placementCellPosition.x, placementCellPosition.z))
                  .GetOrigin();

        const auto& d_mesh = std::make_shared<MeshComponentData>(
            "PlacementAllowedAreaMeshComponent",
            "plane.obj",
            glm::vec3(realCellOriginPositionVec2.x, 0.0f, realCellOriginPositionVec2.y),
            glm::vec3(),
            glm::vec3(mTowerCellSize, 1.0f, mTowerCellSize),
            "",
            placementAllowedAreaMaterial);
        const auto& c_mesh
            = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
        c_mesh->SetSortOrderValue(20);
        mPlacementAllowedAreaActor->AddComponent(c_mesh);
    }
}

void UserInteractionController::InitializeGhostTower()
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in InitializeGhostTower");
    sceneSp->AddActor(mGhostTowerActor);

    const auto& albedoName = "Space_Station_COLOR.png";
    const auto& albedoTexture = TexturePool::GetInstance()->GetOrAllocateResource(albedoName);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& ghostTowerActorMaterial = materialParser.ParseMaterialDescriptor("GhostTowerMaterial.m");
    sceneSp->RegisterMaterialInstance(ghostTowerActorMaterial);
    MaterialPropertySetter::SetMaterialPropertyValue(ghostTowerActorMaterial, "opacity", 0.5f);
    MaterialPropertySetter::SetMaterialPropertyValue(ghostTowerActorMaterial, "uvScale", 1.0f);
    MaterialPropertySetter::SetMaterialPropertyValue(ghostTowerActorMaterial, "albedo", albedoTexture);
    MaterialPropertySetter::SetMaterialPropertyValue(ghostTowerActorMaterial, "blendFactor", 0.5f);
    MaterialPropertySetter::SetMaterialPropertyValue(ghostTowerActorMaterial, mGhostTowerActor, "p_blendColor", "b_blendColor");

    const auto& d_mesh = std::make_shared<MeshComponentData>(
        "PlacementAllowedAreaMeshComponent",
        "space_station.obj",
        glm::vec3(),
        glm::vec3(),
        glm::vec3(mTowerCellSize),
        "",
        ghostTowerActorMaterial);
    const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(false);
    const auto& c_mesh
        = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
    mGhostTowerActor->AddComponent(c_mesh);
}

void UserInteractionController::InitializeRemoveTowerMarker()
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in InitializeRemoveTowerMarker");
    sceneSp->AddActor(mRemoveTowerMarkerActor);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& billboard_material = materialParser.ParseMaterialDescriptor("BillboardMaterial.m");
    sceneSp->RegisterMaterialInstance(billboard_material);
    const auto mask_texture = TexturePool::GetInstance()->GetOrAllocateResource("default_circle_mask.png");
    const auto albedo_texture = TexturePool::GetInstance()->GetOrAllocateResource("cancel.png");

    MaterialPropertySetter::SetMaterialPropertyValue(billboard_material, "albedo", albedo_texture);
    MaterialPropertySetter::SetMaterialPropertyValue(billboard_material, "mask", mask_texture);
    MaterialPropertySetter::SetMaterialPropertyValue(billboard_material, "inverse_y", (int32_t) true);
    MaterialPropertySetter::SetMaterialPropertyValue(billboard_material, "use_mask", (int32_t) true);
    MaterialPropertySetter::SetMaterialPropertyValue(billboard_material, "fill_albedo_transparency_with_color", (int32_t) true);
    MaterialPropertySetter::SetMaterialPropertyValue(billboard_material, "use_custom_color_for_albedo", (int32_t) true);
    MaterialPropertySetter::SetMaterialPropertyValue(billboard_material, "albedo_custom_color", glm::vec3(1.0f, 0.0f, 0.0f));
    MaterialPropertySetter::SetMaterialPropertyValue(
        billboard_material, mRemoveTowerMarkerActor, "p_transparency_color_filler", "b_transparency_color_filler");

    auto billboardComponentCreator = std::make_shared<BillboardComponentCreator<BillboardComponent>>();
    const auto data = std::make_shared<BillboardComponentData>(
        "c_billboard_RemoveTowerMarkerActor",
        0.015f,
        true,
        glm::vec3(0.0f),
        0.0f,
        glm::vec3(1.0f),
        billboard_material,
        [](const glm::mat4& viewMatrix) { return glm::mat4(1); },
        [](const glm::mat4& projectionMatrix) { return glm::mat4(1); });
    const auto& billboardComponent
        = std::static_pointer_cast<BillboardComponent>(sceneSp->CreateComponent_GameThread(billboardComponentCreator, data));
    billboardComponent->SetSortOrderValue(10000);
    mRemoveTowerMarkerActor->AddComponent(billboardComponent);
}

void UserInteractionController::TriggerPlayerStatusChangedEvent(
    const eMainPlayerStatusType statusChanged, const std::string& jsonArgs)
{
    MainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, statusChanged, jsonArgs);

    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in TriggerPlayerStatusChangedEvent");
    static constexpr auto functionId = Hash64_CT("UserInteractionController::TriggerPlayerStatusChangedEvent");
    sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
        eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
        static_cast<int32_t>(statusChanged),
        functionId,
        [statusChanged, jsonArgs](
            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
            std::weak_ptr<EngineCore::Scene> sceneWp,
            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
            LuaMainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, statusChanged, jsonArgs);
        });
}

eMissileType UserInteractionController::MissileTypeFromString(const std::string& typeStr) const
{
    if (typeStr == "BOMB") {
        return eMissileType::BOMB;
    } else if (typeStr == "FREEZING_BOMB") {
        return eMissileType::FREEZING_BOMB;
    } else if (typeStr == "ELECTRO_RAY") {
        return eMissileType::ELECTRO_RAY;
    } else if (typeStr == "FREEZING_RAY") {
        return eMissileType::FREEZING_RAY;
    } else if (typeStr == "BLACK_HOLE") {
        return eMissileType::BLACK_HOLE;
    }
    return eMissileType::NONE;
}
} // namespace Game