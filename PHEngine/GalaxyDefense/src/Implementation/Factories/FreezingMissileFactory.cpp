#include "FreezingMissileFactory.h"

#include "Core/AudioCore/SoundSource.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ParticleSystemComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/ParticleComponents/GpuParticleSystemComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Particles/Emitters/ParticleExplosionEmitter.h"
#include "Core/GameCore/Particles/Modules/Color/SimpleColorModule.h"
#include "Core/GameCore/Particles/Modules/Lifetime/SimpleLifeTimeModule.h"
#include "Core/GameCore/Particles/Modules/Size/SimpleSizeModule.h"
#include "Core/GameCore/Particles/Modules/Velocity/SimpleVelocityModule.h"
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
#include "Implementation/Actors/FreezingMissileActor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Controllers/AiActorController.h"
#include "Implementation/DataProviders/GameConstants.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t FreezingMissileFactory::s_bulletCounter = 0;

std::shared_ptr<MissileActor> FreezingMissileFactory::CreateMissile(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale)
{
    using namespace Constants::FreezingMissile;

    const auto& shipBulletIndexStr = std::to_string(s_bulletCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_freezingMissile_rootComponent_" + shipBulletIndexStr, translation, rotation, scale, true);
    const auto& a_missile = std::make_shared<FreezingMissileActor>(
        "a_freezingMissile_" + shipBulletIndexStr, rootComponent, combatActorsPoolHandler);
    scene->AddActor(a_missile);

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
    const float uvScale = c_uvScale;

    MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "albedo", albedo_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "normalMap", normal_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "roughnessMap", roughness_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "metallicMap", metallic_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "uvScale", uvScale);
    MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, scene->GetMainCamera(), "CameraPosition", "cameraPosition");
    MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "outlineColor", c_outlineColor);

    const auto d_mesh = std::make_shared<MeshComponentData>(
        "c_freezingMissileMesh_" + shipBulletIndexStr,
        "missile1_model.fbx",
        glm::vec3(0),
        glm::vec3(0),
        glm::vec3(c_meshScale),
        pbs_mat);
    const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
    const auto& c_mesh = scene->CreateComponent_GameThread(meshComponentCreator, d_mesh);
    a_missile->AddComponent(c_mesh);

    const std::shared_ptr<IMaterial>& particles_mat = materialParser.ParseMaterialDescriptor("OpaqueParticleMaterial.m");
    scene->RegisterMaterialInstance(particles_mat);
    MaterialPropertySetter::SetMaterialPropertyValue(particles_mat, "opacity", c_particleOpacity);
    MaterialPropertySetter::SetMaterialPropertyValue(particles_mat, "clipRadius", c_particleClipRadius);

    const auto d_particle = std::make_shared<ParticleSystemComponentData>(
        "c_freezeParticleSystemComponent_" + shipBulletIndexStr, particles_mat, glm::vec3(0), glm::vec3(1.0f), c_particleCount, false);
    d_particle->emitterData = std::make_shared<ParticleEmitterData>();
    d_particle->emitterData->emitterType = "explosion";
    d_particle->emitterData->radius = c_emitRadius;
    d_particle->emitterData->thetaSlicesCount = c_thetaSlices;

    d_particle->velocityModules.push_back(std::make_shared<VelocityModuleData>());
    d_particle->velocityModules.back()->moduleType = "simple";
    d_particle->velocityModules.back()->velocityDirection = c_velocityDirection;
    d_particle->velocityModules.back()->velocityDeviation = glm::vec3(0.0f, 0.0f, 0.0f);
    d_particle->velocityModules.back()->speed = c_velocitySpeed;

    d_particle->colorData = std::make_shared<ColorModuleData>();
    d_particle->colorData->moduleType = "simple";
    d_particle->colorData->colorBegin = c_colorBegin;
    d_particle->colorData->colorEnd = c_colorEnd;

    d_particle->lifeTimeData = std::make_shared<LifeTimeModuleData>();
    d_particle->lifeTimeData->moduleType = "simple";
    d_particle->lifeTimeData->lifeTime = c_lifeTime;

    d_particle->sizeData = std::make_shared<SizeModuleData>();
    d_particle->sizeData->moduleType = "simple";
    d_particle->sizeData->sizeBegin = c_sizeBegin;
    d_particle->sizeData->sizeEnd = c_sizeEnd;

    const auto& particleSystemComponentCreator = std::make_shared<ParticleSystemComponentCreator<GpuParticleSystemComponent>>();
    const auto& c_particleSystemComponent = std::static_pointer_cast<GpuParticleSystemComponent>(
        scene->CreateComponent_GameThread(particleSystemComponentCreator, d_particle));

    a_missile->AddComponent(c_particleSystemComponent);

    const auto d_movement = std::make_shared<MovementComponentData>(
        "c_freezingMissileNoPhysMove_" + shipBulletIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
    const auto& moveComponentCreator = std::make_shared<MovementComponentCreator<NoPhysicsMovementComponent>>();
    const auto& c_movement = std::static_pointer_cast<NoPhysicsMovementComponent>(
        scene->CreateComponent_GameThread(moveComponentCreator, d_movement));
    c_movement->SetReferenceSpeed(c_speed);
    c_movement->SetCurrentSpeedToReferenceValue();
    c_movement->SetDirection(glm::vec3(.0f, .0f, 1.0f));
    a_missile->AddComponent(c_movement);

    const auto& soundComponentCreator = std::make_shared<AudioComponentCreator<SoundComponent>>();
    const auto& c_sound = std::static_pointer_cast<SoundComponent>(scene->CreateComponent_GameThread(
        soundComponentCreator, std::make_shared<ComponentData>("c_freezingMissileSound_" + shipBulletIndexStr)));
    c_sound->CreateSoundBuffer("explosion1.ogg", "explosion");
    c_sound->GetSoundSource()->SetGain(c_soundGain);
    a_missile->AddComponent(c_sound);

    const auto& sphereShape = std::make_shared<CollisionSphereShape>(c_colliderRadius);
    const auto& ghostController
        = std::make_shared<GhostController>(scene->GetPhysicsWorld(), std::make_shared<CollisionSphereShape>(c_colliderRadius), 0.0f);
    const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
    const auto& c_ghostPhysics = std::static_pointer_cast<PhysicsComponent>(scene->CreateComponent_GameThread(
        physicsComponentCreator,
        std::make_shared<PhysicsComponentData>("c_freezingMissilePhysics_" + shipBulletIndexStr, ghostController)));
    sphereShape->SetParentPhysicsComponent(c_ghostPhysics);
    a_missile->AddComponent(c_ghostPhysics);

    const auto& bulletActorController = std::make_shared<AiActorController>(a_missile);
    scene->AddActorController(bulletActorController);

    a_missile->SetScene(scene);

    return a_missile;
}
} // namespace Game