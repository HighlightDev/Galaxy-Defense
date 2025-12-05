#include "NavigationController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentCreators/InputComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedQuadraticBezierCurveComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/DataProviders/GameConstants.h"
#include "Implementation/DataProviders/LevelDataProvider.h"
#include "Implementation/Navigation/Path.h"
#include "Implementation/Navigation/PathSegment.h"

#include <glm/vec3.hpp>

#include <array>

using namespace Graphics;
using namespace Resources;

namespace Game {
NavigationController::NavigationController(const std::weak_ptr<::EngineCore::Scene>& sceneWp)
    : mSceneWp(sceneWp)
    , mNavPathDummyActor(std::make_shared<Actor>(
          "NavPathDummyActor",
          std::make_shared<EngineCore::SceneComponent>("NavPathDummy_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1))))
{
}

void NavigationController::Initialize()
{
    const auto sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in NavigationController::Initialize");
    sceneSp->AddActor(mNavPathDummyActor);
}

void NavigationController::InitializePathDebugRendering()
{
    const auto sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in NavigationController::InitializePathDebugRendering");
    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& splineMaterial = materialParser.ParseMaterialDescriptor("CurveLineMaterial.m");
    sceneSp->RegisterMaterialInstance(splineMaterial);

    MaterialPropertySetter::SetMaterialPropertyValue(splineMaterial, "opacity", 1.0f);
    MaterialPropertySetter::SetMaterialPropertyValue(splineMaterial, "color", glm::vec3(0.5f, 0.7f, 0.2f));

    const auto& paths = mNavPathBuilder.GetPaths();
    const auto& extendedPaths = mNavPathBuilder.GetExtendedPaths();
    std::unordered_map<std::string, Path> allPaths;
    std::transform(paths.cbegin(), paths.cend(), std::inserter(allPaths, allPaths.end()), [](const auto& pathPair) {
        return std::make_pair(pathPair.first, pathPair.second);
    });
    std::transform(
        extendedPaths.cbegin(), extendedPaths.cend(), std::inserter(allPaths, allPaths.end()), [](const auto& extendedPathPair) {
            return std::make_pair(extendedPathPair.second.first, extendedPathPair.second.second);
        });

    for (const auto& [pathName, path] : allPaths) {
        const auto pathSegments = path.GetPathSegments();
        for (int i = 0; i < pathSegments.size(); ++i) {
            const auto bezierControlPoints = pathSegments.at(i).GetQuadraticBezierControlPoints();
            auto d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>(
                "c_bezierCurveLineMesh_" + pathName + "_" + std::to_string(i),
                150,
                glm::vec3(),
                glm::vec3(),
                glm::vec3(1),
                "",
                splineMaterial);
            const auto& meshComponentCreator
                = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedQuadraticBezierCurveComponent>>();
            auto c_mesh = std::static_pointer_cast<RuntimeGeneratedQuadraticBezierCurveComponent>(
                sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
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

void NavigationController::SetPathRoutes(const std::unordered_map<std::string, Path>& paths)
{
    for (const auto& [pathName, pathSegment] : paths) {
        mNavPathBuilder.AddPath(pathName, pathSegment);
        mNavPathBuilder.ExtendPath(pathName, Game::Constants::c_extraPathPerSideCount);
    }
}

void NavigationController::SetLevelBounds(const BoundingBox3D& levelBounds)
{
    mLevelBounds = levelBounds;
}

void NavigationController::OnPreLevelInit()
{
}

void NavigationController::OnLevelInit()
{
    ext_assert(mNavPathBuilder.GetPaths().size(), " No path routes found in NavigationController::OnLevelInit");
    Initialize();
    if (cEnableDebugPathRendering) { // todo: make it runtime configurable
        InitializePathDebugRendering(); // for debug visualisation purpose
    }
}

void NavigationController::OnPostLevelInit()
{
}

void NavigationController::PostPlayLevelFinished()
{
}

void NavigationController::CleanUp()
{
    mEnemies.clear();
    mMissiles.clear();
}

void NavigationController::Tick(const float deltaTimeSec)
{
    bool needToValidateMissiles = false;
    for (auto& missile : mMissiles) {
        if (!missile) {
            needToValidateMissiles = true;
        } else if (
            eMissileActivityState::IDLE != missile->GetMissileActivityState()
            && eMissileActivityState::OUT_OF_LEVEL != missile->GetMissileActivityState()) {
            if (!missile->IsInsideLevel(mLevelBounds)) {
                LogInfo("NavigationController::Tick: missile ", missile->GetName(), " is out of level.");
                missile->SetMissileActivityState(eMissileActivityState::OUT_OF_LEVEL);
            }
        }
    }

    if (needToValidateMissiles) {
        mMissiles.erase(std::remove_if(mMissiles.begin(), mMissiles.end(), [](const auto& missile) { return !missile; }));
    }

    for (const auto& spaceship : mEnemies) {
        const auto& routeMoveComp = spaceship->GetOnRouteMovementComponent();
        if (routeMoveComp->GetIsDistanceCompleted()) {
            const auto& levelDataProviderPtr = LevelDataProvider::GetInstance();
            levelDataProviderPtr->SetCurrentStageSurvivedEnemySpaceshipsCount(
                levelDataProviderPtr->GetCurrentStageSurvivedEnemySpaceshipsCount() + 1);
            spaceship->SetSpaceshipActivityState(eSpaceshipActivityState::PENDING_DISABLE);
        }
    }
}

void NavigationController::UnpausableTick(const float deltaTimeSec)
{
}

std::vector<std::string> NavigationController::GetPathNames() const
{
    const auto& spacePaths = mNavPathBuilder.GetPaths();
    std::vector<std::string> pathNames;
    pathNames.reserve(spacePaths.size());
    std::transform(
        spacePaths.cbegin(), spacePaths.cend(), std::back_inserter(pathNames), [](const auto& spacePathPair) -> std::string {
            return spacePathPair.first;
        });
    return pathNames;
}

const Path& NavigationController::GetPath(const std::string& pathName) const
{
    const auto& spacePaths = mNavPathBuilder.GetPaths();
    ext_assert(spacePaths.count(pathName), "Path not found: " + pathName + " in NavigationController::GetPath");
    return spacePaths.at(pathName);
}

const std::unordered_multimap<std::string, std::pair<std::string, Path>>& NavigationController::GetExtendedPaths() const
{
    return mNavPathBuilder.GetExtendedPaths();
}

void NavigationController::PutSpaceshipOnRoute(const std::string& routeName, const std::shared_ptr<SpaceshipActor>& spaceship)
{
    const auto& spacePaths = mNavPathBuilder.GetPaths();
    const auto& extendedSpacePaths = mNavPathBuilder.GetExtendedPaths();
    std::optional<Path> spacePath;
    if (spacePaths.count(routeName)) {
        spacePath = spacePaths.at(routeName);
    } else {
        for (const auto& [originalPathName, extendedPathPair] : extendedSpacePaths) {
            if (extendedPathPair.first == routeName) {
                spacePath = extendedPathPair.second;
                break;
            }
        }
    }

    ext_assert(spacePath.has_value(), "Route not found: " + routeName + " in NavigationController::PutSpaceshipOnRoute");
    const auto enemyMovementComponent = spaceship->GetOnRouteMovementComponent();
    ext_assert(enemyMovementComponent, "Enemy movement component is null");
    enemyMovementComponent->ResetStates();
    enemyMovementComponent->SetIsMovementOnRouteAllowed(true);
    enemyMovementComponent->SetRoutePoints(spacePath->GetRoutePoints());
    spaceship->TriggerSpawn(spacePath->GetRouteFirstPoint());
    mEnemies.emplace_back(spaceship);
}

void NavigationController::PutMissileToNavigate(const std::shared_ptr<MissileActor>& missile)
{
    ext_assert(missile, "Missile pointer is null in PutMissileToNavigate");
    ext_assert(
        missile->GetMissileActivityState() == eMissileActivityState::ACTIVE, "Missile is not active in PutMissileToNavigate");
    const bool missingMissile = std::none_of(mMissiles.cbegin(), mMissiles.cend(), [missile](const auto& missileSp) {
        return missile->GetObjectId() == missileSp->GetObjectId();
    });
    if (missingMissile) {
        mMissiles.emplace_back(missile);
    }
}

void NavigationController::RemoveSpaceshipFromRoute(const int32_t spaceshipActorId)
{
    if (mEnemies.size()) {
        mEnemies.erase(std::remove_if(mEnemies.begin(), mEnemies.end(), [spaceshipActorId](const auto& enemy) {
            return spaceshipActorId == enemy->GetObjectId();
        }));
    }
}

void NavigationController::RemoveMissileFromNavigation(const int32_t missileActorId)
{
    if (mMissiles.size()) {
        auto removeIt = std::remove_if(mMissiles.begin(), mMissiles.end(), [missileActorId](const auto& missile) {
            return !missile || (missile && missileActorId == missile->GetObjectId());
        });
        if (removeIt != mMissiles.end()) {
            mMissiles.erase(removeIt, mMissiles.end());
        }
    }
}
} // namespace Game
