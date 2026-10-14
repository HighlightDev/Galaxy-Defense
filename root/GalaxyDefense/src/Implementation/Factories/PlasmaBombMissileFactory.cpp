#include "PlasmaBombMissileFactory.h"

#include "Core/AudioCore/SoundSource.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ParticleSystemComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/ParticleComponents/CpuParticleSystemComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/PlasmaTrailComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Particles/Emitters/ParticleExplosionEmitter.h"
#include "Core/GameCore/Particles/Modules/Color/SimpleColorModule.h"
#include "Core/GameCore/Particles/Modules/Lifetime/SimpleLifeTimeModule.h"
#include "Core/GameCore/Particles/Modules/Size/SimpleSizeModule.h"
#include "Core/GameCore/Particles/Modules/Velocity/RadialVelocityModule.h"
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
#include "Implementation/Actors/PlasmaBombMissileActor.h"
#include "Implementation/Controllers/AiActorController.h"
#include "Implementation/DataProviders/GameConstants.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t PlasmaBombMissileFactory::s_plasmaBombMissileCounter = 0;

std::shared_ptr<MissileActor> PlasmaBombMissileFactory::CreateMissile(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale)
{
    using namespace Constants::PlasmaBombMissile;

    const auto& missileIndexStr = std::to_string(s_plasmaBombMissileCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_plasmaBombMissile_rootComponent_" + missileIndexStr, translation, rotation, scale, true);
    const auto& a_missile = std::make_shared<PlasmaBombMissileActor>(
        "a_plasmaBombMissile_" + missileIndexStr, rootComponent, combatActorsPoolHandler);
    scene->AddActor(a_missile);

    TweenerParser tweenerParser;
    const auto& missileTweener = tweenerParser.ParseTweenerDescriptor("plasmaBombMissile.tween");

    std::shared_ptr<Actor> a_missileCombatActivePhase;
    std::shared_ptr<MissileActor> a_missileExplosionSecondPhase;

    {
        a_missileCombatActivePhase = std::make_shared<Actor>(
            "a_plasmaMissileCombatActivePhase_" + missileIndexStr,
            std::make_shared<EngineCore::SceneComponent>(
                "c_plasmaMissileCombatActivePhase_rootComponent_" + missileIndexStr,
                glm::vec3(),
                glm::vec3(),
                glm::vec3(1),
                true));

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

        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "albedo", albedo_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "normalMap", normal_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "roughnessMap", roughness_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "metallicMap", metallic_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "uvScale", c_combatUvScale);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, scene->GetMainCamera(), "CameraPosition", "cameraPosition");
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "outlineColor", c_combatOutlineColor);

        const auto d_mesh = std::make_shared<MeshComponentData>(
            "c_plasmaMissileCombatActivePhaseMesh_" + missileIndexStr,
            "missile1_model.fbx",
            glm::vec3(0),
            glm::vec3(0),
            glm::vec3(c_combatMeshScale),
            pbs_mat);
        const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
        const auto& c_mesh = scene->CreateComponent_GameThread(meshComponentCreator, d_mesh);
        a_missileCombatActivePhase->AddComponent(c_mesh);

        const auto& sphereShape = std::make_shared<CollisionSphereShape>(c_combatColliderRadius);
        const auto& ghostController = std::make_shared<GhostController>(scene->GetPhysicsWorld(), sphereShape, 0.0f);
        const auto physData = std::make_shared<PhysicsComponentData>(
            "c_plasmaMissileCombatActivePhasePhysics_" + missileIndexStr, ghostController);
        const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
        const auto& c_ghostPhysics
            = std::static_pointer_cast<PhysicsComponent>(scene->CreateComponent_GameThread(physicsComponentCreator, physData));
        sphereShape->SetParentPhysicsComponent(c_ghostPhysics);
        a_missileCombatActivePhase->AddComponent(c_ghostPhysics);

        const auto d_movement = std::make_shared<MovementComponentData>(
            "c_plasmaMissileCombatActivePhase_noPhysMoveComponent_" + missileIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
        const auto& moveComponentCreator = std::make_shared<MovementComponentCreator<NoPhysicsMovementComponent>>();
        const auto& c_movement = std::static_pointer_cast<NoPhysicsMovementComponent>(
            scene->CreateComponent_GameThread(moveComponentCreator, d_movement));
        c_movement->SetReferenceSpeed(c_combatSpeed);
        c_movement->SetCurrentSpeedToReferenceValue();
        c_movement->SetDirection(glm::vec3(.0f, .0f, 1.0f));
        a_missileCombatActivePhase->AddComponent(c_movement);

        const auto& bulletActorController = std::make_shared<AiActorController>(a_missileCombatActivePhase);
        scene->AddActorController(bulletActorController);
    }

    {
        a_missileExplosionSecondPhase = std::make_shared<MissileActor>(
            "a_plasmaMissileExplosionSecondPhase" + missileIndexStr,
            std::make_shared<EngineCore::SceneComponent>(
                "c_plasmaMissileExplosionSecondPhase_rootComponent_" + missileIndexStr,
                glm::vec3(),
                glm::vec3(),
                glm::vec3(1),
                true),
            combatActorsPoolHandler);

        MaterialParser materialParser;

        // The explosion actor carries a small semi-transparent plasma ball that pops at the impact point: no field, no
        // collider. The projectile vanishes on impact and this swirling, glowing sphere blooms briefly where it struck.
        // It is forward/non-deferred (creator(false)) so the forward pass blends its alpha for the transparency.
        const std::shared_ptr<IMaterial>& sphere_mat = materialParser.ParseMaterialDescriptor("PlasmaCoreMaterial.m");
        scene->RegisterMaterialInstance(sphere_mat);

        const auto& dudv_tex = TexturePool::GetInstance()->GetOrAllocateResource("water_dudv.jpg");
        const auto& albedo_tex = TexturePool::GetInstance()->GetOrAllocateResource("nightLeft.jpg");
        MaterialPropertySetter::SetMaterialPropertyValue(sphere_mat, "mul_coef", c_materialMulCoef);
        MaterialPropertySetter::SetMaterialPropertyValue(sphere_mat, scene, "GT_DeltaSec", "deltaTimeSec");
        MaterialPropertySetter::SetMaterialPropertyValue(sphere_mat, "dudv", dudv_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(sphere_mat, "albedo", albedo_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(sphere_mat, "tintColor", c_coreTintColor);
        MaterialPropertySetter::SetMaterialPropertyValue(sphere_mat, "glowColor", c_coreGlowColor);
        MaterialPropertySetter::SetMaterialPropertyValue(sphere_mat, "opacity", c_sphereOpacity);

        const auto d_sphere = std::make_shared<MeshComponentData>(
            "c_plasmaSphere_" + missileIndexStr,
            "sphere.obj",
            glm::vec3(0),
            glm::vec3(0),
            glm::vec3(c_coreMeshScale),
            sphere_mat);
        const auto& sphereMeshCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(false);
        const auto& c_sphere
            = std::static_pointer_cast<StaticMeshComponent>(scene->CreateComponent_GameThread(sphereMeshCreator, d_sphere));
        c_sphere->SetCanBloomBeApplied(true);
        a_missileExplosionSecondPhase->AddComponent(c_sphere);

        // Scorch burst: a short radial spray of sparks fired from the impact point as the plasma sears the target.
        // Lives on the explosion actor (so it inherits the impact position) and is emitted on s_Impact by the actor.
        const std::shared_ptr<IMaterial>& burn_mat = materialParser.ParseMaterialDescriptor("OpacityMaskParticleMaterial.m");
        scene->RegisterMaterialInstance(burn_mat);
        const auto& opacityMask_tex = TexturePool::GetInstance()->GetOrAllocateResource("circle_mask1.png");
        MaterialPropertySetter::SetMaterialPropertyValue(burn_mat, "opacityMask", opacityMask_tex);

        const auto& particleSystemComponentCreator
            = std::make_shared<ParticleSystemComponentCreator<CpuParticleSystemComponent>>();
        const auto d_burn = std::make_shared<ParticleSystemComponentData>(
            "c_plasmaBombScorchBurst_" + missileIndexStr, burn_mat, glm::vec3(0), glm::vec3(1.0f), c_burnParticleCount, false);
        const auto& c_burn = std::static_pointer_cast<CpuParticleSystemComponent>(
            scene->CreateComponent_GameThread(particleSystemComponentCreator, d_burn));

        auto burnEmitter = std::make_shared<ParticleExplosionEmitter>();
        burnEmitter->SetOwner(c_burn);
        burnEmitter->SetExplosionRadius(c_burnEmitRadius);
        burnEmitter->SetThetaSlicesCount(c_burnThetaSlices);
        c_burn->SetParticleEmitter(burnEmitter);

        auto burnLifeTimeModule = std::make_shared<SimpleLifeTimeModule>();
        burnLifeTimeModule->SetOwner(c_burn);
        burnLifeTimeModule->SetLifeTime(c_burnLifeTime);
        c_burn->AddParticleModule(burnLifeTimeModule);

        auto burnSizeModule = std::make_shared<SimpleSizeModule>();
        burnSizeModule->SetOwner(c_burn);
        burnSizeModule->SetSizeBegin(c_burnSizeBegin);
        burnSizeModule->SetSizeEnd(c_burnSizeEnd);
        c_burn->AddParticleModule(burnSizeModule);

        auto burnVelocityModule = std::make_shared<RadialVelocityModule>();
        burnVelocityModule->SetOwner(c_burn);
        burnVelocityModule->SetSpeed(c_burnRadialSpeed);
        c_burn->AddParticleModule(burnVelocityModule);

        auto burnColorModule = std::make_shared<SimpleColorModule>();
        burnColorModule->SetOwner(c_burn);
        burnColorModule->SetColorBegin(c_burnColorBegin);
        burnColorModule->SetColorEnd(c_burnColorEnd);
        c_burn->AddParticleModule(burnColorModule);

        c_burn->SetCanBloomBeApplied(true);
        a_missileExplosionSecondPhase->AddComponent(c_burn);
        a_missile->SetBurnParticleComponent(c_burn);
    }

    // World-space flight trail. It records the flying body's world-space path, so it must not inherit any parent
    // transform: it hangs off the missile actor's root, which stays at an identity transform for the missile's whole
    // life (only the combat-phase child moves), so no dedicated host actor is needed.
    {
        MaterialParser trailMaterialParser;

        // Tapered plasma trail tail following the flying missile. The missile actor resets/enables it on spawn, then
        // freezes recording and fades it out on impact.
        const std::shared_ptr<IMaterial>& trail_mat = trailMaterialParser.ParseMaterialDescriptor("PlasmaTrailMaterial.m");
        scene->RegisterMaterialInstance(trail_mat);
        MaterialPropertySetter::SetMaterialPropertyValue(trail_mat, "opacity", c_trailOpacity);
        MaterialPropertySetter::SetMaterialPropertyValue(trail_mat, "color", c_trailColor);
        MaterialPropertySetter::SetMaterialPropertyValue(trail_mat, "glowColor", c_trailGlowColor);
        MaterialPropertySetter::SetMaterialPropertyValue(trail_mat, scene, "GT_DeltaSec", "gt_timeSec");

        const auto d_trail = std::make_shared<RuntimeGeneratedMeshComponentData>(
            "c_plasmaBombTrail_" + missileIndexStr,
            c_trailMaxVertices,
            glm::vec3(0),
            glm::vec3(0),
            glm::vec3(1),
            trail_mat,
            false,
            true);
        const auto& trailComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<PlasmaTrailComponent>>();
        const auto& c_trail
            = std::static_pointer_cast<PlasmaTrailComponent>(scene->CreateComponent_GameThread(trailComponentCreator, d_trail));
        c_trail->SetCanBloomBeApplied(true);
        c_trail->SetLineWidth(c_trailLineWidth);
        c_trail->SetMaxPoints(c_trailMaxPoints);
        c_trail->SetMinPointSpacing(c_trailMinPointSpacing);
        c_trail->SetSourceComponent(a_missileCombatActivePhase->GetRootComponent());
        c_trail->SetIsRecording(false); // pooled/idle: don't record until the missile is spawned
        c_trail->SetIsEnabled(false);
        a_missile->AddComponent(c_trail);
        a_missile->SetTrailComponent(c_trail);
    }

    a_missile->AddCombatActivePhaseActor(a_missileCombatActivePhase);
    a_missile->AddExplosionSecondPhaseActor(a_missileExplosionSecondPhase);

    const auto& soundComponentCreator = std::make_shared<AudioComponentCreator<SoundComponent>>();
    const auto& c_sound = std::static_pointer_cast<SoundComponent>(scene->CreateComponent_GameThread(
        soundComponentCreator, std::make_shared<ComponentData>("c_plasmaMissileCombatActivePhaseSound_" + missileIndexStr)));
    c_sound->CreateSoundBuffer("explosion1.ogg", "explosion");
    c_sound->GetSoundSource()->SetGain(c_soundGain);
    a_missile->AddComponent(c_sound);

    a_missile->AttachTweener(missileTweener);

    const auto& b_plasmaActivePhaseEnabled = missileTweener->GetPropertyBindingByName("b_plasmaActivePhaseEnabled");
    const auto& b_plasmaBurstEnabled = missileTweener->GetPropertyBindingByName("b_plasmaBurstEnabled");
    const auto& b_plasmaSize = missileTweener->GetPropertyBindingByName("b_plasmaSize");

    BindingAttachmentBuilder::SetAttachment(a_missileCombatActivePhase, b_plasmaActivePhaseEnabled, "p_isEnabled");
    BindingAttachmentBuilder::SetAttachment(a_missileExplosionSecondPhase, b_plasmaBurstEnabled, "p_isEnabled");
    BindingAttachmentBuilder::SetAttachment(a_missileExplosionSecondPhase->GetRootComponent(), b_plasmaSize, "p_scale");

    a_missile->SetScene(scene);

    return a_missile;
}
} // namespace Game
