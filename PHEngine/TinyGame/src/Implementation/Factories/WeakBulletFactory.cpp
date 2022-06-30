#include "WeakBulletFactory.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Scene.h"
#include "Implementation/AiActorController.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhySphereShape.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"

#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"

#include "Core/ResourceManagerCore/Pool/TexturePool.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game
{
    size_t WeakBulletFactory::s_weakBulletCounter = 0;

    std::shared_ptr<Actor>
    WeakBulletFactory::CreateWeaponBullet(const std::shared_ptr<::EngineCore::Scene> &scene,
                                          const glm::vec3 &translation,
                                          const glm::vec3 &rotation,
                                          const glm::vec3 &scale)
    {
        const auto &shipBulletIndexStr = std::to_string(s_weakBulletCounter++);
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

        const MeshComponentData d_mesh("c_meshComponent_" + shipBulletIndexStr, "playerCube.obj", glm::vec3(0),
                                       glm::vec3(0), glm::vec3(2), "", pbs_mat);
        const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>();
        const auto &c_mesh = scene->CreateComponent_GameThread(meshComponentCreator, d_mesh);
        a_bullet->AddComponent(c_mesh);

        MovementComponentData d_movement("c_noPhysMoveComponent_" + shipBulletIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
        const auto& moveComponentCreator = std::make_shared<MovementComponentCreator<NoPhysicsMovementComponent>>();
        const auto &c_movement = std::static_pointer_cast<NoPhysicsMovementComponent>(scene->CreateComponent_GameThread(moveComponentCreator, d_movement));
        c_movement->SetSpeed(0.03f);
        c_movement->SetDirection(glm::vec3(.0f, .0f, 1.0f));
        a_bullet->AddComponent(c_movement);

        ComponentData d_audio("c_soundComponent_" + shipBulletIndexStr);
        const auto& soundComponentCreator = std::make_shared<AudioComponentCreator<SoundComponent>>();
        const auto &c_sound = std::static_pointer_cast<SoundComponent>(scene->CreateComponent_GameThread(soundComponentCreator, d_audio));
        c_sound->CreateSoundBuffer("bounce.wav", "explosion");
        a_bullet->AddComponent(c_sound);

        GhostController *ghostController = new GhostController(scene->GetPhysicsWorld(), new PhySphereShape(3.0f), 0.0f);
        scene->GetPhysicsWorld()->AddPhysDescriptor(ghostController);
        PhysicsComponentData physData("c_bulletPhysicsComponent_" + shipBulletIndexStr, ghostController);
        const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
        const auto &c_ghostPhysics = scene->CreateComponent_GameThread(physicsComponentCreator, physData);
        a_bullet->AddComponent(c_ghostPhysics);

        const auto &bulletActorController = std::make_shared<AiActorController>(a_bullet);
        scene->AddActorController(bulletActorController);

        return a_bullet;
    }
}