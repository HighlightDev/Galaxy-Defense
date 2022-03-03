#include "EnemySceneController.h"
#include "AiActorController.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"

using namespace Graphics;

namespace Game
{
    EnemySceneController::EnemySceneController(const std::weak_ptr<Scene> &scene)
        : mScene(scene), mEnemies(), mEnemyActorControllers()
    {
    }

    void EnemySceneController::PreInit()
    {
    }

    void EnemySceneController::PostInit()
    {
    }

    void EnemySceneController::Tick(const float deltaTime)
    {
    }

    void EnemySceneController::SpawnEnemySpaceShip(const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale)
    {
        if (const auto &sceneSp = mScene.lock())
        {
            const auto &a_enemySpaceship = CreateEnemySpaceShip(sceneSp, translation, rotation, scale);
        }
    }

    std::shared_ptr<Actor> EnemySceneController::CreateEnemySpaceShip(const std::shared_ptr<Scene> &scene, const glm::vec3 &translation,
                                                                      const glm::vec3 &rotation, const glm::vec3 &scale)
    {
        const auto &enemyShipIndexStr = std::to_string(enemyShipCounter);
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

        MovementComponentData d_movement("NoPhysMoveComponentData_" + enemyShipIndexStr, glm::vec3());
        const auto &c_movement = scene->CreateComponent_GameThread<NoPhysicsMovementComponent,
                                                                   eComponentMetaType::Movement>(d_movement);
        a_enemySpaceship->AddComponent(c_movement);

        const auto &enemyController = std::make_shared<AiActorController>(a_enemySpaceship);
        scene->AddActorController(enemyController);

        mEnemies.emplace_back(a_enemySpaceship);
        mEnemyActorControllers.emplace_back(enemyController);

        ++enemyShipCounter;
        return a_enemySpaceship;
    }
}