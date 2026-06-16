#include "GravityBombMissileFactory.h"

#include "Core/AudioCore/SoundSource.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ParticleSystemComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/ParticleComponents/CpuParticleSystemComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/GravityGridComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/OrbitalRingComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Particles/Emitters/ParticleExplosionEmitter.h"
#include "Core/GameCore/Particles/Modules/Color/SimpleColorModule.h"
#include "Core/GameCore/Particles/Modules/Lifetime/SimpleLifeTimeModule.h"
#include "Core/GameCore/Particles/Modules/Size/SimpleSizeModule.h"
#include "Core/GameCore/Particles/Modules/Velocity/OrbitVelocityModule.h"
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
#include "Implementation/Actors/GravityBombMissileActor.h"
#include "Implementation/Controllers/AiActorController.h"
#include "Implementation/DataProviders/GameConstants.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t GravityBombMissileFactory::s_gravityBombMissileCounter = 0;

std::shared_ptr<MissileActor> GravityBombMissileFactory::CreateMissile(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale)
{
    using namespace Constants::GravityBombMissile;

    const auto& missileIndexStr = std::to_string(s_gravityBombMissileCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_gravityBombMissile_rootComponent_" + missileIndexStr, translation, rotation, scale, true);
    const auto& a_missile = std::make_shared<GravityBombMissileActor>(
        "a_gravityBombMissile_" + missileIndexStr, rootComponent, combatActorsPoolHandler);
    scene->AddActor(a_missile);

    TweenerParser tweenerParser;
    const auto& missileTweener = tweenerParser.ParseTweenerDescriptor("gravityBombMissile.tween");

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
        const float uvScale = c_combatUvScale;

        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "albedo", albedo_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "normalMap", normal_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "roughnessMap", roughness_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "metallicMap", metallic_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "uvScale", uvScale);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, scene->GetMainCamera(), "CameraPosition", "cameraPosition");
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "outlineColor", c_combatOutlineColor);

        const auto d_mesh = std::make_shared<MeshComponentData>(
            "c_missileCombatActivePhaseMesh_" + missileIndexStr,
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
        const auto physData
            = std::make_shared<PhysicsComponentData>("c_missileCombatActivePhasePhysics_" + missileIndexStr, ghostController);
        const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
        const auto& c_ghostPhysics
            = std::static_pointer_cast<PhysicsComponent>(scene->CreateComponent_GameThread(physicsComponentCreator, physData));
        sphereShape->SetParentPhysicsComponent(c_ghostPhysics);
        a_missileCombatActivePhase->AddComponent(c_ghostPhysics);

        const auto d_movement = std::make_shared<MovementComponentData>(
            "c_missileCombatActivePhase_noPhysMoveComponent_" + missileIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
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
            "a_missileExplosionSecondPhase" + missileIndexStr,
            std::make_shared<EngineCore::SceneComponent>(
                "c_missileExplosionSecondPhase_rootComponent_" + missileIndexStr, glm::vec3(), glm::vec3(), glm::vec3(1), true),
            combatActorsPoolHandler);

        MaterialParser materialParser;

        const std::shared_ptr<IMaterial>& missile_mat = materialParser.ParseMaterialDescriptor("GravityBombMissileMaterial.m");
        scene->RegisterMaterialInstance(missile_mat);

        const std::string dudvTextureName = "water_dudv.jpg";
        const std::string albedoTextureName = "nightLeft.jpg";
        const auto& dudv_tex = TexturePool::GetInstance()->GetOrAllocateResource(dudvTextureName);
        const auto& albedo_tex = TexturePool::GetInstance()->GetOrAllocateResource(albedoTextureName);

        MaterialPropertySetter::SetMaterialPropertyValue(missile_mat, "mul_coef", c_materialMulCoef);
        MaterialPropertySetter::SetMaterialPropertyValue(missile_mat, scene, "GT_DeltaSec", "deltaTimeSec");
        MaterialPropertySetter::SetMaterialPropertyValue(missile_mat, "dudv", dudv_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(missile_mat, "albedo", albedo_tex);

        // The visible singularity mesh is kept smaller than the gravity/collision radius so ships are pulled in and
        // visibly spaghettified in the open space around it before reaching the core.
        const auto d_mesh = std::make_shared<MeshComponentData>(
            "c_missileExplosionSecondPhase_meshComponent_" + missileIndexStr,
            "sphere.obj",
            glm::vec3(0),
            glm::vec3(0),
            glm::vec3(c_singularityMeshScale),
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
        c_movement->SetReferenceSpeed(c_singularitySpeed);
        c_movement->SetCurrentSpeedToReferenceValue();
        a_missileExplosionSecondPhase->AddComponent(c_movement);

        const std::shared_ptr<IMaterial>& particles_mat = materialParser.ParseMaterialDescriptor("OpacityMaskParticleMaterial.m");
        scene->RegisterMaterialInstance(particles_mat);
        const auto& opacityMask_tex = TexturePool::GetInstance()->GetOrAllocateResource("circle_mask1.png");
        MaterialPropertySetter::SetMaterialPropertyValue(particles_mat, "opacityMask", opacityMask_tex);

        const auto& particleSystemComponentCreator
            = std::make_shared<ParticleSystemComponentCreator<CpuParticleSystemComponent>>();

        // Accretion swirl: matter spiralling inward around the singularity. Emitted on impact, fades over the capture
        // window. This is the FIRST particle system on the actor (front()); the burst below is the second (back()).
        {
            const auto d_accretion = std::make_shared<ParticleSystemComponentData>(
                "c_gravityBombAccretionParticles_" + missileIndexStr,
                particles_mat,
                glm::vec3(0),
                glm::vec3(1.0f),
                c_accretionParticleCount,
                false);
            const auto& c_accretion = std::static_pointer_cast<CpuParticleSystemComponent>(
                scene->CreateComponent_GameThread(particleSystemComponentCreator, d_accretion));

            auto emitter = std::make_shared<ParticleExplosionEmitter>();
            emitter->SetOwner(c_accretion);
            emitter->SetExplosionRadius(c_accretionEmitRadius);
            emitter->SetThetaSlicesCount(c_accretionThetaSlices);
            c_accretion->SetParticleEmitter(emitter);

            auto lifeTimeModule = std::make_shared<SimpleLifeTimeModule>();
            lifeTimeModule->SetOwner(c_accretion);
            lifeTimeModule->SetLifeTime(c_accretionLifeTime);
            c_accretion->AddParticleModule(lifeTimeModule);

            auto sizeModule = std::make_shared<SimpleSizeModule>();
            sizeModule->SetOwner(c_accretion);
            sizeModule->SetSizeBegin(c_accretionSizeBegin);
            sizeModule->SetSizeEnd(c_accretionSizeEnd);
            c_accretion->AddParticleModule(sizeModule);

            auto velocityModule = std::make_shared<OrbitVelocityModule>();
            velocityModule->SetOwner(c_accretion);
            velocityModule->SetExtraVelocityDirectionType(eOrbitExtraVelocityDirectionType::Inside);
            velocityModule->SetSpeed(c_accretionInwardSpeed);
            c_accretion->AddParticleModule(velocityModule);

            auto colorModule = std::make_shared<SimpleColorModule>();
            colorModule->SetOwner(c_accretion);
            colorModule->SetColorBegin(c_accretionColorBegin);
            colorModule->SetColorEnd(c_accretionColorEnd);
            c_accretion->AddParticleModule(colorModule);

            a_missileExplosionSecondPhase->AddComponent(c_accretion);
        }

        // Collapse burst: a radial shockwave of bright sparks fired when the singularity collapses.
        {
            const auto d_burst = std::make_shared<ParticleSystemComponentData>(
                "c_gravityBombCollapseBurst_" + missileIndexStr,
                particles_mat,
                glm::vec3(0),
                glm::vec3(1.0f),
                c_burstParticleCount,
                false);
            const auto& c_burst = std::static_pointer_cast<CpuParticleSystemComponent>(
                scene->CreateComponent_GameThread(particleSystemComponentCreator, d_burst));

            auto emitter = std::make_shared<ParticleExplosionEmitter>();
            emitter->SetOwner(c_burst);
            emitter->SetExplosionRadius(c_burstEmitRadius);
            emitter->SetThetaSlicesCount(c_burstThetaSlices);
            c_burst->SetParticleEmitter(emitter);

            auto lifeTimeModule = std::make_shared<SimpleLifeTimeModule>();
            lifeTimeModule->SetOwner(c_burst);
            lifeTimeModule->SetLifeTime(c_burstLifeTime);
            c_burst->AddParticleModule(lifeTimeModule);

            auto sizeModule = std::make_shared<SimpleSizeModule>();
            sizeModule->SetOwner(c_burst);
            sizeModule->SetSizeBegin(c_burstSizeBegin);
            sizeModule->SetSizeEnd(c_burstSizeEnd);
            c_burst->AddParticleModule(sizeModule);

            auto velocityModule = std::make_shared<RadialVelocityModule>();
            velocityModule->SetOwner(c_burst);
            velocityModule->SetSpeed(c_burstRadialSpeed);
            c_burst->AddParticleModule(velocityModule);

            auto colorModule = std::make_shared<SimpleColorModule>();
            colorModule->SetOwner(c_burst);
            colorModule->SetColorBegin(c_burstColorBegin);
            colorModule->SetColorEnd(c_burstColorEnd);
            c_burst->AddParticleModule(colorModule);

            a_missileExplosionSecondPhase->AddComponent(c_burst);
        }

        const auto& ghostController
            = std::make_shared<GhostController>(
                scene->GetPhysicsWorld(), std::make_shared<CollisionSphereShape>(c_hitboxColliderRadius), 0.0f);
        const auto physData = std::make_shared<PhysicsComponentData>(
            "c_missileExplosionSecondPhase_PhysicsComponent_" + missileIndexStr, ghostController);
        const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
        const auto& c_ghostPhysics = scene->CreateComponent_GameThread(physicsComponentCreator, physData);
        a_missileExplosionSecondPhase->AddComponent(c_ghostPhysics);

        const auto& b_gravityBombHitBoxSize = missileTweener->GetPropertyBindingByName("b_gravityBombHitBoxSize");
        BindingAttachmentBuilder::SetAttachment(c_ghostPhysics, b_gravityBombHitBoxSize, "p_hitBoxScale");
    }

    // Two orbital rings tumbling in different planes around the singularity. They are components of the explosion
    // phase actor, so they inherit its enable/disable from the tweener and appear/disappear with the singularity.
    {
        MaterialParser ringMaterialParser;
        const auto createOrbitalRing = [&](const std::string& namePrefix,
                                           const glm::vec3& tumbleAxis,
                                           const float radius,
                                           const float spinSpeedDegPerSec,
                                           const glm::vec3& color,
                                           const glm::vec3& glowColor) {
            const std::shared_ptr<IMaterial>& ring_mat
                = ringMaterialParser.ParseMaterialDescriptor("GravityBombEnergyLineMaterial.m");
            scene->RegisterMaterialInstance(ring_mat);
            MaterialPropertySetter::SetMaterialPropertyValue(ring_mat, "opacity", c_ringOpacity);
            MaterialPropertySetter::SetMaterialPropertyValue(ring_mat, "color", color);
            MaterialPropertySetter::SetMaterialPropertyValue(ring_mat, "glowColor", glowColor);
            MaterialPropertySetter::SetMaterialPropertyValue(ring_mat, scene, "GT_DeltaSec", "gt_timeSec");

            const auto d_ring = std::make_shared<RuntimeGeneratedMeshComponentData>(
                namePrefix + missileIndexStr,
                c_ringMaxVertices,
                glm::vec3(0),
                glm::vec3(0),
                glm::vec3(1),
                ring_mat,
                true,
                true);
            const auto& ringComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<OrbitalRingComponent>>();
            const auto& c_ring = std::static_pointer_cast<OrbitalRingComponent>(
                scene->CreateComponent_GameThread(ringComponentCreator, d_ring));
            c_ring->SetCanBloomBeApplied(true);
            // Radius/width are in the singularity actor's local space; the bound root scale (b_gravityBombSize, 0 -> 5)
            // scales the rings together with the sphere, so they grow and shrink with it.
            c_ring->SetLineWidth(c_ringLineWidth);
            c_ring->SetRingRadius(radius);
            c_ring->SetRingTumbleAxis(tumbleAxis);
            c_ring->SetRingSpinSpeedDegPerSec(spinSpeedDegPerSec);
            c_ring->SetRingSegmentsCount(c_ringSegments);
            c_ring->SetCanBloomBeApplied(true);
            a_missileExplosionSecondPhase->AddComponent(c_ring);
        };

        createOrbitalRing(
            "c_gravityBombRingA_",
            c_ringA_TumbleAxis,
            c_ringA_Radius,
            c_ringA_SpinSpeed,
            c_ringA_Color,
            c_ringA_GlowColor);
        createOrbitalRing(
            "c_gravityBombRingB_",
            c_ringB_TumbleAxis,
            c_ringB_Radius,
            c_ringB_SpinSpeed,
            c_ringB_Color,
            c_ringB_GlowColor);
    }

    // Host actor (kept at the world origin with an identity transform) for the gravity tether curves, plus their shared
    // material. Tethers span from the singularity to moving ships, so their geometry is world-space and must not inherit
    // any parent transform.
    {
        const auto a_tetherHost = std::make_shared<Actor>(
            "a_gravityBombTetherHost_" + missileIndexStr,
            std::make_shared<EngineCore::SceneComponent>(
                "c_gravityBombTetherHost_rootComponent_" + missileIndexStr, glm::vec3(), glm::vec3(), glm::vec3(1), true));
        scene->AddActor(a_tetherHost);

        MaterialParser tetherMaterialParser;
        const std::shared_ptr<IMaterial>& tether_mat
            = tetherMaterialParser.ParseMaterialDescriptor("GravityBombEnergyLineMaterial.m");
        scene->RegisterMaterialInstance(tether_mat);
        MaterialPropertySetter::SetMaterialPropertyValue(tether_mat, "opacity", c_tetherOpacity);
        MaterialPropertySetter::SetMaterialPropertyValue(tether_mat, "color", c_tetherColor);
        MaterialPropertySetter::SetMaterialPropertyValue(tether_mat, "glowColor", c_tetherGlowColor);
        MaterialPropertySetter::SetMaterialPropertyValue(tether_mat, scene, "GT_DeltaSec", "gt_timeSec");

        a_missile->SetTetherHostActor(a_tetherHost);
        a_missile->SetTetherMaterial(tether_mat);

        // Gravity-well grid on the same identity host (world-space geometry). Starts disabled; the missile actor enables
        // it and anchors it to the singularity on impact, and disables it when the black hole ends.
        const std::shared_ptr<IMaterial>& grid_mat = tetherMaterialParser.ParseMaterialDescriptor("GravityBombEnergyLineMaterial.m");
        scene->RegisterMaterialInstance(grid_mat);
        MaterialPropertySetter::SetMaterialPropertyValue(grid_mat, "opacity", c_gridOpacity);
        MaterialPropertySetter::SetMaterialPropertyValue(grid_mat, "color", c_gridColor);
        MaterialPropertySetter::SetMaterialPropertyValue(grid_mat, "glowColor", c_gridGlowColor);
        MaterialPropertySetter::SetMaterialPropertyValue(grid_mat, scene, "GT_DeltaSec", "gt_timeSec");

        const auto d_grid = std::make_shared<RuntimeGeneratedMeshComponentData>(
            "c_gravityBombGrid_" + missileIndexStr,
            c_gridMaxVertices,
            glm::vec3(0),
            glm::vec3(0),
            glm::vec3(1),
            grid_mat,
            false,
            true);
        const auto& gridComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<GravityGridComponent>>();
        const auto& c_grid
            = std::static_pointer_cast<GravityGridComponent>(scene->CreateComponent_GameThread(gridComponentCreator, d_grid));
        c_grid->SetCanBloomBeApplied(true);
        c_grid->SetLineWidth(c_gridLineWidth);
        c_grid->SetGridHalfExtent(c_gridHalfExtent);
        c_grid->SetGridLineCount(c_gridLineCount);
        c_grid->SetSegmentsPerLine(c_gridSegments);
        c_grid->SetDeformStrength(c_gridDeformStrength);
        c_grid->SetDipStrength(c_gridDipStrength);
        c_grid->SetSoftening(c_gridSoftening);
        a_tetherHost->AddComponent(c_grid);
    }

    a_missile->AddCombatActivePhaseActor(a_missileCombatActivePhase);
    a_missile->AddExplosionSecondPhaseActor(a_missileExplosionSecondPhase);

    const auto& soundComponentCreator = std::make_shared<AudioComponentCreator<SoundComponent>>();
    const auto& c_sound = std::static_pointer_cast<SoundComponent>(scene->CreateComponent_GameThread(
        soundComponentCreator, std::make_shared<ComponentData>("c_missileCombatActivePhaseSound_" + missileIndexStr)));
    c_sound->CreateSoundBuffer("explosion1.ogg", "explosion");
    c_sound->GetSoundSource()->SetGain(c_soundGain);
    a_missile->AddComponent(c_sound);

    a_missile->AttachTweener(missileTweener);

    const auto& b_activePhaseActorIsEnabled = missileTweener->GetPropertyBindingByName("b_activePhaseActorIsEnabled");
    const auto& b_missileExplosionSecondPhaseActorIsEnabled
        = missileTweener->GetPropertyBindingByName("b_missileExplosionSecondPhaseActorIsEnabled");
    const auto& b_gravityBombSize = missileTweener->GetPropertyBindingByName("b_gravityBombSize");

    BindingAttachmentBuilder::SetAttachment(a_missileCombatActivePhase, b_activePhaseActorIsEnabled, "p_isEnabled");
    BindingAttachmentBuilder::SetAttachment(
        a_missileExplosionSecondPhase, b_missileExplosionSecondPhaseActorIsEnabled, "p_isEnabled");
    BindingAttachmentBuilder::SetAttachment(a_missileExplosionSecondPhase->GetRootComponent(), b_gravityBombSize, "p_scale");

    a_missile->SetScene(scene);

    return a_missile;
}
} // namespace Game