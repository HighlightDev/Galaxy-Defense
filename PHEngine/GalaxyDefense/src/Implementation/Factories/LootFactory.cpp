#include "LootFactory.h"

#include "Core/GameCore/Components/ComponentCreators/InstancedStaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ParticleSystemComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/InstancedMeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"
#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/ParticleComponents/CpuParticleSystemComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/InstancedStaticMeshComponent.h"
#include "Core/GameCore/Particles/Emitters/ParticleExplosionEmitter.h"
#include "Core/GameCore/Particles/Modules/Color/SimpleColorModule.h"
#include "Core/GameCore/Particles/Modules/Lifetime/RandomRangeLifeTimeModule.h"
#include "Core/GameCore/Particles/Modules/Size/SimpleSizeModule.h"
#include "Core/GameCore/Particles/Modules/Velocity/RadialVelocityModule.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionSphereShape.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/GameCore/Tweener/State.h"
#include "Core/GameCore/Tweener/StateTransition.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Implementation/Actors/LootActor.h"
#include "Implementation/Components/MovementComponents/LootDropMovementComponent.h"
#include "Implementation/Controllers/AiActorController.h"
#include "Implementation/DataProviders/GameConstants.h"

using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t LootFactory::s_lootCounter = 0;

std::shared_ptr<LootActor> LootFactory::CreateLoot(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    const eLootCategory lootCategory)
{
    const auto& lootIndexStr = std::to_string(s_lootCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_loot_root_" + lootIndexStr, translation, glm::vec3(0), glm::vec3(1), true);
    const auto& a_loot = std::make_shared<LootActor>("a_loot_" + lootIndexStr, rootComponent);
    scene->AddActor(a_loot);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& loot_material = materialParser.ParseMaterialDescriptor("PbrSingleValueMaterial.m");
    scene->RegisterMaterialInstance(loot_material);
    MaterialPropertySetter::SetMaterialPropertyValue(loot_material, "albedo", glm::vec3(0.0f, 1.0f, 1.0f));
    MaterialPropertySetter::SetMaterialPropertyValue(loot_material, "metallicValue", 0.5f);
    MaterialPropertySetter::SetMaterialPropertyValue(loot_material, "roughnessValue", 0.5f);
    MaterialPropertySetter::SetMaterialPropertyValue(loot_material, "opacity", 1.0f);
    auto meshComponentCreator = std::make_shared<InstancedStaticMeshComponentCreator<InstancedStaticMeshComponent>>(false);
    const auto data = std::make_shared<InstancedMeshComponentData>(
        "c_loot_mesh_" + lootIndexStr, "сrystal_1.obj", glm::vec3(0), glm::vec3(0), glm::vec3(1.0f), loot_material);
    const auto& meshComponent
        = std::static_pointer_cast<InstancedStaticMeshComponent>(scene->CreateComponent_GameThread(meshComponentCreator, data));
    a_loot->AddComponent(meshComponent);
    a_loot->SetScene(scene);
    a_loot->SetLootCategory(lootCategory);

    const auto d_movement
        = std::make_shared<MovementComponentData>("LootDropMoveData_" + lootIndexStr, glm::vec3(0.0f, -1.0f, 0.0f));
    const auto& moveComponentCreator = std::make_shared<MovementComponentCreator<LootDropMovementComponent>>();
    const auto& c_movement = std::static_pointer_cast<LootDropMovementComponent>(
        scene->CreateComponent_GameThread(moveComponentCreator, d_movement));
    c_movement->SetFallDistance(5.0f);
    c_movement->SetBounceAmplitude(0.5f);
    c_movement->SetBounceDuration(0.3f);
    c_movement->SetCurrentSpeed(16.0f);
    a_loot->AddComponent(c_movement);

    const auto& sphereShape = std::make_shared<CollisionSphereShape>(
        glm::length(scale) * 1.0f); // make radius a bit bigger to make it more priority target for collision
    const auto& ghostController = std::make_shared<GhostController>(scene->GetPhysicsWorld(), sphereShape, 0.0f);
    const auto physData = std::make_shared<PhysicsComponentData>("c_lootPhysicsComponent_" + lootIndexStr, ghostController);
    const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
    const auto& c_ghostPhysics
        = std::static_pointer_cast<PhysicsComponent>(scene->CreateComponent_GameThread(physicsComponentCreator, physData));
    sphereShape->SetParentPhysicsComponent(c_ghostPhysics);
    a_loot->AddComponent(c_ghostPhysics);

    const std::shared_ptr<IMaterial>& particles_mat = materialParser.ParseMaterialDescriptor("OpacityMaskParticleMaterial.m");
    scene->RegisterMaterialInstance(particles_mat);
    const auto& shiningMask = Resources::TexturePool::GetInstance()->GetOrAllocateResource("shining_mask.png");
    MaterialPropertySetter::SetMaterialPropertyValue(particles_mat, "opacityMask", shiningMask);
    const auto d_particle = std::make_shared<ParticleSystemComponentData>(
        "c_lootParticles_" + lootIndexStr, particles_mat, glm::vec3(0), glm::vec3(1.0f), 50);
    const auto& particleCreator = std::make_shared<ParticleSystemComponentCreator<GpuParticleSystemComponent>>();
    const auto& c_particles
        = std::static_pointer_cast<GpuParticleSystemComponent>(scene->CreateComponent_GameThread(particleCreator, d_particle));
    c_particles->SetIsEndlessRespawnEnabled(true); // Keep emitting particles while loot is active, to make it more eye-catching

    auto emitter = std::make_shared<ParticleExplosionEmitter>();
    emitter->SetOwner(c_particles);
    emitter->SetExplosionRadius(0.5f);
    emitter->SetThetaSlicesCount(5);
    c_particles->SetParticleEmitter(emitter);

    auto lifeTimeModule = std::make_shared<RandomRangeLifeTimeModule>();
    lifeTimeModule->SetOwner(c_particles);
    lifeTimeModule->SetMinLifeTime(0.4f);
    lifeTimeModule->SetMaxLifeTime(0.9f);
    c_particles->AddParticleModule(lifeTimeModule);

    auto velocityModule = std::make_shared<RadialVelocityModule>();
    velocityModule->SetOwner(c_particles);
    velocityModule->SetSpeed(3.5f);
    c_particles->AddParticleModule(velocityModule);

    auto sizeModule = std::make_shared<SimpleSizeModule>();
    sizeModule->SetOwner(c_particles);
    sizeModule->SetSizeBegin(0.4f);
    sizeModule->SetSizeEnd(0.1f);
    c_particles->AddParticleModule(sizeModule);

    auto colorModule = std::make_shared<SimpleColorModule>();
    colorModule->SetOwner(c_particles);
    colorModule->SetColorBegin(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
    colorModule->SetColorEnd(glm::vec4(0.0f, 0.5f, 0.8f, 0.0f));
    c_particles->AddParticleModule(colorModule);

    a_loot->AddComponent(c_particles);

    scene->AddActorController(std::make_shared<AiActorController>(a_loot));

    // Build the collect fly-off tweener programmatically: s_Idle → s_Collected via a
    // quadratic-bezier arc.  Property values are placeholders here; they are overwritten
    // at each SpawnLoot() call with the actual dynamic spawn position.
    {
        using BezierProp_t = StateProperty<eEnginePropertyBindingType::Vec3QuadraticBezier>;
        using BezierBinding_t = Vec3QuadraticBezierPropertyBinding;
        using RotProp_t = StateProperty<eEnginePropertyBindingType::EulerAnglesRotation>;
        using RotBinding_t = EulerAnglesRotationPropertyBinding;

        auto stateIdle = std::make_shared<State>("s_Idle");
        auto stateCollected = std::make_shared<State>("s_Collected");
        std::vector<std::shared_ptr<State>> allStates = {stateIdle, stateCollected};

        auto lootTweener = std::make_shared<Tweener>("lootCollect", "LootCollect", stateIdle, std::move(allStates));

        StateTransition collectTransition(stateIdle, stateCollected, Game::Constants::c_lootTweenTimeSec);
        stateIdle->AddStateTransition(collectTransition);

        const auto b_translation = std::make_shared<BezierBinding_t>("b_lootTranslation");
        lootTweener->AddPropertyBinding("b_lootTranslation", b_translation);
        BindingAttachmentBuilder::SetAttachment(rootComponent, b_translation, "p_translation");

        const auto b_rotator = std::make_shared<RotBinding_t>("b_lootRotator");
        lootTweener->AddPropertyBinding("b_lootRotator", b_rotator);
        BindingAttachmentBuilder::SetAttachment(rootComponent, b_rotator, "p_rotator");

        const glm::vec3 dummyPos(0.0f);
        const auto idleProp = std::make_shared<BezierProp_t>(dummyPos, dummyPos, b_translation);
        const auto collectedProp = std::make_shared<BezierProp_t>(dummyPos, dummyPos, b_translation);
        stateIdle->AddStateProperty(idleProp);
        stateCollected->AddStateProperty(collectedProp);

        const auto idleRotProp = std::make_shared<RotProp_t>(glm::vec3(0.0f), b_rotator);
        const auto collectedRotProp = std::make_shared<RotProp_t>(glm::vec3(0.0f), b_rotator);
        stateIdle->AddStateProperty(idleRotProp);
        stateCollected->AddStateProperty(collectedRotProp);

        a_loot->SetupLootTweener(lootTweener, idleProp, collectedProp, collectedRotProp);
    }

    return a_loot;
}
} // namespace Game