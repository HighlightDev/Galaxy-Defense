#include "WeakSpaceShipFactory.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Scene.h"
#include "Implementation/AiActorController.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Components/SceneComponent.h"
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
#include "Core/GameCore/Particles/Emitters/ParticleExplosionEmitter.h"

using namespace EngineCore;
using namespace Graphics;

namespace Game
{
    size_t WeakSpaceShipFactory::s_weakSpaceShipCounter = 0;

    std::shared_ptr<Actor>
    WeakSpaceShipFactory::CreateSpaceShip(const std::shared_ptr<::EngineCore::Scene> &scene,
                                          const glm::vec3 &translation,
                                          const glm::vec3 &rotation,
                                          const glm::vec3 &scale)
    {
        const auto &enemyShipIndexStr = std::to_string(s_weakSpaceShipCounter++);
        const auto &rootComponent = std::make_shared<EngineCore::SceneComponent>("c_enemyShip_rootComponent_" + enemyShipIndexStr,
                                                                                 translation, glm::vec3(0), glm::vec3(1));
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

        const auto &damagePropSp = std::make_shared<EngineGOProperty<float>>(0.0f, "property_damageEffect");
        a_enemySpaceship->AddEngineProperty(damagePropSp);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, a_enemySpaceship.get(), "property_damageEffect", "damageTime");

        const MeshComponentData d_mesh("MeshComponentData_" + enemyShipIndexStr, "spaceship.obj", glm::vec3(0),
                                       rotation, scale, "", pbs_mat);

        const auto &c_mesh = scene->CreateComponent_GameThread<StaticMeshComponent, eComponentMetaType::StaticMesh>(d_mesh);
        a_enemySpaceship->AddComponent(c_mesh);

        MovementComponentData d_movement("NoPhysMoveComponentData_" + enemyShipIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
        const auto &c_movement = scene->CreateComponent_GameThread<NoPhysicsMovementComponent,
                                                                   eComponentMetaType::Movement>(d_movement);
        c_movement->SetSpeed(0.005f);

        a_enemySpaceship->AddComponent(c_movement);

        GhostController *ghostController = new GhostController(scene->GetPhysicsWorld(), new PhySphereShape(5.0f), 0.0f);
        scene->GetPhysicsWorld()->AddPhysDescriptor(ghostController);
        PhysicsComponentData physData("c_spaceShipPhysicsComponent_" + enemyShipIndexStr, ghostController);
        const auto &c_ghostPhysics = scene->CreateComponent_GameThread<GhostPhysicsComponent, eComponentMetaType::Physics>(physData);
        a_enemySpaceship->AddComponent(c_ghostPhysics);

        ParticleSystemComponentData d_particle("c_particleSystemComponent_" + enemyShipIndexStr, glm::vec3(0), 200);
        const auto &c_particleSystemComponent = scene->CreateComponent_GameThread<ParticleSystemComponent,
                                                                                  eComponentMetaType::ParticleSystem>(d_particle);
        auto emitter = std::make_shared<ParticleExplosionEmitter>();
        emitter->SetOwner(c_particleSystemComponent);
        emitter->SetColor(glm::vec4(1.0f, 1.0f, 0.3f, 1.0f), glm::vec4(0.5f, 1.0f, 0.3f, 1.0f));
        emitter->SetSize(0.4f, 0.1f);
        emitter->SetLifeTime(0.5f);
        c_particleSystemComponent->SetParticleEmitter(emitter);

        a_enemySpaceship->AddComponent(c_particleSystemComponent);

        scene->AddActorController(std::make_shared<AiActorController>(a_enemySpaceship));

        TweenerParser tweenerParser;
        const auto &spaceshipTweener = tweenerParser.ParseTweenerDescriptor("spaceshipMove.tween");

        a_enemySpaceship->AttachTweener(spaceshipTweener);

        const auto &binding = spaceshipTweener->GetPropertyBindingByName("b_rotator");
        BindingAttachmentBuilder::SetAttachment(rootComponent.get(), binding.get(), "b_rotator");

        return a_enemySpaceship;
    }
}