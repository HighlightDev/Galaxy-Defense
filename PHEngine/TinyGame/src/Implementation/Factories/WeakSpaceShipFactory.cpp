#include "WeakSpaceShipFactory.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Scene.h"
#include "Implementation/Controllers/AiSpaceshipActorController.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Actors/WeakSpaceshipActor.h"
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
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhySphereShape.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Particles/Emitters/ParticleExplosionEmitter.h"
#include "Core/GameCore/Particles/Modules/Velocity/SimpleVelocityModule.h"
#include "Core/GameCore/Particles/Modules/Velocity/ExplosionInitialVelocityModule.h"
#include "Core/GameCore/Particles/Modules/Color/SimpleColorModule.h"
#include "Core/GameCore/Particles/Modules/Size/SimpleSizeModule.h"
#include "Core/GameCore/Particles/Modules/Lifetime/SimpleLifeTimeModule.h"

#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ParticleSystemComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/UiComponentCreator.h"
#include "Core/GameCore/Components/UiComponents/UiComponent.h"

#include "Core/ResourceManagerCore/Pool/TexturePool.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game
{
    size_t WeakSpaceShipFactory::s_weakSpaceShipCounter = 0;

    std::shared_ptr<SpaceshipActor>
    WeakSpaceShipFactory::CreateSpaceShip(const std::shared_ptr<::EngineCore::Scene> &scene,
                                          const glm::vec3 &translation,
                                          const glm::vec3 &rotation,
                                          const glm::vec3 &scale)
    {
        const auto &enemyShipIndexStr = std::to_string(s_weakSpaceShipCounter++);
        const auto &rootComponent = std::make_shared<EngineCore::SceneComponent>("c_enemyShip_rootComponent_" + enemyShipIndexStr,
                                                                                 translation, glm::vec3(0), glm::vec3(1));
        const auto &a_enemySpaceship = std::make_shared<WeakSpaceshipActor>("a_enemyShip_" + enemyShipIndexStr, rootComponent);
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

        const auto &damagePropSp = std::make_shared<EngineGOProperty<float>>(0.0f, "p_damageEffect");
        a_enemySpaceship->AddEngineProperty(damagePropSp);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, a_enemySpaceship.get(), "p_damageEffect", "damageTime");

        const MeshComponentData d_mesh("MeshComponentData_" + enemyShipIndexStr, "spaceship.obj", glm::vec3(0),
                                       rotation, scale, "", pbs_mat);
        const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>();
        const auto &c_mesh = scene->CreateComponent_GameThread(meshComponentCreator, d_mesh);
        a_enemySpaceship->AddComponent(c_mesh);

        MovementComponentData d_movement("NoPhysMoveComponentData_" + enemyShipIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
        const auto& moveComponentCreator = std::make_shared<MovementComponentCreator<NoPhysicsMovementComponent>>();
        const auto &c_movement = std::static_pointer_cast<NoPhysicsMovementComponent>(scene->CreateComponent_GameThread(moveComponentCreator, d_movement));
        c_movement->SetSpeed(0.005f);
        a_enemySpaceship->AddComponent(c_movement);

        GhostController *ghostController = new GhostController(scene->GetPhysicsWorld(), new PhySphereShape(5.0f), 0.0f);
        scene->GetPhysicsWorld()->AddPhysDescriptor(ghostController);
        PhysicsComponentData physData("c_spaceShipPhysicsComponent_" + enemyShipIndexStr, ghostController);
        const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
        const auto &c_ghostPhysics = scene->CreateComponent_GameThread(physicsComponentCreator, physData);;
        a_enemySpaceship->AddComponent(c_ghostPhysics);

        const auto &particles_mat = materialParser.ParseMaterialDescriptor("OpaqueParticleMaterial.m");
        MaterialPropertySetter::SetMaterialPropertyValue(particles_mat, "opacity", 1.0f);

        ParticleSystemComponentData d_particle("c_particleSystemComponent_" + enemyShipIndexStr, particles_mat, glm::vec3(0), 100);
        const auto& particleSystemComponentCreator = std::make_shared<ParticleSystemComponentCreator<ParticleSystemComponent>>();
        const auto &c_particleSystemComponent = std::static_pointer_cast<ParticleSystemComponent>(scene->CreateComponent_GameThread(particleSystemComponentCreator, d_particle));
        auto emitter = std::make_shared<ParticleExplosionEmitter>();
        emitter->SetOwner(c_particleSystemComponent);
        emitter->SetThetaSlicesCount(10);
        c_particleSystemComponent->SetParticleEmitter(emitter);

        auto lifeTimeModule = std::make_shared<SimpleLifeTimeModule>();
        lifeTimeModule->SetOwner(c_particleSystemComponent);
        lifeTimeModule->SetLifeTime(2.5f);
        c_particleSystemComponent->AddParticleModule(lifeTimeModule);

        auto sizeModule = std::make_shared<SimpleSizeModule>();
        sizeModule->SetOwner(c_particleSystemComponent);
        sizeModule->SetSizeBegin(0.4f);
        sizeModule->SetSizeEnd(0.1f);
        c_particleSystemComponent->AddParticleModule(sizeModule);

        auto initialVelocityModule = std::make_shared<ExplosionInitialVelocityModule>();
        initialVelocityModule->SetOwner(c_particleSystemComponent);
        c_particleSystemComponent->AddParticleModule(initialVelocityModule);

        auto velocityModule = std::make_shared<SimpleVelocityModule>();
        velocityModule->SetOwner(c_particleSystemComponent);
        velocityModule->SetVelocityDirection(glm::vec3(0, -25.0f, 0));
        velocityModule->SetVelocityDeviation(glm::vec3(2.0f, 0.0f, 2.0f));
        c_particleSystemComponent->AddParticleModule(velocityModule);

        auto colorModule = std::make_shared<SimpleColorModule>();
        colorModule->SetOwner(c_particleSystemComponent);
        colorModule->SetColorBegin(glm::vec4(1.0f, 0.7f, 0.2f, 1.0f));
        colorModule->SetColorEnd(glm::vec4(1.0f, 0.2f, 0.02f, 1.0f));
        c_particleSystemComponent->AddParticleModule(colorModule);

        a_enemySpaceship->AddComponent(c_particleSystemComponent);

        scene->AddActorController(std::make_shared<AiSpaceshipActorController>(a_enemySpaceship));

        TweenerParser tweenerParser;
        const auto &spaceshipTweener = tweenerParser.ParseTweenerDescriptor("spaceshipMove.tween");

        a_enemySpaceship->AttachTweener(spaceshipTweener);

        const auto &binding = spaceshipTweener->GetPropertyBindingByName("b_rotator");
        BindingAttachmentBuilder::SetAttachment(rootComponent.get(), binding.get(), "b_rotator");

        const auto &uiComponentCreator = std::make_shared<UiComponentCreator<UiComponent>>();
        const auto &c_uiComponent = scene->CreateComponent_GameThread(uiComponentCreator, ComponentData("c_uiComponent_" + enemyShipIndexStr));
        a_enemySpaceship->AddComponent(c_uiComponent);

        return a_enemySpaceship;
    }
}