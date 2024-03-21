#include "LevelEditorController.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedQuadraticBezierCurveComponent.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ForwardShadingMeshComponentCreator.h"
#include "Core/GameCore/Components/PrimitiveComponents/ForwardShadingMeshComponent.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/UtilityCore/ScreenRayCaster.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/IoCore/FileFacade.h"
#include "Implementation/Navigation/PathSerializationHelper.h"

#include <json/json.hpp>

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;
using namespace IO;

namespace Game
{
    LevelEditorController::LevelEditorController(const std::weak_ptr<Scene> &sceneWp)
        : mSceneWp(sceneWp),
          mInputComponent(std::make_shared<InputComponent>(std::make_shared<ComponentData>("LevelEditorController_InputComponent"))),
          mBezierCurvesActor(std::make_shared<Actor>("BezierCurvesActor",
                                                     std::make_shared<SceneComponent>("BezierCurvesActor_RootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f))))
    {
    }

    static std::shared_ptr<IMaterial> splineMaterial = nullptr;

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
        const auto &sceneSp = mSceneWp.lock();
        assert(sceneSp);
        const auto &mainCameraSp = std::dynamic_pointer_cast<ThirdPersonCamera>(sceneSp->GetMainCamera());
        assert(mainCameraSp);
        mMainSceneCamera = mainCameraSp;

        sceneSp->AddActor(mBezierCurvesActor);

        MaterialParser materialParser;
        splineMaterial = materialParser.ParseMaterialDescriptor("CurveLineMaterial.m");
        MaterialPropertySetter::SetMaterialPropertyValue(splineMaterial, "opacity", 1.0f);
        MaterialPropertySetter::SetMaterialPropertyValue(splineMaterial, "color", glm::vec3(0.5f, 0.7f, 0.2f));
        sceneSp->RegisterMaterialInstance(splineMaterial);
    }

    void LevelEditorController::PostPlayLevelFinished()
    {
    }

    void LevelEditorController::CleanUp()
    {
    }

    void LevelEditorController::SetLevelAreaBoundingBox(const BoundingBox2D<glm::vec2> &levelAreaBoundingBox)
    {
        mLevelAreaBoundingBox = levelAreaBoundingBox;
        Initialize();
    }

    void LevelEditorController::SpawnBezierCurveComponent(const int32_t bezierIndex, const glm::vec3 &pointA, const glm::vec3 &controlPoint, const glm::vec3 &pointB)
    {
        if (mIdleCurveComponents.empty())
        {
            const auto &sceneSp = mSceneWp.lock();
            const auto &d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>("c_bezierCurveLineMesh_" + std::to_string(bezierIndex), 150, glm::vec3(0, 5, 0), glm::vec3(), glm::vec3(1), "", splineMaterial);
            const auto &meshComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedQuadraticBezierCurveComponent>>();
            auto c_mesh = std::static_pointer_cast<RuntimeGeneratedQuadraticBezierCurveComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
            c_mesh->SetLineWidth(1.5f);
            c_mesh->SetSortOrderValue(30);
            c_mesh->SetCurveSegmentsCount(50);
            c_mesh->SetLineBeginWorldSpacePosition(pointA);
            c_mesh->SetBezierControlPointWorldSpacePosition(controlPoint);
            c_mesh->SetLineEndWorldSpacePosition(pointB);
            mBezierCurvesActor->AddComponent(c_mesh);
            mActiveCurveComponents.emplace(c_mesh);
        }
        else
        {
            const auto &idleCurveComponent = mIdleCurveComponents.top();
            idleCurveComponent->SetIsEnabled(true);
            idleCurveComponent->SetLineBeginWorldSpacePosition(pointA);
            idleCurveComponent->SetBezierControlPointWorldSpacePosition(controlPoint);
            idleCurveComponent->SetLineEndWorldSpacePosition(pointB);
            mActiveCurveComponents.emplace(idleCurveComponent);
            mIdleCurveComponents.pop();
        }
    }

    void LevelEditorController::Tick(const float deltaTime)
    {
        if (bezierControlPointsList.size() >= 3)
        {
            static int sBezierCurveCounter = 0;
            SpawnBezierCurveComponent(sBezierCurveCounter++, bezierControlPointsList.at(0), bezierControlPointsList.at(1), bezierControlPointsList.at(2));
            bezierControlPointsList.clear();
        }

        if (const auto &sceneCameraSp = mMainSceneCamera.lock())
        {
            if (eEditModeType::IDLE != mCurrentEditModeType)
            {
                const auto projectionMatrix = sceneCameraSp->GetViewProjectionInfo()->CreateProjectionMatrix();
                const auto &mouseBindings = mInputComponent->GetMouseBindings();
                if (mouseBindings->IsMouseMoveEventDirty())
                {
                    const auto &mouseMoveEvent = mouseBindings->FlushMouseMoveEvent();
                    const glm::ivec2 &screenSpacePosition = glm::ivec2(mouseMoveEvent.x, mouseMoveEvent.y);
                    const ScreenRayCaster screenRayCaster;
                    const glm::vec3 &worldSpaceRay = screenRayCaster.CastRayFromScreenSpaceToWorldSpace(screenSpacePosition,
                                                                                                        glm::ivec2(DisplayDeviceDataProvider::GetInstance()->GetWindowWidth() - 1,
                                                                                                                   DisplayDeviceDataProvider::GetInstance()->GetWindowHeight() - 1),
                                                                                                        projectionMatrix,
                                                                                                        sceneCameraSp->GetViewMatrix());
                    const glm::vec4 planeAtOrigin = glm::vec4(0, 1, 0, 0);
                    const float tParam = EngineMath::RaycastPlane(sceneCameraSp->GetEyeVector(), worldSpaceRay, planeAtOrigin);
                    if (tParam >= 0.0f)
                    {
                        const auto &rayIntersectionPosition = sceneCameraSp->GetEyeVector() + (worldSpaceRay * tParam);
                        if (eEditModeType::EDIT_TOWERS == mCurrentEditModeType)
                        {
                            const auto &nearestCellBoundingBox = mLevelPlacementGrid->GetNearestToPositionTowerCellBoundingBox(glm::vec2(rayIntersectionPosition.x, rayIntersectionPosition.z));
                            const auto pickerCellHalfSize = mLevelPlacementGrid->GetGridCellSizeForTower() * 0.5f;
                            mTowerPlacementPickerActor->GetRootComponent()->SetTranslation(
                                glm::vec3(nearestCellBoundingBox.GetOrigin().x, 0.0f, nearestCellBoundingBox.GetOrigin().y) - glm::vec3(pickerCellHalfSize, 0.0f, -pickerCellHalfSize));
                        }
                        else
                        {
                            const auto &nearestRouteNodePosition = mLevelPlacementGrid->GetNearestToPositionRouteEdgeNode(glm::vec2(rayIntersectionPosition.x, rayIntersectionPosition.z));
                            mRouteNodePickerActor->GetRootComponent()->SetTranslation(glm::vec3(nearestRouteNodePosition.x, 0.0f, nearestRouteNodePosition.y));
                        }
                    }
                }

                if (eEditModeType::EDIT_ROUTES == mCurrentEditModeType)
                {
                    if (KeyState::PRESSED == mouseBindings->GetKeyState(EngineCore::eMouseKeys::MouseButtonLeft))
                    {
                        if (!leftButtonPressed)
                        {
                            leftButtonPressed = true;
                            const auto &mouseMoveEvent = mouseBindings->GetLastMouseCursorPosition();
                            const glm::ivec2 &screenSpacePosition = glm::ivec2(mouseMoveEvent.x, mouseMoveEvent.y);
                            const ScreenRayCaster screenRayCaster;
                            const glm::vec3 &worldSpaceRay = screenRayCaster.CastRayFromScreenSpaceToWorldSpace(screenSpacePosition,
                                                                                                                glm::ivec2(DisplayDeviceDataProvider::GetInstance()->GetWindowWidth() - 1,
                                                                                                                           DisplayDeviceDataProvider::GetInstance()->GetWindowHeight() - 1),
                                                                                                                projectionMatrix,
                                                                                                                sceneCameraSp->GetViewMatrix());
                            const glm::vec4 planeAtOrigin = glm::vec4(0, 1, 0, 0);
                            const float tParam = EngineMath::RaycastPlane(sceneCameraSp->GetEyeVector(), worldSpaceRay, planeAtOrigin);
                            if (tParam >= 0.0f)
                            {
                                const auto &rayIntersectionPosition = sceneCameraSp->GetEyeVector() + (worldSpaceRay * tParam);
                                const auto &nearestRouteNodePosition = mLevelPlacementGrid->GetNearestToPositionRouteEdgeNode(glm::vec2(rayIntersectionPosition.x, rayIntersectionPosition.z));
                                const auto &newControlPoint = glm::vec3(nearestRouteNodePosition.x, 0.0f, nearestRouteNodePosition.y);
                                if (bezierControlPointsList.size() || !mActiveCurveComponents.size())
                                {
                                    bezierControlPointsList.emplace_back(newControlPoint);
                                }
                                else
                                {
                                    const auto &previousLineEndPosition = mActiveCurveComponents.top()->GetLineEndWorldSpacePosition();
                                    if (EngineMath::CheckSimilarityVec3(previousLineEndPosition, newControlPoint))
                                    {
                                        bezierControlPointsList.emplace_back(newControlPoint);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        leftButtonPressed = false;
                    }
                }
            }
        }
    }

    void LevelEditorController::UnpausableTick(const float deltaTime)
    {
    }

    void LevelEditorController::ProcessEvent(const BroadcastGameThreadEvent::EventData_t &data)
    {
        const auto eventHeader = std::get<0>(data);

        if ("EditorLevelEvents" == eventHeader)
        {
            const auto &jsonObj = nlohmann::json::parse(std::get<1>(data));
            if (jsonObj.contains("action"))
            {
                const auto &doneAction = jsonObj["action"].get<std::string>();
                if ("undo" == doneAction)
                {
                    if (!mActiveCurveComponents.empty())
                    {
                        const auto &lastActiveCurveComponent = mActiveCurveComponents.top();
                        lastActiveCurveComponent->SetIsEnabled(false);
                        mIdleCurveComponents.emplace(lastActiveCurveComponent);
                        mActiveCurveComponents.pop();
                    }
                }
                else if ("save" == doneAction)
                {
                    std::string lvlName = "unknown";
                    if (jsonObj.contains("name"))
                    {
                        lvlName = jsonObj["name"].get<std::string>();
                    }

                    if (!mActiveCurveComponents.empty())
                    {
                        std::vector<std::tuple<glm::vec3 /*start*/, glm::vec3 /*control point*/, glm::vec3 /*end*/>> pathControlPoints;
                        pathControlPoints.reserve(mActiveCurveComponents.size() * 3);
                        auto curveComponentsCopy = mActiveCurveComponents;
                        while (!curveComponentsCopy.empty())
                        {
                            const auto &topPathComponent = curveComponentsCopy.top();
                            curveComponentsCopy.pop();
                            pathControlPoints.emplace_back(std::make_tuple<glm::vec3, glm::vec3, glm::vec3>(topPathComponent->GetLineBeginWorldSpacePosition(),
                                                                                                            topPathComponent->GetBezierControlPointWorldSpacePosition(),
                                                                                                            topPathComponent->GetLineEndWorldSpacePosition()));
                            std::rotate(pathControlPoints.rbegin(), pathControlPoints.rbegin() + 1, pathControlPoints.rend());
                        }

                        PathSerializationHelper pathSerialization;
                        const std::string &serializedPathJsonStr = pathSerialization.DumpRouteControlPointsToJsonString(pathControlPoints);
                        FileFacade fileFacade;
                        fileFacade.OpenAndReadOrCreateFile(lvlName);
                        fileFacade.RewriteSrc(serializedPathJsonStr);
                        fileFacade.WriteToFile();
                    }
                }
            }
        }
    }

    void LevelEditorController::ProcessEvent(const ChangeEditModeEvent::EventData_t &data)
    {
        const auto editModeType = std::get<0>(data);
        mCurrentEditModeType = editModeType;
        const bool isVisibleTowerPlacementGridActor = eEditModeType::EDIT_TOWERS == mCurrentEditModeType;
        const bool isVisibleRoutePlacementGridActor = eEditModeType::EDIT_ROUTES == mCurrentEditModeType;
        mTowerPlacementGridActor->SetIsEnabled(isVisibleTowerPlacementGridActor);
        mTowerPlacementPickerActor->SetIsEnabled(isVisibleTowerPlacementGridActor);
        mRoutePlacementGridActor->SetIsEnabled(isVisibleRoutePlacementGridActor);
        mRouteNodePickerActor->SetIsEnabled(isVisibleRoutePlacementGridActor);
    }

    void LevelEditorController::Initialize()
    {
        mLevelPlacementGrid = std::make_unique<LevelPlacementGrid>(mLevelAreaBoundingBox);
        InitializeRoutePlacementGrid();
        InitializeTowerPlacementGrid();
        ProcessEvent(std::make_tuple<eEditModeType>(eEditModeType::IDLE));
    }

    void LevelEditorController::InitializeRoutePlacementGrid()
    {
        const auto &sceneSp = mSceneWp.lock();
        assert(sceneSp);

        // Route node picker initialize

        const auto pickerSize = 2.0f;
        mRouteNodePickerActor = std::make_shared<Actor>("RouteNodePickerActor", std::make_shared<SceneComponent>("RouteNodePickerActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f)));
        sceneSp->AddActor(mRouteNodePickerActor);

        MaterialParser materialParser;
        const std::shared_ptr<IMaterial> &editorNodePickerMaterial = materialParser.ParseMaterialDescriptor("RouteNodeEditorMaterial.m");
        sceneSp->RegisterMaterialInstance(editorNodePickerMaterial);
        MaterialPropertySetter::SetMaterialPropertyValue(editorNodePickerMaterial, "opacity", 1.0f);
        MaterialPropertySetter::SetMaterialPropertyValue(editorNodePickerMaterial, "color", glm::vec3(1.0f, 0.0f, 0.0f));

        const auto &meshComponentCreator = std::make_shared<ForwardShadingMeshComponentCreator<ForwardShadingMeshComponent>>();
        const auto &d_mesh = std::make_shared<ForwardShadingMeshComponentData>("TowerPlacementMeshComponent", "sphere.obj", glm::vec3(), glm::vec3(), glm::vec3(pickerSize, 1.0f, pickerSize), editorNodePickerMaterial);
        const auto &c_mesh = std::static_pointer_cast<ForwardShadingMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
        c_mesh->SetSortOrderValue(1);
        mRouteNodePickerActor->AddComponent(c_mesh);

        // Route grid initialize

        mRoutePlacementGridActor = std::make_shared<Actor>("RoutePlacementGridActor", std::make_shared<SceneComponent>("RoutePlacementGridActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f)));
        sceneSp->AddActor(mRoutePlacementGridActor);
        const glm::ivec2 routeGridColumnsAndRowsCount = mLevelPlacementGrid->GetRouteGridColumnsAndRowsCount();
        const int32_t columnsLineCount = routeGridColumnsAndRowsCount.x + 1;
        const int32_t rowsLineCount = routeGridColumnsAndRowsCount.y + 1;

        const std::shared_ptr<IMaterial> &lineMaterial = materialParser.ParseMaterialDescriptor("AlbedoColorWithOpacityMaterial.m");
        sceneSp->RegisterMaterialInstance(lineMaterial);
        MaterialPropertySetter::SetMaterialPropertyValue(lineMaterial, "opacity", 1.0f);
        MaterialPropertySetter::SetMaterialPropertyValue(lineMaterial, "color", glm::vec3(1.0f));

        const auto &rtMeshComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedLineComponent>>();
        const auto &levelAreaBoundingBox = mLevelPlacementGrid->GetRouteLevelAreaBoundingBox();

        static constexpr float grid_elevation_bias = 1.0f;
        const auto gridCellSize = mLevelPlacementGrid->GetGridCellSizeForRoute();
        for (int32_t columnIdx = 0; columnIdx < columnsLineCount; ++columnIdx)
        {
            const auto &d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>(std::string("c_routeGridColumnLineMesh_" + std::to_string(columnIdx)), 4, glm::vec3(), glm::vec3(), glm::vec3(1), "", lineMaterial);
            const auto &c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(sceneSp->CreateComponent_GameThread(rtMeshComponentCreator, d_mesh));
            const auto &lineBegin = glm::vec3(levelAreaBoundingBox.GetMin().x + columnIdx * gridCellSize, -grid_elevation_bias, levelAreaBoundingBox.GetMin().y);
            const auto &lineEnd = glm::vec3(levelAreaBoundingBox.GetMin().x + columnIdx * gridCellSize, -grid_elevation_bias, levelAreaBoundingBox.GetMax().y);
            c_mesh->SetSortOrderValue(0);
            c_mesh->SetLineBeginWorldSpacePosition(lineBegin);
            c_mesh->SetLineEndWorldSpacePosition(lineEnd);
            mRoutePlacementGridActor->AddComponent(c_mesh);
        }

        for (int32_t rowIdx = 0; rowIdx < rowsLineCount; ++rowIdx)
        {
            const auto &d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>(std::string("c_routeGridRowLineMesh_" + std::to_string(rowIdx)), 4, glm::vec3(0), glm::vec3(), glm::vec3(1), "", lineMaterial);
            const auto &c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(sceneSp->CreateComponent_GameThread(rtMeshComponentCreator, d_mesh));
            const auto &lineBegin = glm::vec3(levelAreaBoundingBox.GetMin().x, -grid_elevation_bias * 1.5f, levelAreaBoundingBox.GetMin().y + rowIdx * gridCellSize);
            const auto &lineEnd = glm::vec3(levelAreaBoundingBox.GetMax().x, -grid_elevation_bias * 1.5f, levelAreaBoundingBox.GetMin().y + rowIdx * gridCellSize);
            c_mesh->SetSortOrderValue(0);
            c_mesh->SetLineBeginWorldSpacePosition(lineBegin);
            c_mesh->SetLineEndWorldSpacePosition(lineEnd);
            mRoutePlacementGridActor->AddComponent(c_mesh);
        }
    }

    void LevelEditorController::InitializeTowerPlacementGrid()
    {
        const auto &sceneSp = mSceneWp.lock();
        assert(sceneSp);

        // Tower grid picker initialize

        const auto pickerCellSize = mLevelPlacementGrid->GetGridCellSizeForTower();
        mTowerPlacementPickerActor = std::make_shared<Actor>("TowerPlacementPickerActor", std::make_shared<SceneComponent>("TowerPlacementPickerActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f)));
        sceneSp->AddActor(mTowerPlacementPickerActor);

        MaterialParser materialParser;
        const std::shared_ptr<IMaterial> &editorPickerMaterial = materialParser.ParseMaterialDescriptor("EditorPickerMaterial.m");
        sceneSp->RegisterMaterialInstance(editorPickerMaterial);
        MaterialPropertySetter::SetMaterialPropertyValue(editorPickerMaterial, "opacity", 1.0f);
        MaterialPropertySetter::SetMaterialPropertyValue(editorPickerMaterial, "color", glm::vec3(0.4f, 0.8f, 0.2f));
        MaterialPropertySetter::SetMaterialPropertyValue(editorPickerMaterial, sceneSp, "GT_DeltaSec", "gt_timeSec");

        const auto &meshComponentCreator = std::make_shared<ForwardShadingMeshComponentCreator<ForwardShadingMeshComponent>>();
        const auto &d_mesh = std::make_shared<ForwardShadingMeshComponentData>("TowerPlacementMeshComponent", "plane.obj", glm::vec3(), glm::vec3(), glm::vec3(pickerCellSize, 1.0f, pickerCellSize), editorPickerMaterial);
        const auto &c_mesh = std::static_pointer_cast<ForwardShadingMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
        c_mesh->SetSortOrderValue(1);
        mTowerPlacementPickerActor->AddComponent(c_mesh);

        // Tower grid initialize

        mTowerPlacementGridActor = std::make_shared<Actor>("TowerPlacementGridActor", std::make_shared<SceneComponent>("TowerPlacementGridActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f)));
        sceneSp->AddActor(mTowerPlacementGridActor);
        const glm::ivec2 towerGridColumnsAndRowsCount = mLevelPlacementGrid->GetTowerGridColumnsAndRowsCount();
        const int32_t columnsLineCount = towerGridColumnsAndRowsCount.x + 1;
        const int32_t rowsLineCount = towerGridColumnsAndRowsCount.y + 1;

        const std::shared_ptr<IMaterial> &lineMaterial = materialParser.ParseMaterialDescriptor("AlbedoColorWithOpacityMaterial.m");
        sceneSp->RegisterMaterialInstance(lineMaterial);
        MaterialPropertySetter::SetMaterialPropertyValue(lineMaterial, "opacity", 1.0f);
        MaterialPropertySetter::SetMaterialPropertyValue(lineMaterial, "color", glm::vec3(1.0f));

        const auto &rtMeshComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedLineComponent>>();
        const auto &levelAreaBoundingBox = mLevelPlacementGrid->GetTowerLevelAreaBoundingBox();

        const int32_t leftSideColumnsCount = columnsLineCount / 2;
        const int32_t rightSideColumnsCount = columnsLineCount - leftSideColumnsCount;
        static constexpr float grid_elevation_bias = 1.0f;
        const auto gridCellSize = mLevelPlacementGrid->GetGridCellSizeForTower();
        for (int32_t columnIdx = 0; columnIdx < columnsLineCount; ++columnIdx)
        {
            const auto &d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>(std::string("c_towerGridColumnLineMesh_" + std::to_string(columnIdx)), 4, glm::vec3(), glm::vec3(), glm::vec3(1), "", lineMaterial);
            const auto &c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(sceneSp->CreateComponent_GameThread(rtMeshComponentCreator, d_mesh));
            const auto &lineBegin = glm::vec3(levelAreaBoundingBox.GetMin().x + columnIdx * gridCellSize, -grid_elevation_bias, levelAreaBoundingBox.GetMin().y);
            const auto &lineEnd = glm::vec3(levelAreaBoundingBox.GetMin().x + columnIdx * gridCellSize, -grid_elevation_bias, levelAreaBoundingBox.GetMax().y);
            c_mesh->SetSortOrderValue(0);
            c_mesh->SetLineBeginWorldSpacePosition(lineBegin);
            c_mesh->SetLineEndWorldSpacePosition(lineEnd);
            mTowerPlacementGridActor->AddComponent(c_mesh);
        }

        for (int32_t rowIdx = 0; rowIdx < rowsLineCount; ++rowIdx)
        {
            const auto &d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>(std::string("c_towerGridRowLineMesh_" + std::to_string(rowIdx)), 4, glm::vec3(0), glm::vec3(), glm::vec3(1), "", lineMaterial);
            const auto &c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(sceneSp->CreateComponent_GameThread(rtMeshComponentCreator, d_mesh));
            const auto &lineBegin = glm::vec3(levelAreaBoundingBox.GetMin().x, -grid_elevation_bias * 1.5f, levelAreaBoundingBox.GetMin().y + rowIdx * gridCellSize);
            const auto &lineEnd = glm::vec3(levelAreaBoundingBox.GetMax().x, -grid_elevation_bias * 1.5f, levelAreaBoundingBox.GetMin().y + rowIdx * gridCellSize);
            c_mesh->SetSortOrderValue(0);
            c_mesh->SetLineBeginWorldSpacePosition(lineBegin);
            c_mesh->SetLineEndWorldSpacePosition(lineEnd);
            mTowerPlacementGridActor->AddComponent(c_mesh);
        }
    }
}
