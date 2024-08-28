#include "NavigationController.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedQuadraticBezierCurveComponent.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/InputComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Implementation/Navigation/PathSegment.h"
#include "Implementation/Navigation/Path.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Actors/MissileActor.h"

#include <array>
#include <glm/vec3.hpp>

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

    void NavigationController::SetLevelBounds(const BoundingBox3D &levelBounds)
    {
        mLevelBounds = levelBounds;
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
        bool needToValidateMissiles = false;
        for (auto &missile : mMissiles)
        {
            if (!missile)
            {
                needToValidateMissiles = true;
            }
            else if (eMissileActivityState::IDLE != missile->GetMissileActivityState() &&
                     eMissileActivityState::OUT_OF_LEVEL != missile->GetMissileActivityState())
            {
                if (!missile->IsInsideLevel(mLevelBounds))
                {
                    LogInfo("NavigationController::Tick => missile ", missile->GetName(), " is out of level.");
                    missile->SetMissileActivityState(eMissileActivityState::OUT_OF_LEVEL);
                }
            }
        }

        if (needToValidateMissiles)
        {
            mMissiles.erase(std::remove_if(mMissiles.begin(), mMissiles.end(), [](const auto &missile)
                                           { return !missile; }));
        }

        for (const auto &spaceship : mEnemies)
        {
            const auto &routeMoveComp = spaceship->GetOnRouteMovementComponent();
            if (routeMoveComp->GetIsDistanceCompleted())
            {
                spaceship->SetSpaceshipActivityState(eSpaceshipActivityState::PENDING_DISABLE);
            }
        }
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
        enemyMovementComponent->ResetStates();
        enemyMovementComponent->SetIsMovementOnRouteAllowed(true);
        enemyMovementComponent->SetRoutePoints(spacePath.GetRoutePoints());
        spaceship->TriggerSpawn(spacePath.GetRouteFirstPoint());
        mEnemies.emplace_back(spaceship);
    }

    void NavigationController::PutMissileToNavigate(const std::shared_ptr<MissileActor> &missile)
    {
        assert(missile);
        assert(missile->GetMissileActivityState() == eMissileActivityState::ACTIVE);
        const bool missingMissile = std::none_of(mMissiles.cbegin(), mMissiles.cend(), [missile](const auto &missileSp)
                                                 { return missile->GetObjectId() == missileSp->GetObjectId(); });
        if (missingMissile)
        {
            mMissiles.emplace_back(missile);
        }
    }

    void NavigationController::RemoveSpaceshipFromRoute(const int32_t spaceshipActorId)
    {
        if (mEnemies.size())
        {
            mEnemies.erase(std::remove_if(mEnemies.begin(), mEnemies.end(), [spaceshipActorId](const auto &enemy)
                                          { return spaceshipActorId == enemy->GetObjectId(); }));
        }
    }

    void NavigationController::RemoveMissileFromNavigation(const int32_t missileActorId)
    {
        if (mMissiles.size())
        {
            mMissiles.erase(std::remove_if(mMissiles.begin(), mMissiles.end(), [missileActorId](const auto &missile)
                                           { return !missile || (missile && missileActorId == missile->GetObjectId()); }));
        }
    }
}
