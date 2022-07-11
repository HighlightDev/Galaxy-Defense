#include "CombatController.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/UiComponents/UiComponent.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/LoggerExtension.h"

#include "Implementation/Factories/WeakSpaceShipFactory.h"
#include "Implementation/Factories/WeakMissileFactory.h"
#include "Implementation/Factories/BlackHoleMissileFactory.h"
#include "Implementation/Actors/BlackHoleMissileActor.h"
#include "Implementation/Controllers/SpaceShipPlayerController.h"

using namespace Graphics;
using namespace EnginePhysics;
using namespace EngineCore;
using namespace Resources;

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

    void CombatController::OnPreLevelInit()
    {
    }

    void CombatController::OnInitLevel()
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

                const auto c_uiComponent = a_enemyShip->GetComponentsByType<UiComponent>().back();
                const size_t dmgTextFieldId = c_uiComponent->CreateEmptyTextField("arial", 3, glm::vec3(1, 0.0, 0.0), 0.3, 1, false);
                mEnemies.emplace_back(CombatEntity(a_enemyShip, c_uiComponent, dmgTextFieldId));
            }
        }
    }

    void CombatController::OnPostLevelInit()
    {
    }

    void CombatController::PostPlayLevelFinished()
    {
        for (const auto &missile : mMissilesPool)
        {
            missile->SetIsEnabled(false); // disable all missiles at level start
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
                auto a_bulletIt = a_thisBulletIt != mMissilesPool.end() ? a_thisBulletIt : a_thatBulletIt;

                if (a_enemyShipIt != mEnemies.end() &&
                    a_bulletIt != mMissilesPool.end())
                {
                    auto missileActor = std::static_pointer_cast<BlackHoleMissileActor>(*a_bulletIt);
                    missileActor->TriggerActivePhaseExplosion();

                    const size_t dmg = std::max((size_t)(Random::Float() * 5.0f), 1UL);
                    const auto &dmgTextField = a_enemyShipIt->GetSpaceShipUiComponent()->GetTextFieldById(a_enemyShipIt->GetDmgTextFieldId());

                    if (a_enemyShipIt->CheckIsAliveAfterDamage(dmg))
                    {
                        a_enemyShipIt->SetIsDamageReceived(true);

                        const auto c_particle = a_enemyShipIt->GetSpaceShipActor()->GetComponentsByType<ParticleSystemComponent>().back();
                        c_particle->EmitParticles();

                        dmgTextField->SetText(std::to_string(dmg));
                        dmgTextField->SetVisibility(true);

                        const auto &spaceShipTranslation = a_enemyShipIt->GetSpaceShipActor()->GetRootComponent()->GetTranslation();
                        const auto &mainCameraSp = mMainPlayerActorController->GetCamera();
                        const glm::vec4 clippedSpaceTranslation = mainCameraSp->GetConvertedToClippedSpacePosition(glm::vec4(spaceShipTranslation, 1.0f));
                        const glm::vec3 ndcTranslation = glm::vec3(clippedSpaceTranslation.x / clippedSpaceTranslation.w,
                                                                   clippedSpaceTranslation.y / clippedSpaceTranslation.w,
                                                                   clippedSpaceTranslation.z / clippedSpaceTranslation.w);
                        const glm::vec2 textureSpaceTranslation = glm::vec2(ndcTranslation.x * 0.5f + 0.5f, 1.0f - (ndcTranslation.y * 0.5f + 0.5f));
                        dmgTextField->SetPosition(textureSpaceTranslation);
                    }
                    else
                    {
                        static constexpr float x_axisHalfWidth = 20.0f;
                        static constexpr float y_axisHalfHeight = 20.0f;
                        const float x = Random::Float() * 10.0f;
                        glm::vec3 startPosition(((x_axisHalfWidth / x) * 2) - x_axisHalfWidth, 0.0f, 70.0f);

                        dmgTextField->SetVisibility(false);

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
                if (const auto &sceneSp = mScene.lock())
                {
                    const auto &dmgTextField = enemyContainer.GetSpaceShipUiComponent()->GetTextFieldById(enemyContainer.GetDmgTextFieldId());
                    float dmgTime = enemyContainer.GetDamageDeltaTime();
                    dmgTime += deltaTime * 10.0f;
                    if (dmgTime > 1.0f)
                    {
                        dmgTime = 0.0f;
                        enemyContainer.SetIsDamageReceived(false);
                        dmgTextField->SetVisibility(false);
                    }
                    enemyContainer.SetDamageDeltaTime(dmgTime);

                    const auto &materialDamageProperty =
                        std::static_pointer_cast<EngineGOProperty<float>>(
                            enemyContainer.GetSpaceShipActor()->GetEnginePropertyByName("property_damageEffect"));
                    materialDamageProperty->SetValue(dmgTime);

                    const auto &spaceShipTranslation = enemyContainer.GetSpaceShipActor()->GetRootComponent()->GetTranslation();
                    const auto &mainCameraSp = mMainPlayerActorController->GetCamera();
                    const glm::vec4 clippedSpaceTranslation = mainCameraSp->GetConvertedToClippedSpacePosition(glm::vec4(spaceShipTranslation, 1.0f));
                    const glm::vec3 ndcTranslation = glm::vec3(clippedSpaceTranslation.x / clippedSpaceTranslation.w,
                                                               clippedSpaceTranslation.y / clippedSpaceTranslation.w,
                                                               clippedSpaceTranslation.z / clippedSpaceTranslation.w);
                    const glm::vec2 textureSpaceTranslation = glm::vec2(ndcTranslation.x * 0.5f + 0.5f, 1.0f - (ndcTranslation.y * 0.5f + 0.5f));
                    const float textWidth = sceneSp->GetTextWidthByTextFieldId_OnGameThread(dmgTextField);
                    dmgTextField->SetPosition(textureSpaceTranslation - (textWidth * 0.5f));
                    dmgTextField->SetPosition(textureSpaceTranslation + glm::vec2(0.0f, -0.2f));
                }
            }
        }

        if (bIsCoolDownInProgress)
        {
            mDeltaTime += deltaTime;

            if (mDeltaTime > mCoolDownTime)
            {
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
    }

    void CombatController::CreateWeaponBulletPool(const size_t poolSize, const std::shared_ptr<Scene> &sceneSp)
    {
        BlackHoleMissileFactory bulletFactory;
        for (size_t i = 0; i < poolSize; ++i)
        {
            const auto &a_shipBullet = bulletFactory.CreateWeaponBullet(sceneSp,
                                                                        glm::vec3(0),
                                                                        glm::vec3(),
                                                                        glm::vec3(1.0));

            mMissilesPool.emplace_back(a_shipBullet);
        }
    }

    void CombatController::ShootBullet(const glm::vec3 &bulletStartPosition)
    {
        auto idleBulletIt = std::find_if(mMissilesPool.begin(), mMissilesPool.end(), [](const auto &missile)
                                         { auto missileActor = std::static_pointer_cast<BlackHoleMissileActor>(missile);
                                          return eMissileActivityState::IDLE == missileActor->GetMissileActivityState(); });

        if (idleBulletIt == mMissilesPool.end())
        {
            LogInfo("CombatController::ShootBullet => Error - no idle bullets in the pool");
            return;
        }

        auto blackHoleMissile = std::static_pointer_cast<BlackHoleMissileActor>(*idleBulletIt);
        blackHoleMissile->TriggerSpawn(bulletStartPosition);
    }

    void CombatController::ReSpawnEnemyShip(CombatEntity &spaceShip, const glm::vec3 &shipStartPosition)
    {
        spaceShip.RestoreLife();
        const auto &c_movement = spaceShip.GetSpaceShipActor()->GetMovementComponent();
        c_movement->Teleport(shipStartPosition);
    }

    void CombatController::FlushToPoolUsedBullets()
    {
        for (auto &missile : mMissilesPool)
        {
            auto blackHoleMissileActor = std::static_pointer_cast<BlackHoleMissileActor>(missile);

            if (eMissileActivityState::ACTIVE == blackHoleMissileActor->GetMissileActivityState())
            {
                if (!blackHoleMissileActor->IsInsideLevel(mLevelBounds))
                {
                    blackHoleMissileActor->TriggerDisable();
                }
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
                                    { return enemyContainer.GetSpaceShipActor()->HasGameObjectIdInHierarchy(actorId); });
        return foundIt;
    }

    typename std::vector<std::shared_ptr<Actor>>::iterator
    CombatController::FindBulletByName(const std::string &actorName)
    {
        typename std::vector<std::shared_ptr<Actor>>::iterator foundIt = std::find_if(mMissilesPool.begin(),
                                                                                      mMissilesPool.end(),
                                                                                      [&actorName = static_cast<const std::string &>(actorName)](const auto &missile)
                                                                                      {
                                                                                          return actorName == missile->GetName();
                                                                                      });
        return foundIt;
    }

    typename std::vector<std::shared_ptr<Actor>>::iterator
    CombatController::FindBulletById(const uint64_t actorId)
    {
        typename std::vector<std::shared_ptr<Actor>>::iterator foundIt = std::find_if(mMissilesPool.begin(),
                                                                                      mMissilesPool.end(),
                                                                                      [=](const auto &missile)
                                                                                      {
                                                                                          return missile->HasGameObjectIdInHierarchy(actorId);
                                                                                      });
        return foundIt;
    }
}