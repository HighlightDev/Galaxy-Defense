#include "BlackHoleMissileFactory.h"

#include "Core/AudioCore/SoundSource.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ParticleSystemComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Particles/Emitters/ParticleExplosionEmitter.h"
#include "Core/GameCore/Particles/Modules/Color/SimpleColorModule.h"
#include "Core/GameCore/Particles/Modules/Lifetime/SimpleLifeTimeModule.h"
#include "Core/GameCore/Particles/Modules/Size/SimpleSizeModule.h"
#include "Core/GameCore/Particles/Modules/Velocity/ExplosionInitialVelocityModule.h"
#include "Core/GameCore/Particles/Modules/Velocity/OrbitVelocityModule.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionSphereShape.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Implementation/Actors/BlackHoleMissileActor.h"
#include "Implementation/Controllers/AiActorController.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t BlackHoleMissileFactory::s_blackHoleMissileCounter = 0;

std::shared_ptr<MissileActor> BlackHoleMissileFactory::CreateMissile(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale)
{
    const auto& missileIndexStr = std::to_string(s_blackHoleMissileCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_blackHoleMissile_rootComponent_" + missileIndexStr, translation, rotation, scale, true);
    const auto& a_missile = std::make_shared<BlackHoleMissileActor>(
        "a_blackHoleMissile_" + missileIndexStr, rootComponent, combatActorsPoolHandler);
    scene->AddActor(a_missile);

    TweenerParser tweenerParser;
    const auto& missileTweener = tweenerParser.ParseTweenerDescriptor("blackHoleMissile.tween");

    std::shared_ptr<Actor> a_missileCombatActivePhase;
    std::shared_ptr<MissileActor> a_missileExplosionSecondPhase;

    {
        a_missileCombatActivePhase = std::make_shared<Actor>(
            "a_missileCombatActivePhase_" + missileIndexStr,
            std::make_shared<EngineCore::SceneComponent>(
                "c_missileCombatActivePhase_rootComponent_" + missileIndexStr, glm::vec3(), glm::vec3(), glm::vec3(1), true));

        MaterialParser materialParser;
        const std::shared_ptr<IMaterial>& pbs_mat = materialParser.ParseMaterialDescriptor("MissileMaterial.m");
        scene->RegisterMaterialInstance(pbs_mat);

        const std::string albedoName = "missile1_albedo.png";
        const std::string normalName = "solar_cells_normal_512.jpg";
        const std::string roughnessName = "solar_cells_roughness_512.jpg";
        const std::string metallicName = "solar_cells_metallic_512.jpg";

        const auto& albedo_tex = TexturePool::GetInstance()->GetOrAllocateResource(albedoName);
        const auto& normal_tex = TexturePool::GetInstance()->GetOrAllocateResource(normalName);
        const auto& roughness_tex = TexturePool::GetInstance()->GetOrAllocateResource(roughnessName);
        const auto& metallic_tex = TexturePool::GetInstance()->GetOrAllocateResource(metallicName);
        const float uvScale = 0.5f;

        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "albedo", albedo_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "normalMap", normal_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "roughnessMap", roughness_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "metallicMap", metallic_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "uvScale", uvScale);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, scene->GetMainCamera(), "CameraPosition", "cameraPosition");
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "outlineColor", glm::vec3(0.647f, 0.215f, 0.99f));

        const auto d_mesh = std::make_shared<MeshComponentData>(
            "c_missileCombatActivePhaseMesh_" + missileIndexStr,
            "missile1_model.fbx",
            glm::vec3(0),
            glm::vec3(0),
            glm::vec3(1.5),
            pbs_mat);
        const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
        const auto& c_mesh = scene->CreateComponent_GameThread(meshComponentCreator, d_mesh);
        a_missileCombatActivePhase->AddComponent(c_mesh);

        const auto& ghostController
            = std::make_shared<GhostController>(scene->GetPhysicsWorld(), std::make_shared<CollisionSphereShape>(3.0f), 0.0f);
        const auto physData
            = std::make_shared<PhysicsComponentData>("c_missileCombatActivePhasePhysics_" + missileIndexStr, ghostController);
        const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
        const auto& c_ghostPhysics = scene->CreateComponent_GameThread(physicsComponentCreator, physData);
        a_missileCombatActivePhase->AddComponent(c_ghostPhysics);

        const auto d_movement = std::make_shared<MovementComponentData>(
            "c_missileCombatActivePhase_noPhysMoveComponent_" + missileIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
        const auto& moveComponentCreator = std::make_shared<MovementComponentCreator<NoPhysicsMovementComponent>>();
        const auto& c_movement = std::static_pointer_cast<NoPhysicsMovementComponent>(
            scene->CreateComponent_GameThread(moveComponentCreator, d_movement));
        c_movement->SetReferenceSpeed(80.0f);
        c_movement->SetCurrentSpeedToReferenceValue();
        c_movement->SetDirection(glm::vec3(.0f, .0f, 1.0f));
        a_missileCombatActivePhase->AddComponent(c_movement);

        const auto& bulletActorController = std::make_shared<AiActorController>(a_missileCombatActivePhase);
        scene->AddActorController(bulletActorController);
    }

    {
        a_missileExplosionSecondPhase = std::make_shared<MissileActor>(
            "a_missileExplosionSecondPhase" + missileIndexStr,
            std::make_shared<EngineCore::SceneComponent>(
                "c_missileExplosionSecondPhase_rootComponent_" + missileIndexStr, glm::vec3(), glm::vec3(), glm::vec3(1), true),
            combatActorsPoolHandler);

        MaterialParser materialParser;

        const std::shared_ptr<IMaterial>& missile_mat = materialParser.ParseMaterialDescriptor("BlackHoleMissileMaterial.m");
        scene->RegisterMaterialInstance(missile_mat);

        const std::string dudvTextureName = "water_dudv.jpg";
        const std::string albedoTextureName = "nightLeft.jpg";
        const auto& dudv_tex = TexturePool::GetInstance()->GetOrAllocateResource(dudvTextureName);
        const auto& albedo_tex = TexturePool::GetInstance()->GetOrAllocateResource(albedoTextureName);

        MaterialPropertySetter::SetMaterialPropertyValue(missile_mat, "mul_coef", 2.5f);
        MaterialPropertySetter::SetMaterialPropertyValue(missile_mat, scene, "GT_DeltaSec", "deltaTimeSec");
        MaterialPropertySetter::SetMaterialPropertyValue(missile_mat, "dudv", dudv_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(missile_mat, "albedo", albedo_tex);

        const auto d_mesh = std::make_shared<MeshComponentData>(
            "c_missileExplosionSecondPhase_meshComponent_" + missileIndexStr,
            "sphere.obj",
            glm::vec3(0),
            glm::vec3(0),
            glm::vec3(5),
            missile_mat);
        const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(false);
        const auto& c_mesh = scene->CreateComponent_GameThread(meshComponentCreator, d_mesh);
        a_missileExplosionSecondPhase->AddComponent(c_mesh);

        const auto d_movement = std::make_shared<MovementComponentData>(
            "c_missileExplosionSecondPhase_noPhysMoveComponent_" + missileIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
        const auto& moveComponentCreator = std::make_shared<MovementComponentCreator<NoPhysicsMovementComponent>>();
        const auto& c_movement = std::static_pointer_cast<NoPhysicsMovementComponent>(
            scene->CreateComponent_GameThread(moveComponentCreator, d_movement));
        c_movement->SetDirection(glm::vec3(.0f, .0f, 1.0f));
        c_movement->SetReferenceSpeed(80.0f);
        c_movement->SetCurrentSpeedToReferenceValue();
        a_missileExplosionSecondPhase->AddComponent(c_movement);

        const std::shared_ptr<IMaterial>& particles_mat = materialParser.ParseMaterialDescriptor("OpacityMaskParticleMaterial.m");
        scene->RegisterMaterialInstance(particles_mat);

        const auto& opacityMask_tex = TexturePool::GetInstance()->GetOrAllocateResource("circle_mask1.png");
        MaterialPropertySetter::SetMaterialPropertyValue(particles_mat, "opacityMask", opacityMask_tex);
        const auto d_particle = std::make_shared<ParticleSystemComponentData>(
            "c_missileExplosionSecondPhase_particleSystemComponent_" + missileIndexStr,
            particles_mat,
            glm::vec3(0),
            glm::vec3(1.0f),
            100);
        const auto& particleSystemComponentCreator = std::make_shared<ParticleSystemComponentCreator<ParticleSystemComponent>>();
        const auto& c_particleSystemComponent = std::static_pointer_cast<ParticleSystemComponent>(
            scene->CreateComponent_GameThread(particleSystemComponentCreator, d_particle));
        auto emitter = std::make_shared<ParticleExplosionEmitter>();
        emitter->SetOwner(c_particleSystemComponent);
        emitter->SetExplosionRadius(2.0f);
        emitter->SetThetaSlicesCount(10);
        c_particleSystemComponent->SetParticleEmitter(emitter);

        auto lifeTimeModule = std::make_shared<SimpleLifeTimeModule>();
        lifeTimeModule->SetOwner(c_particleSystemComponent);
        lifeTimeModule->SetLifeTime(10.0f);
        c_particleSystemComponent->AddParticleModule(lifeTimeModule);

        auto sizeModule = std::make_shared<SimpleSizeModule>();
        sizeModule->SetOwner(c_particleSystemComponent);
        sizeModule->SetSizeBegin(2.2f);
        sizeModule->SetSizeEnd(0.4f);
        c_particleSystemComponent->AddParticleModule(sizeModule);

        auto velocityModule = std::make_shared<OrbitVelocityModule>();
        velocityModule->SetOwner(c_particleSystemComponent);
        velocityModule->SetExtraVelocityDirectionType(eOrbitExtraVelocityDirectionType::Outside);
        velocityModule->SetExtraVelocityPower(0.1f);
        c_particleSystemComponent->AddParticleModule(velocityModule);

        auto colorModule = std::make_shared<SimpleColorModule>();
        colorModule->SetOwner(c_particleSystemComponent);
        colorModule->SetColorBegin(glm::vec4(0.5f, 0.0f, 0.6f, 1.0f));
        colorModule->SetColorEnd(glm::vec4(0.4f, 0.0f, 0.5f, 1.0f));
        c_particleSystemComponent->AddParticleModule(colorModule);

        a_missileExplosionSecondPhase->AddComponent(c_particleSystemComponent);

        const auto& ghostController
            = std::make_shared<GhostController>(scene->GetPhysicsWorld(), std::make_shared<CollisionSphereShape>(5.0f), 0.0f);
        const auto physData = std::make_shared<PhysicsComponentData>(
            "c_missileExplosionSecondPhase_PhysicsComponent_" + missileIndexStr, ghostController);
        const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
        const auto& c_ghostPhysics = scene->CreateComponent_GameThread(physicsComponentCreator, physData);
        a_missileExplosionSecondPhase->AddComponent(c_ghostPhysics);

        const auto& b_blackHoleHitBoxSize = missileTweener->GetPropertyBindingByName("b_blackHoleHitBoxSize");
        BindingAttachmentBuilder::SetAttachment(c_ghostPhysics, b_blackHoleHitBoxSize, "p_hitBoxScale");
    }

    a_missile->AddCombatActivePhaseActor(a_missileCombatActivePhase);
    a_missile->AddExplosionSecondPhaseActor(a_missileExplosionSecondPhase);

    const auto& soundComponentCreator = std::make_shared<AudioComponentCreator<SoundComponent>>();
    const auto& c_sound = std::static_pointer_cast<SoundComponent>(scene->CreateComponent_GameThread(
        soundComponentCreator, std::make_shared<ComponentData>("c_missileCombatActivePhaseSound_" + missileIndexStr)));
    c_sound->CreateSoundBuffer("explosion1.ogg", "explosion");
    c_sound->GetSoundSource()->SetGain(0.2f);
    a_missile->AddComponent(c_sound);

    a_missile->AttachTweener(missileTweener);

    const auto& b_activePhaseActorIsEnabled = missileTweener->GetPropertyBindingByName("b_activePhaseActorIsEnabled");
    const auto& b_missileExplosionSecondPhaseActorIsEnabled
        = missileTweener->GetPropertyBindingByName("b_missileExplosionSecondPhaseActorIsEnabled");
    const auto& b_blackHoleSize = missileTweener->GetPropertyBindingByName("b_blackHoleSize");

    BindingAttachmentBuilder::SetAttachment(a_missileCombatActivePhase, b_activePhaseActorIsEnabled, "p_isEnabled");
    BindingAttachmentBuilder::SetAttachment(
        a_missileExplosionSecondPhase, b_missileExplosionSecondPhaseActorIsEnabled, "p_isEnabled");
    BindingAttachmentBuilder::SetAttachment(a_missileExplosionSecondPhase->GetRootComponent(), b_blackHoleSize, "p_scale");

    a_missile->SetScene(scene);

    return a_missile;
}
} // namespace Game