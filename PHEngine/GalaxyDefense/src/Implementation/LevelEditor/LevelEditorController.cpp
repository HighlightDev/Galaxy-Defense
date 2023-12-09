#include "LevelEditorController.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game
{
    LevelEditorController::LevelEditorController(const std::weak_ptr<Scene> &sceneWp)
        : mSceneWp(sceneWp)
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
    }

    void LevelEditorController::UnpausableTick(const float deltaTime)
    {
    }

    void LevelEditorController::Initialize()
    {
        mLevelPlacementGrid = std::make_unique<LevelPlacementGrid>(mLevelAreaBoundingBox);

        const auto &sceneSp = mSceneWp.lock();
        assert(sceneSp);

        mLevelPlacementGridActor = std::make_shared<Actor>("LevelPlacementGridActor", std::make_shared<SceneComponent>("LevelPlacementGridActor_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1.0f)));
        sceneSp->AddActor(mLevelPlacementGridActor);
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
        for (int32_t columnIdx = -leftSideColumnsCount; columnIdx < rightSideColumnsCount; ++columnIdx)
        {
            const auto d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>("c_levelGridColumnLineMesh_" + columnIdx, 4, glm::vec3(), glm::vec3(), glm::vec3(1), "", lineMaterial);
            const auto &c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
            const auto &lineBegin = glm::vec3(columnIdx * mLevelPlacementGrid->GetGridCellSizeForTower(), 0.0f, levelAreaBoundingBox.GetMin().y);
            const auto &lineEnd = glm::vec3(columnIdx * mLevelPlacementGrid->GetGridCellSizeForTower(), 0.0f, levelAreaBoundingBox.GetMax().y);
            c_mesh->SetSortOrderValue(100);
            c_mesh->SetLineBeginWorldSpacePosition(lineBegin);
            c_mesh->SetLineEndWorldSpacePosition(lineEnd);
            mLevelPlacementGridActor->AddComponent(c_mesh);
        }

        const int32_t forwardSideRowsCount = rowsLineCount / 2;
        const int32_t nearSideRowsCount = rowsLineCount - forwardSideRowsCount;
        for (int32_t rowIdx = -forwardSideRowsCount; rowIdx < nearSideRowsCount; ++rowIdx)
        {
            const auto d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>("c_levelGridRowLineMesh_" + rowIdx, 4, glm::vec3(0), glm::vec3(), glm::vec3(1), "", lineMaterial);
            const auto &c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
            const auto &lineBegin = glm::vec3(levelAreaBoundingBox.GetMin().x, 0.0f, rowIdx * mLevelPlacementGrid->GetGridCellSizeForTower());
            const auto &lineEnd = glm::vec3(levelAreaBoundingBox.GetMax().x, 0.0f, rowIdx * mLevelPlacementGrid->GetGridCellSizeForTower());
            c_mesh->SetSortOrderValue(100);
            c_mesh->SetLineBeginWorldSpacePosition(lineBegin);
            c_mesh->SetLineEndWorldSpacePosition(lineEnd);
            mLevelPlacementGridActor->AddComponent(c_mesh);
        }
    }
}
