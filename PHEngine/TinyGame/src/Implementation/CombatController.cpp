#include "CombatController.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Factories/WeakSpaceShipFactory.h"
#include "Factories/WeakBulletFactory.h"
#include "SpaceShipPlayerController.h"

#include <random>

using namespace Graphics;
using namespace EnginePhysics;

namespace Game
{

    float get_random(float min, float max)
    {
        static std::default_random_engine e;
        static std::uniform_real_distribution<> dis(min, max);
        return dis(e);
    }

    float mDeltaTime = 0.0f;
    float mDamageDeltaTime = 0.0f;
    bool bDamageReceived = false;

    CombatController::CombatController(const std::weak_ptr<Scene> &scene)
        : mScene(scene),
          mEnemies(),
          mLevelBounds(BoundingBox(glm::vec3(0), glm::vec3(50, 50, 100)))
    {
        MainPlayerActionEvent::GetInstance()->AddListener(this);
        PhysicsCollisionOccuredEvent::GetInstance()->AddListener(this);
    }

    CombatController::~CombatController()
    {
        MainPlayerActionEvent::GetInstance()->RemoveListener(this);
        PhysicsCollisionOccuredEvent::GetInstance()->RemoveListener(this);
    }

    void CombatController::PreInit()
    {
    }

    void CombatController::PostInit()
    {
        if (const auto &sceneSp = mScene.lock())
        {
            CreateWeaponBulletPool(4, sceneSp);

            WeakSpaceShipFactory spaceShipFactory;

            std::srand(std::time(nullptr));
            for (size_t i = 0; i < 2; i++)
            {
                static constexpr float x_axisHalfWidth = 50.0f;
                static constexpr float y_axisHalfHeight = 30.0f;
                const float x = get_random(0.5f, 10.0f);
                const float scale = glm::clamp(x, 0.5f, 3.0f);
                glm::vec3 startPosition(((x_axisHalfWidth / x) * 2) - x_axisHalfWidth,
                                        0, //((y_axisHalfHeight / x) * 2) - y_axisHalfHeight,
                                        80 + (i * i) + 2);

                const auto &a_enemyShip = spaceShipFactory.CreateSpaceShip(sceneSp,
                                                                           startPosition,
                                                                           glm::vec3(),
                                                                           glm::vec3(9));

                mEnemies.emplace_back(a_enemyShip);
            }
        }
    }

    void CombatController::PostPlayLevelFinished()
    {
        for (const auto &bulletPair : mWeaponBulletsPool)
        {
            if (const auto &bulletSp = bulletPair.first)
            {
                bulletSp->SetIsEnabled(false); // disable all bullets at level start
            }
        }
    }

    void CombatController::ProcessEvent(const typename MainPlayerActionEvent::EventData_t &data)
    {
        if (!bIsCoolDownInProgress)
        {
            bIsCoolDownInProgress = true;
            const auto &playerAction = std::get<0>(data);
            if (const auto &sceneSp = mScene.lock())
            {
                ShootBullet(mPlayerShip->GetRootComponent()->GetTranslation());
            }
        }
    }

    void CombatController::ProcessEvent(const typename PhysicsCollisionOccuredEvent::EventData_t &data)
    {
        const ePhysicsBodyType physBodyType = std::get<0>(data);
        const auto this_actor_id = std::get<2>(data);
        const auto that_actor_id = std::get<4>(data);

        if (ePhysicsBodyType::GHOST == physBodyType)
        {
            if (const auto &sceneSp = mScene.lock())
            {
                const auto &a_thisEnemyShip = FindEnemyShipById(this_actor_id);
                auto a_thisBulletIt = FindBulletById(this_actor_id);

                const auto &a_thatEnemyShip = FindEnemyShipById(that_actor_id);
                auto a_thatBulletIt = FindBulletById(that_actor_id);

                const std::shared_ptr<Actor> &a_enemyShip = a_thisEnemyShip ? a_thisEnemyShip : a_thatEnemyShip;
                auto a_bulletIt = a_thisBulletIt != mWeaponBulletsPool.end() ? a_thisBulletIt : a_thatBulletIt;

                if (a_enemyShip && a_bulletIt != mWeaponBulletsPool.end())
                {
                    a_bulletIt->first->SetIsEnabled(false);
                    a_bulletIt->second = eBulletState::IDLE;
                    bDamageReceived = true;
                }
            }
        }
    }

    void CombatController::SetPlayerActorController(const std::shared_ptr<SpaceShipPlayerController> &mainPlayerActorController)
    {
        assert(mainPlayerActorController);
        mMainPlayerActorController = mainPlayerActorController;
        mMainPlayerActorController->SetLevelBounds(mLevelBounds);
        mPlayerShip = mMainPlayerActorController->GetBindedActor();
        assert(mPlayerShip);
    }

    void CombatController::Tick(const float deltaTime)
    {
        // Test bullets if they are still inside level bounds
        FlushToPoolUsedBullets();

        if (bDamageReceived)
        {
            mDamageDeltaTime += deltaTime * 10;
            if (mDamageDeltaTime > 1.0f)
            {
                mDamageDeltaTime = 0.0f;
                bDamageReceived = false;
            }

            for (const auto &a_enemyShip : mEnemies)
            {
                const auto &materialDamageProperty =
                    std::static_pointer_cast<EngineGOProperty<float>>(
                        a_enemyShip->GetEnginePropertyByName("property_damageEffect"));
                materialDamageProperty->SetValue(mDamageDeltaTime);
            }
        }

        if (bIsCoolDownInProgress)
        {
            mDeltaTime += deltaTime;

            if (mDeltaTime > mCoolDownTime)
            {
                Logger::Out("CombatController::Tick. Cooldown!");
                mDeltaTime = 0.0f;
                bIsCoolDownInProgress = false;
            }
        }

        for (const auto &enemySp : mEnemies)
        {
            const auto &enemyTranslation = enemySp->GetRootComponent()->GetTranslation();
            if (glm::length(enemyTranslation) > 70.0f)
            {
                std::srand(std::time(nullptr));
                static constexpr float x_axisHalfWidth = 40.0f;
                static constexpr float y_axisHalfHeight = 20.0f;
                const float x = get_random(1.0f, 10.0f);
                glm::vec3 startPosition(((x_axisHalfWidth / x) * 2) - x_axisHalfWidth,
                                        0.0f, //((y_axisHalfHeight / x) * 2) - y_axisHalfHeight,
                                        60.0f);
                const auto &c_movement = enemySp->GetMovementComponent();
                c_movement->Teleport(startPosition);
            }
        }
    }

    void CombatController::CreateWeaponBulletPool(const size_t poolSize, const std::shared_ptr<Scene> &sceneSp)
    {
        WeakBulletFactory bulletFactory;
        for (size_t i = 0; i < poolSize; ++i)
        {
            const auto &a_shipBullet = bulletFactory.CreateWeaponBullet(sceneSp,
                                                          glm::vec3(0),
                                                          glm::vec3(),
                                                          glm::vec3(1.0));

            mWeaponBulletsPool.emplace_back(std::make_pair(a_shipBullet, eBulletState::IDLE));
        }
    }

    void CombatController::ShootBullet(const glm::vec3 &bulletStartPosition)
    {
        auto idleBulletIt = std::find_if(mWeaponBulletsPool.begin(), mWeaponBulletsPool.end(), [](const auto &bulletPair)
                                         { return bulletPair.second == eBulletState::IDLE; });

        if (idleBulletIt == mWeaponBulletsPool.end())
        {
            Logger::Out("CombatController::ShootBullet. Error - no idle bullets in the pool");
            return;
        }

        if (const auto &bulletSp = idleBulletIt->first)
        {
            bulletSp->SetIsEnabled(true);
            bulletSp->GetRootComponent()->SetTranslation(bulletStartPosition);
            idleBulletIt->second = eBulletState::ACTIVE;
            Logger::Out("CombatController::ShootBuller. Successfull shoot.");
        }
    }

    void CombatController::FlushToPoolUsedBullets()
    {
        for (auto &weaponPair : mWeaponBulletsPool)
        {
            if (auto weaponSP = weaponPair.first)
            {
                const auto &bulletPosition = weaponSP->GetRootComponent()->GetTranslation();
                const bool bBulletInsideLevel = EngineMath::TestPointInAABB(mLevelBounds.GetMin(), mLevelBounds.GetMax(), bulletPosition);

                if (!bBulletInsideLevel)
                {
                    weaponSP->SetIsEnabled(false);
                    weaponSP->GetRootComponent()->SetTranslation(glm::vec3(0));
                    weaponPair.second = eBulletState::IDLE;

                    Logger::Out("CombatController::FlushToPoolUsedBullets. Flush bullet to pool. Position = ",
                                bulletPosition, " . Actor name = ", weaponSP->GetName());
                }
            }
            else
            {
                Logger::Out("CombatController::FlushToPoolUsedBullets. Error: Bullet was destroyed!");
            }
        }
    }

    std::shared_ptr<Actor> CombatController::FindEnemyShipByName(const std::string &actorName) const
    {
        std::shared_ptr<Actor> result;

        auto foundIt = std::find_if(mEnemies.begin(), mEnemies.end(),
                                    [&actorName = static_cast<const std::string &>(actorName)](const auto &enemyActor)
                                    {
                                        return actorName == enemyActor->GetName();
                                    });

        if (foundIt != mEnemies.end())
        {
            result = *foundIt;
        }

        return result;
    }

    std::shared_ptr<Actor> CombatController::FindEnemyShipById(const uint64_t actorId) const
    {
        std::shared_ptr<Actor> result;

        auto foundIt = std::find_if(mEnemies.begin(), mEnemies.end(), [=](const auto &enemyActor)
                                    { return actorId == enemyActor->GetObjectId(); });

        if (foundIt != mEnemies.end())
        {
            result = *foundIt;
        }

        return result;
    }

    typename std::vector<std::pair<std::shared_ptr<Actor>, eBulletState>>::iterator
    CombatController::FindBulletByName(const std::string &actorName)
    {
        typename std::vector<std::pair<std::shared_ptr<Actor>, eBulletState>>::iterator foundIt = std::find_if(mWeaponBulletsPool.begin(),
                                                                                                               mWeaponBulletsPool.end(),
                                                                                                               [&actorName = static_cast<const std::string &>(actorName)](const auto &bulletPair)
                                                                                                               {
                                                                                                                   return actorName == bulletPair.first->GetName();
                                                                                                               });
        return foundIt;
    }

    typename std::vector<std::pair<std::shared_ptr<Actor>, eBulletState>>::iterator
    CombatController::FindBulletById(const uint64_t actorId)
    {
        typename std::vector<std::pair<std::shared_ptr<Actor>, eBulletState>>::iterator foundIt = std::find_if(mWeaponBulletsPool.begin(),
                                                                                                               mWeaponBulletsPool.end(),
                                                                                                               [=](const auto &bulletPair)
                                                                                                               {
                                                                                                                   return actorId == bulletPair.first->GetObjectId();
                                                                                                               });
        return foundIt;
    }
}