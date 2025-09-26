#include "PortalActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/Random.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Controllers/NavigationController.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

namespace Game {
PortalActor::PortalActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent)
    : Actor(gameObjectName, rootComponent)
{
}

void PortalActor::SetCombatActorsPoolsHandler(const std::weak_ptr<CombatActorsPoolHandler>& poolHandlerWp)
{
    mCombatActorsPoolHandlerWp = poolHandlerWp;
}

void PortalActor::SetNavigationController(const std::weak_ptr<NavigationController>& navigationControllerWp)
{
    mNavigationControllerWp = navigationControllerWp;
}

void PortalActor::Tick(const float deltaTime)
{
    Actor::Tick(deltaTime);

    const auto poolSp = mCombatActorsPoolHandlerWp.lock();
    const auto navController = mNavigationControllerWp.lock();
    if (poolSp && navController) {
        if (mIsSpawnActive && mPathNames.size() > 0) {
            mAccumulatedDeltaTime += deltaTime;
            if (mAccumulatedDeltaTime >= mSpawnInterval) {
                if (const auto& freeShip = poolSp->GetFreeSpaceshipActor()) {
                    const auto randomIndex
                        = static_cast<int32_t>(std::round(Random::Float() * static_cast<float>(mPathNames.size() - 1)));
                    navController->PutSpaceshipOnRoute(mPathNames[randomIndex], freeShip);
                }
                mAccumulatedDeltaTime = std::fmod(mAccumulatedDeltaTime, mSpawnInterval);
            }
        }
    }
}

void PortalActor::SetupSpaceshipSpawn(const std::string& pathName, const size_t spawnIntervalMilliseconds)
{
    mPathNames.emplace_back(pathName);
    constexpr float c_millisecondToSecondsMul = 0.001F;
    mSpawnInterval = static_cast<float>(spawnIntervalMilliseconds) * c_millisecondToSecondsMul;
}

void PortalActor::StopSpawn()
{
    mIsSpawnActive = false;
}

void PortalActor::StartSpawn()
{
    mIsSpawnActive = true;
}

bool PortalActor::IsSpawnActive() const
{
    return mIsSpawnActive;
}

} // namespace Game