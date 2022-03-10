#include "EnemySceneController.h"
#include "AiActorController.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"

#include <random>

using namespace Graphics;

namespace Game
{

    float get_random(float min, float max)
    {
        static std::default_random_engine e;
        static std::uniform_real_distribution<> dis(min, max);
        return dis(e);
    }

    EnemySceneController::EnemySceneController(const std::weak_ptr<Scene> &scene)
        : mScene(scene), mEnemies(), mEnemyActorControllers()
    {
    }

    void EnemySceneController::PreInit()
    {
    }

    void EnemySceneController::PostInit()
    {
        if (const auto &sceneSp = mScene.lock())
        {
            std::srand(std::time(nullptr));
            for (size_t i = 0; i < 10; i++)
            {
                static constexpr float x_axisHalfWidth = 20.0f;
                static constexpr float y_axisHalfHeight = 10.0f;
                const float x = get_random(1.0f, 10.0f);
                glm::vec3 startPosition(((x_axisHalfWidth / x) * 2) - x_axisHalfWidth,
                                        ((y_axisHalfHeight / x) * 2) - y_axisHalfHeight,
                                        50 + (i * i) + 2);
                CreateEnemySpaceShip(sceneSp,
                                     startPosition,
                                     glm::vec3(),
                                     glm::vec3(glm::clamp(glm::vec3(static_cast<float>(i)), 0.1f, 5.0f)));
            }
        }
    }

    void EnemySceneController::Tick(const float deltaTime)
    {
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

    void EnemySceneController::SpawnEnemySpaceship(const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale)
    {
        if (const auto &sceneSp = mScene.lock())
        {
            const auto &a_enemySpaceship = CreateEnemySpaceShip(sceneSp, translation, rotation, scale);
        }
    }

    std::shared_ptr<Actor> EnemySceneController::CreateEnemySpaceShip(const std::shared_ptr<Scene> &scene, const glm::vec3 &translation,
                                                                      const glm::vec3 &rotation, const glm::vec3 &scale)
    {
        const auto &enemyShipIndexStr = std::to_string(enemyShipCounter++);
        const auto &rootComponent = std::make_shared<EngineCore::SceneComponent>("enemyShip_rootComponent_" + enemyShipIndexStr,
                                                                                 translation, rotation, scale);
        const auto &a_enemySpaceship = std::make_shared<Actor>("enemyShip_" + enemyShipIndexStr, rootComponent);
        scene->AddActor(a_enemySpaceship);

        MaterialParser materialParser;
        const auto &pbs_mat = materialParser.ParseMaterialDescriptor("PhysicalBasedMaterial.m");

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

        mEnemies.emplace_back(a_enemySpaceship);
        mEnemyActorControllers.emplace_back(enemyController);

        return a_enemySpaceship;
    }
}