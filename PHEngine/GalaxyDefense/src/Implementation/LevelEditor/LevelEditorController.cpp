#include "LevelEditorController.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ForwardShadingMeshComponentCreator.h"
#include "Core/GameCore/Components/PrimitiveComponents/ForwardShadingMeshComponent.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/UtilityCore/ScreenRayCaster.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;
using namespace IO;

namespace Game
{
    LevelEditorController::LevelEditorController(const std::weak_ptr<Scene> &sceneWp)
        : mSceneWp(sceneWp),
          mInputComponent(std::make_shared<InputComponent>(std::make_shared<ComponentData>("LevelEditorController_InputComponent")))
    {
    }

    void LevelEditorController::OnPreLevelInit()
    {
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

    void LevelEditorController::Tick(const float deltaTime)
    {
        if (const auto &sceneCameraSp = mMainSceneCamera.lock())
        {
            const auto viewPerspectiveInfo = sceneCameraSp->GetViewPerspectiveInfo();
            const auto projectionMatrix = glm::perspective<float>(viewPerspectiveInfo.FoV,
                                                                  viewPerspectiveInfo.AspectRatio,
                                                                  viewPerspectiveInfo.NearPlane,
                                                                  viewPerspectiveInfo.FarPlane);
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
                    const auto &placementPosition = sceneCameraSp->GetEyeVector() + (worldSpaceRay * tParam);
                    const auto &nearestCellBoundingBox = mLevelPlacementGrid->GetNearestToPositionTowerCellBoundingBox(glm::vec2(placementPosition.x, placementPosition.z));
                    const auto pickerCellHalfSize = mLevelPlacementGrid->GetGridCellSizeForTower() * 0.5f;
                    mTowerPlacementPickerActor->GetRootComponent()->SetTranslation(
                        glm::vec3(nearestCellBoundingBox.GetOrigin().x, 0.0f, nearestCellBoundingBox.GetOrigin().y) - glm::vec3(pickerCellHalfSize, 0.0f, -pickerCellHalfSize));
                }
            }
        }
    }

    void LevelEditorController::UnpausableTick(const float deltaTime)
    {
    }

    void LevelEditorController::Initialize()
    {
        mLevelPlacementGrid = std::make_unique<LevelPlacementGrid>(mLevelAreaBoundingBox);
        InitializeRoutePlacementGrid();
        InitializeTowerPlacementGrid();
    }

    void LevelEditorController::InitializeRoutePlacementGrid()
    {
        const auto &sceneSp = mSceneWp.lock();
        assert(sceneSp);

        mRoutePlacementGridActor = std::make_shared<Actor>("RoutePlacementGridActor", std::make_shared<SceneComponent>("RoutePlacementGridActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f)));
        sceneSp->AddActor(mRoutePlacementGridActor);
        const glm::ivec2 towerGridColumnsAndRowsCount = mLevelPlacementGrid->GetTowerGridColumnsAndRowsCount();
        const int32_t columnsLineCount = towerGridColumnsAndRowsCount.x + 1;
        const int32_t rowsLineCount = towerGridColumnsAndRowsCount.y + 1;

        MaterialParser materialParser;
        const std::shared_ptr<IMaterial> &lineMaterial = materialParser.ParseMaterialDescriptor("AlbedoColorWithOpacityMaterial.m");
        sceneSp->RegisterMaterialInstance(lineMaterial);
        MaterialPropertySetter::SetMaterialPropertyValue(lineMaterial, "opacity", 1.0f);
        MaterialPropertySetter::SetMaterialPropertyValue(lineMaterial, "color", glm::vec3(1.0f));

        const auto &meshComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedLineComponent>>();
        const auto &levelAreaBoundingBox = mLevelPlacementGrid->GetLevelAreaBoundingBox();

        const int32_t leftSideColumnsCount = columnsLineCount / 2;
        const int32_t rightSideColumnsCount = columnsLineCount - leftSideColumnsCount;
        static constexpr float grid_elevation_bias = 2.0f;
        const auto gridCellSize = mLevelPlacementGrid->GetGridCellSizeForTower();
        for (int32_t columnIdx = -leftSideColumnsCount; columnIdx < rightSideColumnsCount; ++columnIdx)
        {
            const auto d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>("c_levelGridColumnLineMesh_" + columnIdx, 4, glm::vec3(), glm::vec3(), glm::vec3(1), "", lineMaterial);
            const auto &c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
            const auto &lineBegin = glm::vec3(columnIdx * gridCellSize, -grid_elevation_bias, levelAreaBoundingBox.GetMin().y);
            const auto &lineEnd = glm::vec3(columnIdx * gridCellSize, -grid_elevation_bias, levelAreaBoundingBox.GetMax().y);
            c_mesh->SetSortOrderValue(50 + columnIdx);
            c_mesh->SetLineBeginWorldSpacePosition(lineBegin);
            c_mesh->SetLineEndWorldSpacePosition(lineEnd);
            mRoutePlacementGridActor->AddComponent(c_mesh);
        }

        const int32_t forwardSideRowsCount = rowsLineCount / 2;
        const int32_t nearSideRowsCount = rowsLineCount - forwardSideRowsCount;
        for (int32_t rowIdx = -forwardSideRowsCount; rowIdx < nearSideRowsCount; ++rowIdx)
        {
            const auto &d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>("c_levelGridRowLineMesh_" + rowIdx, 4, glm::vec3(0), glm::vec3(), glm::vec3(1), "", lineMaterial);
            const auto &c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
            const auto &lineBegin = glm::vec3(levelAreaBoundingBox.GetMin().x, -grid_elevation_bias, rowIdx * gridCellSize);
            const auto &lineEnd = glm::vec3(levelAreaBoundingBox.GetMax().x, -grid_elevation_bias, rowIdx * gridCellSize);
            c_mesh->SetSortOrderValue(100 + rowIdx);
            c_mesh->SetLineBeginWorldSpacePosition(lineBegin);
            c_mesh->SetLineEndWorldSpacePosition(lineEnd);
            mRoutePlacementGridActor->AddComponent(c_mesh);
        }
    }

    void LevelEditorController::InitializeTowerPlacementGrid()
    {
        const auto &sceneSp = mSceneWp.lock();
        assert(sceneSp);

        const auto pickerCellSize = mLevelPlacementGrid->GetGridCellSizeForTower();
        mTowerPlacementPickerActor = std::make_shared<Actor>("TowerPlacementPickerActor", std::make_shared<SceneComponent>("TowerPlacementPickerActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f)));
        sceneSp->AddActor(mTowerPlacementPickerActor);

        MaterialParser materialParser;
        const std::shared_ptr<IMaterial> &editorPickerMaterial = materialParser.ParseMaterialDescriptor("EditorPickerMaterial.m");
        sceneSp->RegisterMaterialInstance(editorPickerMaterial);
        MaterialPropertySetter::SetMaterialPropertyValue(editorPickerMaterial, "opacity", 1.0f);
        MaterialPropertySetter::SetMaterialPropertyValue(editorPickerMaterial, "color", glm::vec3(0.4f, 0.8f, 0.2f));

        const auto &meshComponentCreator = std::make_shared<ForwardShadingMeshComponentCreator<ForwardShadingMeshComponent>>();
        const auto &d_mesh = std::make_shared<ForwardShadingMeshComponentData>("TowerPlacementMeshComponent", "plane.obj", glm::vec3(), glm::vec3(), glm::vec3(pickerCellSize, 1.0f, pickerCellSize), editorPickerMaterial);
        const auto &c_mesh = std::static_pointer_cast<ForwardShadingMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
        c_mesh->SetSortOrderValue(200);
        mTowerPlacementPickerActor->AddComponent(c_mesh);
    }
}
