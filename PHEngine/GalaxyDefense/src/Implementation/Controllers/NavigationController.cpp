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
#include "Implementation/Navigation/Path.h"
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
        sceneSp->AddActor(mNavPathDummyActor);
    }

    void NavigationController::InitializePathDebugRendering()
    {
        const auto sceneSp = mSceneWp.lock();
        assert(sceneSp);
        MaterialParser materialParser;
        const std::shared_ptr<IMaterial> &splineMaterial = materialParser.ParseMaterialDescriptor("CurveLineMaterial.m");
        sceneSp->RegisterMaterialInstance(splineMaterial);

        MaterialPropertySetter::SetMaterialPropertyValue(splineMaterial, "opacity", 1.0f);
        MaterialPropertySetter::SetMaterialPropertyValue(splineMaterial, "color", glm::vec3(0.5f, 0.7f, 0.2f));

        const auto &paths = mNavPathBuilder.GetPaths();
        for (const auto &[pathName, path] : paths)
        {
            const auto pathSegments = path.GetPathSegments();
            for (int i = 0; i < pathSegments.size(); ++i)
            {
                const auto bezierControlPoints = pathSegments.at(i).GetQuadraticBezierControlPoints();
                auto d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>("c_bezierCurveLineMesh_" + pathName + "_" + std::to_string(i),
                                                                                  150,
                                                                                  glm::vec3(), glm::vec3(), glm::vec3(1),
                                                                                  "",
                                                                                  splineMaterial);
                const auto &meshComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedQuadraticBezierCurveComponent>>();
                auto c_mesh = std::static_pointer_cast<RuntimeGeneratedQuadraticBezierCurveComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
                c_mesh->SetLineWidth(0.75f);
                c_mesh->SetSortOrderValue(100);
                c_mesh->SetCurveSegmentsCount(50);
                c_mesh->SetLineBeginWorldSpacePosition(bezierControlPoints.at(0));
                c_mesh->SetBezierControlPointWorldSpacePosition(bezierControlPoints.at(1));
                c_mesh->SetLineEndWorldSpacePosition(bezierControlPoints.at(2));
                mNavPathDummyActor->AddComponent(c_mesh);
            }
        }
    }

    void NavigationController::SetPathRoutes(const std::unordered_map<std::string, Path> &paths)
    {
        for (const auto &[pathName, pathSegment] : paths)
        {
            mNavPathBuilder.AddPath(pathName, pathSegment);
        }
    }

    void NavigationController::OnPreLevelInit()
    {
    }

    void NavigationController::OnLevelInit()
    {
        assert(mNavPathBuilder.GetPaths().size());
        Initialize();
        InitializePathDebugRendering(); // for debug visualisation purpose
    }

    void NavigationController::OnPostLevelInit()
    {
    }

    void NavigationController::PostPlayLevelFinished()
    {
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

    std::vector<std::string> NavigationController::GetPathNames() const
    {
        const auto &spacePaths = mNavPathBuilder.GetPaths();
        std::vector<std::string> pathNames;
        pathNames.reserve(spacePaths.size());
        std::transform(spacePaths.cbegin(), spacePaths.cend(), std::back_inserter(pathNames), [](const auto &spacePathPair) -> std::string
                       { return spacePathPair.first; });
        return pathNames;
    }

    void NavigationController::PutSpaceshipOnRoute(const std::string &routeName, const std::shared_ptr<SpaceshipActor> &spaceship)
    {
        auto &spacePaths = mNavPathBuilder.GetPaths();
        assert(spacePaths.count(routeName));
        auto &spacePath = spacePaths[routeName];

        const auto enemyMovementComponent = spaceship->GetOnRouteMovementComponent();
        assert(enemyMovementComponent);
        enemyMovementComponent->SetRoutePoints(spacePath.GetRoutePoints());
        enemyMovementComponent->SetIsMovementAllowed(true);
        spaceship->TriggerSpawn(spacePath.GetRouteFirstPoint());
    }
}
