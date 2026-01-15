#include "LevelEditorController.h"

#include "Core/CommonCore/JsonHelper.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedQuadraticBezierCurveComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/IoCore/FileFacade.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"
#include "Core/UtilityCore/ScreenRayCaster.h"
#include "Implementation/DataProviders/LevelDataProvider.h"
#include "Implementation/Levels/LevelSerializationHelper.h"

#include <json/json.hpp>

using namespace Resources;
using namespace EngineCore;
using namespace EngineCore::DataProviders;
using namespace Graphics;
using namespace IO;

namespace Game {
LevelEditorController::LevelEditorController(const std::weak_ptr<Scene>& sceneWp)
    : mSceneWp(sceneWp)
    , mInputComponent(std::make_shared<InputComponent>(std::make_shared<ComponentData>("LevelEditorController_InputComponent")))
    , mBezierCurvesActor(std::make_shared<Actor>(
          "BezierCurvesActor",
          std::make_shared<SceneComponent>("BezierCurvesActor_RootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f), true)))
    , mTowersActor(std::make_shared<Actor>(
          "TowersActor",
          std::make_shared<SceneComponent>("TowersActor_RootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f), true)))
    , mGhostTowerActor(std::make_shared<Actor>(
          "GhostTowerActor",
          std::make_shared<SceneComponent>("GhostTowerActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f), true)))
    , mGhostTowerBlendColorProperty(std::make_shared<EngineObjectProperty<glm::vec3>>(glm::vec3(0.0f), "p_blendColor"))
    , mRoutesHandler(mSceneWp, mBezierCurvesActor)
    , mTowersHandler(mSceneWp, mTowersActor)
    , mBarriersHandler(mSceneWp)
{
    mGhostTowerActor->AddEngineProperty(mGhostTowerBlendColorProperty);
}

LevelEditorController::~LevelEditorController()
{
    ChangeEditModeEvent::GetInstance()->RemoveListener(ChangeEditModeEvent::GetInstanceId());
    BroadcastGameThreadEvent::GetInstance()->RemoveListener(BroadcastGameThreadEvent::GetInstanceId());
}

void LevelEditorController::OnPreLevelInit()
{
    ChangeEditModeEvent::GetInstance()->AddListener(shared_from_this());
    BroadcastGameThreadEvent::GetInstance()->AddListener(shared_from_this());
}

void LevelEditorController::OnLevelInit()
{
}

void LevelEditorController::OnPostLevelInit()
{

    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "LevelEditorController scene pointer is null in OnPostLevelInit");
    const auto& mainCameraSp = std::dynamic_pointer_cast<ThirdPersonCamera>(sceneSp->GetMainCamera());
    ext_assert(mainCameraSp, "LevelEditorController main camera is null or not a ThirdPersonCamera");
    mMainSceneCamera = mainCameraSp;

    sceneSp->AddActor(mBezierCurvesActor);
    sceneSp->AddActor(mTowersActor);

    MaterialParser materialParser;
    mSplineMaterialPrefab = materialParser.ParseMaterialDescriptor("CurveLineMaterial.m");
    MaterialPropertySetter::SetMaterialPropertyValue(mSplineMaterialPrefab, "opacity", 1.0f);
    sceneSp->RegisterMaterialInstance(mSplineMaterialPrefab);

    mBarriersHandler.OnPostLevelInit();
}

void LevelEditorController::PostPlayLevelFinished()
{
}

void LevelEditorController::CleanUp()
{
}

glm::vec3 LevelEditorController::RaycastLevelPlane(bool& raycastWasSuccessfull, const glm::ivec2& screenSpacePoint)
{
    if (const auto& sceneCameraSp = mMainSceneCamera.lock()) {
        const auto& projectionMatrix = sceneCameraSp->GetViewProjectionInfo()->CreateProjectionMatrix();
        const auto& viewMatrix = sceneCameraSp->GetViewMatrix();

        const ScreenRayCaster screenRayCaster;
        const glm::vec3& worldSpaceRay = screenRayCaster.CastRayFromScreenSpaceToWorldSpace(
            screenSpacePoint,
            glm::ivec2(
                GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth() - 1,
                GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight() - 1),
            projectionMatrix,
            viewMatrix);
        const glm::vec4 planeAtOrigin = glm::vec4(0, 1, 0, 0);
        const float tParam = EngineMath::RaycastPlane(sceneCameraSp->GetEyeVector(), worldSpaceRay, planeAtOrigin);
        glm::vec3 rayIntersectionPosition = glm::vec3();
        if (tParam >= 0.0f) {
            rayIntersectionPosition = sceneCameraSp->GetEyeVector() + (worldSpaceRay * tParam);
            raycastWasSuccessfull = true;
        } else {
            raycastWasSuccessfull = false;
        }

        return rayIntersectionPosition;
    } else {
        raycastWasSuccessfull = false;
        return {};
    }
}

bool LevelEditorController::IsTowerPositionValid(const glm::vec3 position) const
{
    const auto& positionXZ = glm::vec2(position.x, position.z);
    if (!EngineMath::TestPointInAABB(mLevelAreaBoundingBox.GetMin(), mLevelAreaBoundingBox.GetMax(), positionXZ)) {
        return false;
    }

    const glm::vec2& cellBoundingBoxOrigin
        = mLevelPlacementGrid->GetNearestToPositionTowerCellBoundingBox(positionXZ).GetOrigin();

    const auto& allPlacedTowerPoints = mTowersHandler.CollectTowerPoints();
    bool isPlaceOccupiedByTower = std::any_of(
        allPlacedTowerPoints.cbegin(), allPlacedTowerPoints.cend(), [this, &cellBoundingBoxOrigin](const auto& pointsPair) {
            const glm::vec3& occupiedPosition = std::get<0>(pointsPair.second);
            const float halfScale = std::get<1>(pointsPair.second).x * 0.5f;
            const auto& validCellMin = glm::vec2(occupiedPosition.x - halfScale, occupiedPosition.z - halfScale);
            const auto& validCellMax = glm::vec2(occupiedPosition.x + halfScale, occupiedPosition.z + halfScale);

            return EngineMath::TestPointInAABB(validCellMin, validCellMax, cellBoundingBoxOrigin);
        });

    return !isPlaceOccupiedByTower;
}

void LevelEditorController::Tick(const float deltaTimeSec)
{
    if (eEditModeType::IDLE != mCurrentEditModeType) {
        const auto& mouseBindings = mInputComponent->GetMouseBindings();
        if (mouseBindings->IsMouseMoveEventDirty()) {
            const auto& mouseMoveEvent = mouseBindings->FlushMouseMoveEvent();
            const glm::ivec2& screenSpacePosition = glm::ivec2(mouseMoveEvent.x, mouseMoveEvent.y);
            bool rayCastWasSuccessfull;
            const auto& rayIntersectionPosition = RaycastLevelPlane(rayCastWasSuccessfull, screenSpacePosition);
            if (rayCastWasSuccessfull) {
                if (eEditModeType::EDIT_TOWERS == mCurrentEditModeType) {
                    const auto& nearestCellBoundingBox = mLevelPlacementGrid->GetNearestToPositionTowerCellBoundingBox(
                        glm::vec2(rayIntersectionPosition.x, rayIntersectionPosition.z));
                    const auto pickerCellHalfSize = mLevelPlacementGrid->GetGridCellSizeForTower() * 0.5f;

                    const auto& cellOriginPosition = nearestCellBoundingBox.GetOrigin();
                    const auto cellPositionVec3 = glm::vec3(cellOriginPosition.x, 0.0f, cellOriginPosition.y);

                    const glm::vec3 ghostTowerPositionValidationColor
                        = IsTowerPositionValid(cellPositionVec3) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
                    mGhostTowerBlendColorProperty->SetValue(ghostTowerPositionValidationColor);
                    const glm::vec3& markerPosition
                        = glm::vec3(nearestCellBoundingBox.GetOrigin().x, 0.0f, nearestCellBoundingBox.GetOrigin().y);
                    mGhostTowerActor->GetRootComponent()->SetTranslation(markerPosition);
                    mTowerPlacementPickerActor->GetRootComponent()->SetTranslation(markerPosition);
                } else {
                    const auto& nearestRouteNodePosition = mLevelPlacementGrid->GetNearestToPositionRouteEdgeNode(
                        glm::vec2(rayIntersectionPosition.x, rayIntersectionPosition.z));
                    mRouteNodePickerActor->GetRootComponent()->SetTranslation(
                        glm::vec3(nearestRouteNodePosition.x, 0.0f, nearestRouteNodePosition.y));
                }
            }
        }

        if (KeyState::PRESSED == mouseBindings->GetKeyState(EngineCore::eMouseKeys::MouseButtonLeft)) {
            if (!leftButtonPressed) {
                leftButtonPressed = true;
                const auto& mouseMoveEvent = mouseBindings->GetLastMouseCursorPosition();
                const glm::ivec2& screenSpacePosition = glm::ivec2(mouseMoveEvent.x, mouseMoveEvent.y);
                bool rayCastWasSuccessfull;
                const auto& rayIntersectionPosition = RaycastLevelPlane(rayCastWasSuccessfull, screenSpacePosition);
                if (rayCastWasSuccessfull) {
                    if (eEditModeType::EDIT_ROUTES == mCurrentEditModeType) {
                        const auto& nearestRouteNodePosition = mLevelPlacementGrid->GetNearestToPositionRouteEdgeNode(
                            glm::vec2(rayIntersectionPosition.x, rayIntersectionPosition.z));
                        const auto& newControlPoint = glm::vec3(nearestRouteNodePosition.x, 0.0f, nearestRouteNodePosition.y);
                        mRoutesHandler.AddPointToActiveRoute(mSplineMaterialPrefab, newControlPoint);
                    } else if (eEditModeType::EDIT_TOWERS == mCurrentEditModeType) {
                        const auto& nearestCellBoundingBox = mLevelPlacementGrid->GetNearestToPositionTowerCellBoundingBox(
                            glm::vec2(rayIntersectionPosition.x, rayIntersectionPosition.z));
                        const auto pickerCellSize = mLevelPlacementGrid->GetGridCellSizeForTower();
                        const auto& tower2DPosition = nearestCellBoundingBox.GetOrigin();
                        const auto cellPositionVec3 = glm::vec3(tower2DPosition.x, 0.0f, tower2DPosition.y);
                        if (IsTowerPositionValid(cellPositionVec3)) {
                            mTowersHandler.CreateNewTower(
                                glm::vec3(tower2DPosition.x, 0.0f, tower2DPosition.y), glm::vec3(pickerCellSize * 0.5f));
                        }
                    } else if (eEditModeType::EDIT_BARRIERS == mCurrentEditModeType) {
                        const auto& nearestBarrierNodePosition = mLevelPlacementGrid->GetNearestToPositionRouteEdgeNode(
                            glm::vec2(rayIntersectionPosition.x, rayIntersectionPosition.z));
                        const auto& pillarPosition = glm::vec3(nearestBarrierNodePosition.x, 0.0f, nearestBarrierNodePosition.y);
                        mBarriersHandler.CreateNewBarrierPillar(pillarPosition, glm::vec3());
                    }
                }
            }
        } else {
            leftButtonPressed = false;
        }
    }
}

void LevelEditorController::UnpausableTick(const float deltaTimeSec)
{
}

void LevelEditorController::ProcessEvent(
    const BroadcastGameThreadEvent* sender, const BroadcastGameThreadEvent::EventData_t& data)
{
    const auto eventHeader = std::get<0>(data);

    if ("EditorLevelEvents" == eventHeader) {
        const auto& jsonObj = nlohmann::json::parse(std::get<1>(data));
        if (jsonObj.contains("action")) {
            const auto& doneAction = jsonObj["action"].get<std::string>();
            if ("undo" == doneAction) {
                if (eEditModeType::EDIT_ROUTES == mCurrentEditModeType) {
                    mRoutesHandler.UndoLastBezierCurveComponent();
                } else if (eEditModeType::EDIT_TOWERS == mCurrentEditModeType) {
                    mTowersHandler.UndoLastTowerComponent();
                } else if (eEditModeType::EDIT_BARRIERS == mCurrentEditModeType) {
                    mBarriersHandler.UndoLastBarrier();
                }
            } else if ("new_route" == doneAction) {
                if (jsonObj.contains("route_name")) {
                    const auto& newRouteName = nlohmann_utilities::GetStringFromJson(jsonObj, "route_name");
                    mRoutesHandler.SelectNewRouteAsActive(newRouteName);
                }
                if (jsonObj.contains("route_color")) {
                    const glm::vec3 newRouteColor = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["route_color"]);
                    mRoutesHandler.SetNewBezierCurveColor(newRouteColor);
                }
            } else if ("new_barrier" == doneAction) {
                if (jsonObj.contains("barrier_name")) {
                    const auto& newBarrierName = nlohmann_utilities::GetStringFromJson(jsonObj, "barrier_name");
                    mBarriersHandler.SelectNewBarrier(newBarrierName);
                }
                if (jsonObj.contains("barrier_color")) {
                    const glm::vec3 newBarrierColor = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["barrier_color"]);
                    mBarriersHandler.SetBarrierColor(newBarrierColor);
                    mBarriersHandler.SetRayColor(newBarrierColor);
                }
            } else if ("save" == doneAction) {
                std::string lvlName = "unknown";
                if (jsonObj.contains("name")) {
                    lvlName = nlohmann_utilities::GetStringFromJson(jsonObj, "name");
                }

                LevelData lvlData;
                lvlData.LevelName = lvlName;
                lvlData.LevelBoundaryMin = mLevelAreaBoundingBox.GetMin();
                lvlData.LevelBoundaryMax = mLevelAreaBoundingBox.GetMax();
                lvlData.RoutesData = mRoutesHandler.CollectRoutesControlPoints();
                lvlData.TowersData = mTowersHandler.CollectTowerPoints();
                lvlData.BarriersData = mBarriersHandler.CollectBarrierPoints();

                ext_assert(lvlData.isDataValid(), "Some data is missing. Level has to include name, routes and towers");
                LevelSerializationHelper lvlSerialization;
                const std::string& serializedPathJsonStr = lvlSerialization.DumpLevelToJsonString(lvlData);
                FileFacade fileFacade;
                fileFacade.OpenAndReadFile(lvlName);
                fileFacade.RewriteSrc(serializedPathJsonStr);
                fileFacade.WriteToFile();
            } else if ("set_level_width" == doneAction) {
                if (jsonObj.contains("width")) {
                    const auto width = nlohmann_utilities::GetFloatFromJson(jsonObj, "width");
                    mLevelAreaBoundingBox.SetHalfExtentX(width * 0.5f);
                    LevelDataProvider::GetInstance()->SetEditorLevelAreaBoundingBox(mLevelAreaBoundingBox, true, false);
                    mLevelPlacementGrid->UpdateLevelAreaBoundingBox(mLevelAreaBoundingBox);
                    ReAllocateLineComponents();
                    UpdateVisibility();
                }
            } else if ("set_level_length" == doneAction) {
                if (jsonObj.contains("length")) {
                    const auto length = nlohmann_utilities::GetFloatFromJson(jsonObj, "length");
                    mLevelAreaBoundingBox.SetHalfExtentY(length * 0.5f);
                    LevelDataProvider::GetInstance()->SetEditorLevelAreaBoundingBox(mLevelAreaBoundingBox, true, false);
                    mLevelPlacementGrid->UpdateLevelAreaBoundingBox(mLevelAreaBoundingBox);
                    ReAllocateLineComponents();
                    UpdateVisibility();
                }
            }
        }
    }
}

void LevelEditorController::UpdateVisibility()
{
    const bool isVisibleTowerPlacementGridActor = eEditModeType::EDIT_TOWERS == mCurrentEditModeType;
    const bool isVisibleRoutePlacementGridActor = eEditModeType::EDIT_ROUTES == mCurrentEditModeType;
    const bool isVisibleBarrierPlacementGridActor = eEditModeType::EDIT_BARRIERS == mCurrentEditModeType;
    mTowerPlacementGridActor->SetIsEnabled(isVisibleTowerPlacementGridActor);
    mGhostTowerActor->SetIsEnabled(isVisibleTowerPlacementGridActor);
    mTowerPlacementPickerActor->SetIsEnabled(isVisibleTowerPlacementGridActor);
    mRoutePlacementGridActor->SetIsEnabled(isVisibleRoutePlacementGridActor || isVisibleBarrierPlacementGridActor);
    mRouteNodePickerActor->SetIsEnabled(isVisibleRoutePlacementGridActor || isVisibleBarrierPlacementGridActor);
}

void LevelEditorController::ProcessEvent(const ChangeEditModeEvent* sender, const ChangeEditModeEvent::EventData_t& data)
{
    const auto editModeType = std::get<0>(data);
    if (mCurrentEditModeType != editModeType) {
        mCurrentEditModeType = editModeType;
        UpdateVisibility();
    }
}

void LevelEditorController::Initialize()
{
    mLevelAreaBoundingBox = LevelDataProvider::GetInstance()->GetEditorLevelAreaBoundingBox();
    ext_assert(
        mLevelAreaBoundingBox.GetHalfExtent().length() > 0.001, "LevelEditorController level area bounding box has zero size");
    mLevelPlacementGrid = std::make_unique<LevelPlacementGrid>(mLevelAreaBoundingBox);
    InitializeGhostTower();
    InitializeInternalActors();
    ReAllocateLineComponents();
    UpdateVisibility();
}

void LevelEditorController::RestoreLineComponentsPool()
{
    const auto& activeTowerComponents = mTowerPlacementGridActor->GetComponentsByType<RuntimeGeneratedLineComponent>();
    mTowerPlacementGridActor->RemoveComponentsByType<RuntimeGeneratedLineComponent>();
    for (const auto& c_mesh : activeTowerComponents) {
        c_mesh->SetIsEnabled(false);
        mIdleRuntimeGeneratedLineComponents.push(std::static_pointer_cast<RuntimeGeneratedLineComponent>(c_mesh));
    }

    const auto& activeRouteComponents = mRoutePlacementGridActor->GetComponentsByType<RuntimeGeneratedLineComponent>();
    mRoutePlacementGridActor->RemoveComponentsByType<RuntimeGeneratedLineComponent>();
    for (const auto& c_mesh : activeRouteComponents) {
        c_mesh->SetIsEnabled(false);
        mIdleRuntimeGeneratedLineComponents.push(std::static_pointer_cast<RuntimeGeneratedLineComponent>(c_mesh));
    }
}

void LevelEditorController::ReAllocateLineComponents()
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "LevelEditorController scene pointer is null in ReAllocateLineComponents");
    const auto& rtMeshComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedLineComponent>>();
    const glm::ivec2 routeGridColumnsAndRowsCount = mLevelPlacementGrid->GetRouteGridColumnsAndRowsCount();
    const glm::ivec2 towerGridColumnsAndRowsCount = mLevelPlacementGrid->GetTowerGridColumnsAndRowsCount();
    const int32_t routeColumnsLineCount = routeGridColumnsAndRowsCount.x + 1;
    const int32_t routeRowsLineCount = routeGridColumnsAndRowsCount.y + 1;
    const int32_t towerGridColumnsLineCount = towerGridColumnsAndRowsCount.x + 1;
    const int32_t towerGridRowsLineCount = towerGridColumnsAndRowsCount.y + 1;
    const int32_t totalLineCount
        = routeColumnsLineCount + routeRowsLineCount + towerGridColumnsLineCount + towerGridRowsLineCount;

    RestoreLineComponentsPool();
    const int32_t allocatedLinesCount = static_cast<int32_t>(mIdleRuntimeGeneratedLineComponents.size());
    if (allocatedLinesCount < totalLineCount) {

        const int32_t linesToAllocateCount = totalLineCount - allocatedLinesCount;
        int32_t nameIndex = allocatedLinesCount;
        for (int32_t idx = 0; idx < linesToAllocateCount; ++idx) {
            const auto& d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>(
                std::string("c_lineMesh_" + std::to_string(nameIndex++)),
                4,
                glm::vec3(),
                glm::vec3(),
                glm::vec3(1),
                mGridLineMaterialPrefab);
            const auto& c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(
                sceneSp->CreateComponent_GameThread(rtMeshComponentCreator, d_mesh));
            c_mesh->SetCanBloomBeApplied(false);
            mIdleRuntimeGeneratedLineComponents.push(c_mesh);
        }
    }
    InitializeRoutePlacementGrid();
    InitializeTowerPlacementGrid();
}

void LevelEditorController::InitializeInternalActors()
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "LevelEditorController scene pointer is null in InitializeInternalActors");

    MaterialParser materialParser;
    mGridLineMaterialPrefab = materialParser.ParseMaterialDescriptor("TowerPlacementGridMaterial.m");
    sceneSp->RegisterMaterialInstance(mGridLineMaterialPrefab);
    MaterialPropertySetter::SetMaterialPropertyValue(mGridLineMaterialPrefab, "opacity", 0.5f);
    MaterialPropertySetter::SetMaterialPropertyValue(mGridLineMaterialPrefab, "color", glm::vec3(0.5f, 0.5f, 1.0f));

    mRouteNodePickerActor = std::make_shared<Actor>(
        "RouteNodePickerActor",
        std::make_shared<SceneComponent>("RouteNodePickerActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f), true));
    sceneSp->AddActor(mRouteNodePickerActor);

    const auto& editorNodePickerMaterial = materialParser.ParseMaterialDescriptor("PbrSingleValueMaterial.m");
    sceneSp->RegisterMaterialInstance(editorNodePickerMaterial);
    MaterialPropertySetter::SetMaterialPropertyValue(editorNodePickerMaterial, "albedo", glm::vec3(1.0f, 0.0f, 0.0f));
    MaterialPropertySetter::SetMaterialPropertyValue(editorNodePickerMaterial, "metallicValue", 1.8f);
    MaterialPropertySetter::SetMaterialPropertyValue(editorNodePickerMaterial, "roughnessValue", 0.5f);

    const auto pickerSize = 2.0f;
    const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
    const auto& d_mesh = std::make_shared<MeshComponentData>(
        "RoutePickerMeshComponent",
        "sphere.obj",
        glm::vec3(),
        glm::vec3(),
        glm::vec3(pickerSize, 1.0f, pickerSize),
        editorNodePickerMaterial);
    const auto& c_mesh
        = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
    c_mesh->SetSortOrderValue(1);
    mRouteNodePickerActor->AddComponent(c_mesh);

    mRoutePlacementGridActor = std::make_shared<Actor>(
        "RoutePlacementGridActor",
        std::make_shared<SceneComponent>(
            "RoutePlacementGridActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f), true));
    sceneSp->AddActor(mRoutePlacementGridActor);

    mTowerPlacementGridActor = std::make_shared<Actor>(
        "TowerPlacementGridActor",
        std::make_shared<SceneComponent>(
            "TowerPlacementGridActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f), true));
    sceneSp->AddActor(mTowerPlacementGridActor);
}

void LevelEditorController::InitializeRoutePlacementGrid()
{
    const glm::ivec2 routeGridColumnsAndRowsCount = mLevelPlacementGrid->GetRouteGridColumnsAndRowsCount();
    const int32_t columnsLineCount = routeGridColumnsAndRowsCount.x + 1;
    const int32_t rowsLineCount = routeGridColumnsAndRowsCount.y + 1;
    ext_assert(
        (columnsLineCount + rowsLineCount) <= mIdleRuntimeGeneratedLineComponents.size(),
        "LevelEditorController insufficient idle line components for route grid");

    const auto& levelAreaBoundingBox = mLevelPlacementGrid->GetRouteLevelAreaBoundingBox();

    static constexpr float grid_elevation_bias = 1.0f;
    const auto gridCellSize = mLevelPlacementGrid->GetGridCellSizeForRoute();
    for (int32_t columnIdx = 0; columnIdx < columnsLineCount; ++columnIdx) {
        const auto c_mesh = mIdleRuntimeGeneratedLineComponents.top();
        mIdleRuntimeGeneratedLineComponents.pop();
        ext_assert(c_mesh, "LevelEditorController route grid line component is null");
        const auto& lineBegin = glm::vec3(
            levelAreaBoundingBox.GetMin().x + columnIdx * gridCellSize, -grid_elevation_bias, levelAreaBoundingBox.GetMin().y);
        const auto& lineEnd = glm::vec3(
            levelAreaBoundingBox.GetMin().x + columnIdx * gridCellSize, -grid_elevation_bias, levelAreaBoundingBox.GetMax().y);
        c_mesh->SetSortOrderValue(0);
        c_mesh->SetLineBeginWorldSpacePosition(lineBegin);
        c_mesh->SetLineEndWorldSpacePosition(lineEnd);
        c_mesh->SetLineWidth(0.3f);
        c_mesh->SetIsEnabled(true);
        mRoutePlacementGridActor->AddComponent(c_mesh);
    }

    for (int32_t rowIdx = 0; rowIdx < rowsLineCount; ++rowIdx) {
        const auto c_mesh = mIdleRuntimeGeneratedLineComponents.top();
        mIdleRuntimeGeneratedLineComponents.pop();
        const auto& lineBegin = glm::vec3(
            levelAreaBoundingBox.GetMin().x,
            -grid_elevation_bias * 1.5f,
            levelAreaBoundingBox.GetMin().y + rowIdx * gridCellSize);
        const auto& lineEnd = glm::vec3(
            levelAreaBoundingBox.GetMax().x,
            -grid_elevation_bias * 1.5f,
            levelAreaBoundingBox.GetMin().y + rowIdx * gridCellSize);
        c_mesh->SetSortOrderValue(0);
        c_mesh->SetLineBeginWorldSpacePosition(lineBegin);
        c_mesh->SetLineEndWorldSpacePosition(lineEnd);
        c_mesh->SetLineWidth(0.3f);
        c_mesh->SetIsEnabled(true);
        mRoutePlacementGridActor->AddComponent(c_mesh);
    }
}

void LevelEditorController::InitializeTowerPlacementGrid()
{
    const glm::ivec2 towerGridColumnsAndRowsCount = mLevelPlacementGrid->GetTowerGridColumnsAndRowsCount();
    const int32_t columnsLineCount = towerGridColumnsAndRowsCount.x + 1;
    const int32_t rowsLineCount = towerGridColumnsAndRowsCount.y + 1;
    ext_assert(
        (columnsLineCount + rowsLineCount) <= mIdleRuntimeGeneratedLineComponents.size(),
        "LevelEditorController insufficient idle line components for tower grid");

    const auto& levelAreaBoundingBox = mLevelPlacementGrid->GetTowerLevelAreaBoundingBox();

    const int32_t leftSideColumnsCount = columnsLineCount / 2;
    const int32_t rightSideColumnsCount = columnsLineCount - leftSideColumnsCount;
    static constexpr float grid_elevation_bias = 1.0f;
    const auto gridCellSize = mLevelPlacementGrid->GetGridCellSizeForTower();
    for (int32_t columnIdx = 0; columnIdx < columnsLineCount; ++columnIdx) {
        const auto c_mesh = mIdleRuntimeGeneratedLineComponents.top();
        mIdleRuntimeGeneratedLineComponents.pop();
        ext_assert(c_mesh, "LevelEditorController tower grid column line component is null");
        const auto& lineBegin = glm::vec3(
            levelAreaBoundingBox.GetMin().x + columnIdx * gridCellSize, -grid_elevation_bias, levelAreaBoundingBox.GetMin().y);
        const auto& lineEnd = glm::vec3(
            levelAreaBoundingBox.GetMin().x + columnIdx * gridCellSize, -grid_elevation_bias, levelAreaBoundingBox.GetMax().y);
        c_mesh->SetSortOrderValue(0);
        c_mesh->SetLineBeginWorldSpacePosition(lineBegin);
        c_mesh->SetLineEndWorldSpacePosition(lineEnd);
        c_mesh->SetLineWidth(0.3f);
        c_mesh->SetIsEnabled(true);
        mTowerPlacementGridActor->AddComponent(c_mesh);
    }

    for (int32_t rowIdx = 0; rowIdx < rowsLineCount; ++rowIdx) {
        const auto c_mesh = mIdleRuntimeGeneratedLineComponents.top();
        mIdleRuntimeGeneratedLineComponents.pop();
        ext_assert(c_mesh, "LevelEditorController tower grid row line component is null");
        const auto& lineBegin = glm::vec3(
            levelAreaBoundingBox.GetMin().x,
            -grid_elevation_bias * 1.5f,
            levelAreaBoundingBox.GetMin().y + rowIdx * gridCellSize);
        const auto& lineEnd = glm::vec3(
            levelAreaBoundingBox.GetMax().x,
            -grid_elevation_bias * 1.5f,
            levelAreaBoundingBox.GetMin().y + rowIdx * gridCellSize);
        c_mesh->SetSortOrderValue(0);
        c_mesh->SetLineBeginWorldSpacePosition(lineBegin);
        c_mesh->SetLineEndWorldSpacePosition(lineEnd);
        c_mesh->SetLineWidth(0.3f);
        c_mesh->SetIsEnabled(true);
        mTowerPlacementGridActor->AddComponent(c_mesh);
    }
}

void LevelEditorController::InitializeGhostTower()
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "LevelEditorController scene pointer is null in InitializeGhostTower");
    sceneSp->AddActor(mGhostTowerActor);

    const auto& albedoName = "Space_Station_COLOR.png";
    const auto& albedoTexture = TexturePool::GetInstance()->GetOrAllocateResource(albedoName);

    const auto towerCellSize = mLevelPlacementGrid->GetGridCellSizeForTower() * 0.5f;

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
        glm::vec3(towerCellSize),
        ghostTowerActorMaterial);
    const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(false);
    const auto& c_mesh
        = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
    mGhostTowerActor->AddComponent(c_mesh);

    // tower placement picker
    const auto pickerCellSize = mLevelPlacementGrid->GetGridCellSizeForTower();
    mTowerPlacementPickerActor = std::make_shared<Actor>(
        "TowerPlacementPickerActor",
        std::make_shared<SceneComponent>(
            "TowerPlacementPickerActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f), true));
    sceneSp->AddActor(mTowerPlacementPickerActor);

    const std::shared_ptr<IMaterial>& editorPickerMaterial = materialParser.ParseMaterialDescriptor("EditorPickerMaterial.m");
    sceneSp->RegisterMaterialInstance(editorPickerMaterial);
    MaterialPropertySetter::SetMaterialPropertyValue(editorPickerMaterial, "opacity", 1.0f);
    MaterialPropertySetter::SetMaterialPropertyValue(editorPickerMaterial, "color", glm::vec3(0.4f, 0.8f, 0.2f));
    MaterialPropertySetter::SetMaterialPropertyValue(editorPickerMaterial, sceneSp, "GT_DeltaSec", "gt_timeSec");

    const auto& d_pickerMesh = std::make_shared<MeshComponentData>(
        "TowerPlacementMeshComponent",
        "plane.obj",
        glm::vec3(),
        glm::vec3(),
        glm::vec3(pickerCellSize, 1.0f, pickerCellSize),
        editorPickerMaterial);
    const auto& c_pickerMesh
        = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_pickerMesh));
    c_pickerMesh->SetSortOrderValue(1);
    mTowerPlacementPickerActor->AddComponent(c_pickerMesh);
}

} // namespace Game
