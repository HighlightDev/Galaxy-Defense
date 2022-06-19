#include "CombatController.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Factories/WeakSpaceShipFactory.h"
#include "Factories/WeakBulletFactory.h"
#include "SpaceShipPlayerController.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/Components/ComponentCreators/UiComponentCreator.h"
#include "Core/GameCore/Components/UiComponents/UiComponent.h"
#include "Core/GameCore/Scene.h"

using namespace Graphics;
using namespace EnginePhysics;
using namespace EngineCore;

namespace Game
{
    float mDeltaTime = 0.0f;

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

    int counter = 0;

    size_t textField1, textField2, textField3;
    std::shared_ptr<UiComponent> uiComp;

    void CombatController::PostInit()
    {
        if (const auto &sceneSp = mScene.lock())
        {
            CreateWeaponBulletPool(4, sceneSp);

            WeakSpaceShipFactory spaceShipFactory;

            for (size_t i = 0; i < 2; ++i)
            {
                static constexpr float x_axisHalfWidth = 20.0f;
                static constexpr float y_axisHalfHeight = 20.0f;
                const float x = Random::Float() * 10.0f;
                glm::vec3 startPosition(((x_axisHalfWidth / x) * 2) - x_axisHalfWidth,
                                        0.0f,
                                        30.0f);

                const auto &a_enemyShip = spaceShipFactory.CreateSpaceShip(sceneSp,
                                                                           startPosition,
                                                                           glm::vec3(),
                                                                           glm::vec3(9));

                mEnemies.emplace_back(CombatEntity(a_enemyShip));
            }

            const auto &uiComponentCreator = std::make_shared<UiComponentCreator<UiComponent>>();
            const auto &c_uiComponent = std::static_pointer_cast<UiComponent>(sceneSp->CreateComponent_GameThread(uiComponentCreator, ComponentData("UiComponent1")));
            mPlayerShip->AddComponent(c_uiComponent);
            textField1 = c_uiComponent->CreateTextField("arial", 5, "Hello world!", glm::vec3(1, 0.5, 0.5), glm::vec2(0.5), 0.5, 5, false);
            textField2 = c_uiComponent->CreateTextField("arial", 5, "Hi!", glm::vec3(0.1, 0.1, 0.5), glm::vec2(0.2), 0.5, 5, false);
            textField3 = c_uiComponent->CreateTextField("arial", 5, "Privet pipka", glm::vec3(1), glm::vec2(), 0.5, 5, false);

            uiComp = c_uiComponent;
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
        const auto &playerAction = std::get<0>(data);

        if (eMainPlayerActionEnum::SHOOT == playerAction && !bIsCoolDownInProgress)
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
                const auto &a_thisEnemyShipIt = FindEnemyShipById(this_actor_id);
                auto a_thisBulletIt = FindBulletById(this_actor_id);

                const auto &a_thatEnemyShipIt = FindEnemyShipById(that_actor_id);
                auto a_thatBulletIt = FindBulletById(that_actor_id);

                auto a_enemyShipIt = a_thisEnemyShipIt != mEnemies.end() ? a_thisEnemyShipIt : a_thatEnemyShipIt;
                auto a_bulletIt = a_thisBulletIt != mWeaponBulletsPool.end() ? a_thisBulletIt : a_thatBulletIt;

                if (a_enemyShipIt != mEnemies.end() &&
                    a_bulletIt != mWeaponBulletsPool.end())
                {
                    a_bulletIt->first->SetIsEnabled(false);
                    a_bulletIt->second = eBulletState::IDLE;
                    if (a_enemyShipIt->CheckIsAliveAfterDamage(1))
                    {
                        a_enemyShipIt->SetIsDamageReceived(true);

                        auto c_particle = a_enemyShipIt->GetSpaceShipActor()->GetComponentsByType<ParticleSystemComponent>().back();
                        c_particle->EmitParticles();
                    }
                    else
                    {
                        static constexpr float x_axisHalfWidth = 20.0f;
                        static constexpr float y_axisHalfHeight = 20.0f;
                        const float x = Random::Float() * 10.0f;
                        glm::vec3 startPosition(((x_axisHalfWidth / x) * 2) - x_axisHalfWidth, 0.0f, 70.0f);
                        ReSpawnEnemyShip(*a_enemyShipIt, startPosition);
                    }
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

        for (auto &enemyContainer : mEnemies)
        {
            if (enemyContainer.GetIsDamageReceived())
            {
                float dmgTime = enemyContainer.GetDamageDeltaTime();
                dmgTime += deltaTime * 10.0f;
                if (dmgTime > 1.0f)
                {
                    dmgTime = 0.0f;
                    enemyContainer.SetIsDamageReceived(false);
                }
                enemyContainer.SetDamageDeltaTime(dmgTime);

                const auto &materialDamageProperty =
                    std::static_pointer_cast<EngineGOProperty<float>>(
                        enemyContainer.GetSpaceShipActor()->GetEnginePropertyByName("property_damageEffect"));
                materialDamageProperty->SetValue(dmgTime);
            }
        }

        if (bIsCoolDownInProgress)
        {
            mDeltaTime += deltaTime;

            if (mDeltaTime > mCoolDownTime)
            {
                Logger::Out("CombatController::Tick => Cooldown!");
                mDeltaTime = 0.0f;
                bIsCoolDownInProgress = false;
            }
        }

        for (auto &enemyContainer : mEnemies)
        {
            const auto &enemyTranslation = enemyContainer.GetSpaceShipActor()->GetRootComponent()->GetTranslation();
            if (enemyTranslation.z < -10.0f)
            {
                static constexpr float x_axisHalfWidth = 20.0f;
                static constexpr float y_axisHalfHeight = 20.0f;
                const float x = Random::Float() * 10.0f;
                glm::vec3 startPosition(((x_axisHalfWidth / x) * 2) - x_axisHalfWidth, 0.0f, 70.0f);
                ReSpawnEnemyShip(enemyContainer, startPosition);
            }
        }

        if (counter == 2)
        {
            const auto &playerTranslation = mPlayerShip->GetRootComponent()->GetTranslation();
            const auto &mainCameraSp = mMainPlayerActorController->GetCamera();
            const glm::vec4 clippedSpaceTranslation = mainCameraSp->GetConvertedToClippedSpacePosition(glm::vec4(playerTranslation, 1.0f));
            const glm::vec3 ndcTranslation = glm::vec3(clippedSpaceTranslation.x / clippedSpaceTranslation.w,
                                                       clippedSpaceTranslation.y / clippedSpaceTranslation.w,
                                                       clippedSpaceTranslation.z / clippedSpaceTranslation.w);
            const glm::vec2 textureSpaceTranslation = glm::vec2(ndcTranslation.x * 0.5f + 0.5f, 1.0f - (ndcTranslation.y * 0.5f + 0.5f));
            uiComp->GetTextFieldById(textField1)->SetPosition(textureSpaceTranslation);
            uiComp->GetTextFieldById(textField1)->SetText(std::to_string(deltaTime * 100.0f) + "_text");
            counter = 0;
        }
        
        ++counter;
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
            Logger::Out("CombatController::ShootBullet => Error - no idle bullets in the pool");
            return;
        }

        if (const auto &bulletSp = idleBulletIt->first)
        {
            bulletSp->SetIsEnabled(true);
            bulletSp->GetRootComponent()->SetTranslation(bulletStartPosition);
            idleBulletIt->second = eBulletState::ACTIVE;
            Logger::Out("CombatController::ShootBuller => Successfull shoot.");
        }
    }

    void CombatController::ReSpawnEnemyShip(CombatEntity &spaceShip, const glm::vec3 &shipStartPosition)
    {
        spaceShip.RestoreLife();
        const auto &c_movement = spaceShip.GetSpaceShipActor()->GetMovementComponent();
        c_movement->Teleport(shipStartPosition);
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

                    Logger::Out("CombatController::FlushToPoolUsedBullets => Flush bullet to pool. Position = ",
                                bulletPosition, " . Actor name = ", weaponSP->GetName());
                }
            }
            else
            {
                Logger::Out("CombatController::FlushToPoolUsedBullets => Error: Bullet was destroyed!");
            }
        }
    }

    typename std::vector<CombatEntity>::iterator
    CombatController::FindEnemyShipByName(const std::string &actorName)
    {
        auto foundIt = std::find_if(mEnemies.begin(), mEnemies.end(),
                                    [&actorName = static_cast<const std::string &>(actorName)](const auto &enemyContainer)
                                    {
                                        return actorName == enemyContainer.GetSpaceShipActor()->GetName();
                                    });
        return foundIt;
    }

    typename std::vector<CombatEntity>::iterator
    CombatController::FindEnemyShipById(const uint64_t actorId)
    {
        auto foundIt = std::find_if(mEnemies.begin(), mEnemies.end(), [=](const auto &enemyContainer)
                                    { return actorId == enemyContainer.GetSpaceShipActor()->GetObjectId(); });
        return foundIt;
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