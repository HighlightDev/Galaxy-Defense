#include "NavigationController.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedQuadraticBezierCurveComponent.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/InputComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Implementation/Navigation/PathSegment.h"
#include "Implementation/Actors/SpaceshipActor.h"

#include <array>
#include <glm/vec3.hpp>

using namespace EngineCore;
using namespace Graphics;
using namespace Resources;

namespace Game
{
    NavigationController::NavigationController(const std::weak_ptr<::EngineCore::Scene> &sceneWp)
        : mSceneWp(sceneWp),
          mNavPathDummyActor(std::make_shared<Actor>("NavPathDummyActor",
                                                     std::make_shared<EngineCore::SceneComponent>("NavPathDummy_rootComponent",
                                                                                                  glm::vec3(),
                                                                                                  glm::vec3(),
                                                                                                  glm::vec3(1))))
    {
    }

    void NavigationController::Initialize()
    {
        const auto sceneSp = mSceneWp.lock();
        assert(sceneSp);
        PathSegment segment;
        segment.SetSubdivisionsCount(50);
        segment.SetControlPoints({{glm::vec3(-50, 0, 50), glm::vec3(-60, 0, 30), glm::vec3(-20, 0, 10)}});
        mNavPathBuilder.AppendPathSegmentToTheEnd(segment);
        segment.SetControlPoints({{glm::vec3(-20, 0, 10), glm::vec3(20, 0, -100), glm::vec3(40, 0, -50)}});
        mNavPathBuilder.AppendPathSegmentToTheEnd(segment);
        segment.SetControlPoints({{glm::vec3(40, 0, -50), glm::vec3(20, 0, 20), glm::vec3(50, 0, -30)}});
        mNavPathBuilder.AppendPathSegmentToTheEnd(segment);
        segment.SetControlPoints({{glm::vec3(50, 0, -30), glm::vec3(60, 0, -40), glm::vec3(100, 0, -100)}});
        mNavPathBuilder.AppendPathSegmentToTheEnd(segment);

        MaterialParser materialParser;
        const std::shared_ptr<IMaterial> &electro_material = materialParser.ParseMaterialDescriptor("ElectroCurveMaterial.m");
        sceneSp->RegisterMaterialInstance(electro_material);
        const auto noiseTex = TexturePool::GetInstance()->GetOrAllocateResource("perlin_noise.png");

        MaterialPropertySetter::SetMaterialPropertyValue(electro_material, "noise", noiseTex);
        MaterialPropertySetter::SetMaterialPropertyValue(electro_material, sceneSp, "GT_DeltaSec", "gt_timeSec");
        MaterialPropertySetter::SetMaterialPropertyValue(electro_material, "opacity", 1.0f);

        const auto pathSegments = mNavPathBuilder.GetPathSegments();
        for (int i = 0; i < pathSegments.size(); ++i)
        {
            const auto bezierControlPoints = pathSegments.at(i).GetQuadraticBezierControlPoints();
            auto d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>("c_bezierCurveLineMesh_" + std::to_string(i), 150, glm::vec3(0, 0, 0), glm::vec3(), glm::vec3(1), "", electro_material);
            const auto &meshComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedQuadraticBezierCurveComponent>>();
            auto c_mesh = std::static_pointer_cast<RuntimeGeneratedQuadraticBezierCurveComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
            c_mesh->SetLineWidth(10.0f);
            c_mesh->SetSortOrderValue(100);
            c_mesh->SetCurveSegmentsCount(50);
            c_mesh->SetLineBeginWorldSpacePosition(bezierControlPoints.at(0));
            c_mesh->SetBezierControlPointWorldSpacePosition(bezierControlPoints.at(1));
            c_mesh->SetLineEndWorldSpacePosition(bezierControlPoints.at(2));
            mNavPathDummyActor->AddComponent(c_mesh);
        }

        sceneSp->AddActor(mNavPathDummyActor);
    }

    void NavigationController::OnPreLevelInit()
    {
    }

    void NavigationController::OnLevelInit()
    {
        Initialize();
    }

    void NavigationController::OnPostLevelInit()
    {
    }

    void NavigationController::PostPlayLevelFinished()
    {
        for (const std::shared_ptr<SpaceshipActor> &enemy : mEnemies)
        {
            const auto enemyMovementComponent = enemy->GetOnRouteMovementComponent();
            assert(enemyMovementComponent);
            enemyMovementComponent->SetRoutePoints(mNavPathBuilder.GetRoutePoints());
            enemyMovementComponent->SetIsMovementAllowed(true);
            enemy->TriggerSpawn(mNavPathBuilder.GetRouteFirstPoint());
        }
    }

    void NavigationController::CleanUp()
    {
    }

    void NavigationController::Tick(const float deltaTime)
    {
    }

    void NavigationController::UnpausableTick(const float deltaTime)
    {
    }

    void NavigationController::SetEnemies(const std::vector<std::shared_ptr<SpaceshipActor>> &enemies)
    {
        mEnemies = enemies;
    }
}
