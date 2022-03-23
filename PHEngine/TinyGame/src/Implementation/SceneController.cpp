#include "SceneController.h"
#include "AiActorController.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"

#include <random>
#include <utility>
#include <vector>
#include <ctime>

using namespace Graphics;

namespace Game
{

    float get_random(float min, float max)
    {
        static std::default_random_engine e;
        static std::uniform_real_distribution<> dis(min, max);
        return dis(e);
    }

    std::vector<std::pair<GameObject *, EngineGOProperty<float>>> gameObjects;
    float mDeltaTime = 1.0f;

    SceneController::SceneController(const std::weak_ptr<Scene> &scene)
        : mScene(scene),
          mEnemies(),
          mEnemyActorControllers(),
          mDummyBullet(),
          mLevelBounds(BoundingBox(glm::vec3(0), glm::vec3(50)))
    {
        MainPlayerActionEvent::GetInstance()->AddListener(this);
    }

    SceneController::~SceneController()
    {
        MainPlayerActionEvent::GetInstance()->RemoveListener(this);
    }

    void SceneController::PreInit()
    {
    }

    void SceneController::ProcessEvent(const typename MainPlayerActionEvent::EventData_t &data)
    {
        if (!bIsCoolDownInProgress)
        {
            bIsCoolDownInProgress = true;
            const auto &playerAction = std::get<0>(data);
            if (const auto &sceneSp = mScene.lock())
            {
                if (const auto &mainPlayerSp = mMainPlayerShip.lock())
                {
                    const auto &playerTranslation = mainPlayerSp->GetRootComponent()->GetTranslation();
                    const auto &a_bulletShip = CreateWeaponBullet(sceneSp,
                                                                  playerTranslation,
                                                                  glm::vec3(),
                                                                  glm::vec3(1.0));

                    mWeaponBulletsPool.emplace_back(a_bulletShip);
                }
            }
        }
    }

    void SceneController::PostInit()
    {
        if (const auto &sceneSp = mScene.lock())
        {
            // due to resource lazy upload to pool - create dummy bullet with all necessary resources, which will be used further
            mDummyBullet = CreateWeaponBullet(sceneSp,
                                              glm::vec3(10000000000),
                                              glm::vec3(),
                                              glm::vec3(1.0));

            std::srand(std::time(nullptr));
            for (size_t i = 0; i < 4; i++)
            {
                static constexpr float x_axisHalfWidth = 50.0f;
                static constexpr float y_axisHalfHeight = 30.0f;
                const float x = get_random(0.5f, 10.0f);
                const float scale = glm::clamp(x, 0.5f, 3.0f);
                glm::vec3 startPosition(((x_axisHalfWidth / x) * 2) - x_axisHalfWidth,
                                        ((y_axisHalfHeight / x) * 2) - y_axisHalfHeight,
                                        50 + (i * i) + 2);
                const auto &a_enemyShip = CreateEnemySpaceShip(sceneSp,
                                                               startPosition,
                                                               glm::vec3(),
                                                               glm::vec3(scale));

                mEnemies.emplace_back(a_enemyShip);
            }
        }
    }

    void SceneController::SetPlayerShipActor(const std::weak_ptr<Actor> &mainPlayerShip)
    {
        mMainPlayerShip = mainPlayerShip;
    }

    void SceneController::SetPlayeActorController(const std::weak_ptr<SpaceShipPlayerController> &mainPlayerActorController)
    {
        mMainPlayerActorController = mainPlayerActorController;
        const auto &playerActorControllerSp = mMainPlayerActorController.lock();
        assert(playerActorControllerSp);
        playerActorControllerSp->SetLevelBounds(mLevelBounds);
    }

    void SceneController::Tick(const float deltaTime)
    {
        if (bIsCoolDownInProgress)
        {
            mDeltaTime += deltaTime;

            if (mDeltaTime > mCoolDownTime)
            {
                mDeltaTime = 0.0f;
                bIsCoolDownInProgress = false;
            }

            // for (auto &goPair : gameObjects)
            // {
            //     EngineGOProperty<float> &prop = goPair.second;
            //     prop.SetValue(mDeltaTime);
            // }
        }

        for (const auto &enemy : mEnemies)
        {
            if (const auto &enemySp = enemy.lock())
            {
                const auto &enemyTranslation = enemySp->GetRootComponent()->GetTranslation();
                if (glm::length(enemyTranslation) > 70.0f)
                {
                    std::srand(std::time(nullptr));
                    static constexpr float x_axisHalfWidth = 40.0f;
                    static constexpr float y_axisHalfHeight = 20.0f;
                    const float x = get_random(1.0f, 10.0f);
                    glm::vec3 startPosition(((x_axisHalfWidth / x) * 2) - x_axisHalfWidth,
                                            ((y_axisHalfHeight / x) * 2) - y_axisHalfHeight,
                                            60.0f);
                    const auto &c_movement = enemySp->GetMovementComponent();
                    c_movement->Teleport(startPosition);
                }
            }
        }
    }

    std::shared_ptr<Actor> SceneController::CreateEnemySpaceShip(const std::shared_ptr<Scene> &scene, const glm::vec3 &translation,
                                                                 const glm::vec3 &rotation, const glm::vec3 &scale)
    {
        const auto &enemyShipIndexStr = std::to_string(enemyShipCounter++);
        const auto &rootComponent = std::make_shared<EngineCore::SceneComponent>("c_enemyShip_rootComponent_" + enemyShipIndexStr,
                                                                                 translation, rotation, scale);
        const auto &a_enemySpaceship = std::make_shared<Actor>("a_enemyShip_" + enemyShipIndexStr, rootComponent);
        scene->AddActor(a_enemySpaceship);

        MaterialParser materialParser;
        const auto &pbs_mat = materialParser.ParseMaterialDescriptor("SpaceshipPBS.m");

        const std::string albedoName = "spaceship_albedo.jpg";
        const std::string normalName = "spaceship_normal.jpg";
        const std::string roughnessName = "spaceship_roughness.jpg";
        const std::string metallicName = "spaceship_metallic.jpg";

        const auto &albedo_tex = TexturePool::GetInstance()->GetOrAllocateResource(albedoName);
        const auto &normal_tex = TexturePool::GetInstance()->GetOrAllocateResource(normalName);
        const auto &roughness_tex = TexturePool::GetInstance()->GetOrAllocateResource(roughnessName);
        const auto &metallic_tex = TexturePool::GetInstance()->GetOrAllocateResource(metallicName);
        const float uvScale = 1.0f;

        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "albedo", albedo_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "normalMap", normal_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "roughnessMap", roughness_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "metallicMap", metallic_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "uvScale", uvScale);

        GameObject *testGO = new GameObject("testGameObject_" + enemyShipIndexStr);
        gameObjects.emplace_back(std::make_pair(testGO, EngineGOProperty<float>(0.0f, "property_damageEffect")));
        testGO->AddEngineProperty(gameObjects.back().second);

        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, testGO, "property_damageEffect", "damageTime");

        const MeshComponentData d_mesh("MeshComponentData_" + enemyShipIndexStr, "spaceship.obj", glm::vec3(0),
                                       glm::vec3(0), glm::vec3(9), "", pbs_mat);

        const auto &c_mesh = scene->CreateComponent_GameThread<StaticMeshComponent, eComponentMetaType::StaticMesh>(d_mesh);
        a_enemySpaceship->AddComponent(c_mesh);

        MovementComponentData d_movement("NoPhysMoveComponentData_" + enemyShipIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
        const auto &c_movement = scene->CreateComponent_GameThread<NoPhysicsMovementComponent,
                                                                   eComponentMetaType::Movement>(d_movement);
        c_movement->SetSpeed(0.005f);

        a_enemySpaceship->AddComponent(c_movement);

        const auto &enemyController = std::make_shared<AiActorController>(a_enemySpaceship);
        scene->AddActorController(enemyController);

        TweenerParser tweenerParser;
        const auto &spaceshipTweener = tweenerParser.ParseTweenerDescriptor("spaceshipMove.tween");

        a_enemySpaceship->AttachTweener(spaceshipTweener);

        const auto &binding = spaceshipTweener->GetPropertyBindingByName("b_rotator");
        BindingAttachmentBuilder::SetAttachment(rootComponent.get(), binding.get(), "b_rotator");

        mEnemyActorControllers.emplace_back(enemyController);

        return a_enemySpaceship;
    }

    std::shared_ptr<Actor> SceneController::CreateWeaponBullet(const std::shared_ptr<Scene> &scene, const glm::vec3 &translation,
                                                               const glm::vec3 &rotation, const glm::vec3 &scale)
    {
        const auto &shipBulletIndexStr = std::to_string(bulletCounter++);
        const auto &rootComponent = std::make_shared<EngineCore::SceneComponent>("c_bullet_rootComponent_" + shipBulletIndexStr,
                                                                                 translation, rotation, scale);
        const auto &a_bullet = std::make_shared<Actor>("a_bullet_" + shipBulletIndexStr, rootComponent);
        scene->AddActor(a_bullet);

        MaterialParser materialParser;
        const auto &pbs_mat = materialParser.ParseMaterialDescriptor("PhysicalBasedMaterial.m");

        const std::string albedoName = "solar_cells_albedo_512.jpg";
        const std::string normalName = "solar_cells_normal_512.jpg";
        const std::string roughnessName = "solar_cells_roughness_512.jpg";
        const std::string metallicName = "solar_cells_metallic_512.jpg";

        const auto &albedo_tex = TexturePool::GetInstance()->GetOrAllocateResource(albedoName);
        const auto &normal_tex = TexturePool::GetInstance()->GetOrAllocateResource(normalName);
        const auto &roughness_tex = TexturePool::GetInstance()->GetOrAllocateResource(roughnessName);
        const auto &metallic_tex = TexturePool::GetInstance()->GetOrAllocateResource(metallicName);
        const float uvScale = 0.5f;

        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "albedo", albedo_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "normalMap", normal_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "roughnessMap", roughness_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "metallicMap", metallic_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "uvScale", uvScale);

        const MeshComponentData d_mesh("c_meshData_" + shipBulletIndexStr, "playerCube.obj", glm::vec3(0),
                                       glm::vec3(0), glm::vec3(2), "", pbs_mat);

        const auto &c_mesh = scene->CreateComponent_GameThread<StaticMeshComponent, eComponentMetaType::StaticMesh>(d_mesh);
        a_bullet->AddComponent(c_mesh);

        MovementComponentData d_movement("c_noPhysMoveData_" + shipBulletIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
        const auto &c_movement = scene->CreateComponent_GameThread<NoPhysicsMovementComponent,
                                                                   eComponentMetaType::Movement>(d_movement);
        c_movement->SetSpeed(0.01f);
        c_movement->SetDirection(glm::vec3(.0f, .0f, 1.0f));

        a_bullet->AddComponent(c_movement);

        const auto &bulletActorController = std::make_shared<AiActorController>(a_bullet);
        scene->AddActorController(bulletActorController);

        return a_bullet;
    }
}