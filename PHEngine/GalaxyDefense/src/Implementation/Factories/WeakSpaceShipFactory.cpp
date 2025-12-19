#include "WeakSpaceShipFactory.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/InstancedStaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/LightComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ParticleSystemComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/UiComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/ComponentData/PointLightComponentData.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PointLightComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/InstancedStaticMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/Particles/Emitters/ParticleExplosionEmitter.h"
#include "Core/GameCore/Particles/Modules/Color/SimpleColorModule.h"
#include "Core/GameCore/Particles/Modules/Lifetime/SimpleLifeTimeModule.h"
#include "Core/GameCore/Particles/Modules/Size/SimpleSizeModule.h"
#include "Core/GameCore/Particles/Modules/Velocity/ExplosionInitialVelocityModule.h"
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
#include "Implementation/ActorLeveling/SpaceshipLevel.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Actors/WeakSpaceshipActor.h"
#include "Implementation/Components/ComponentData/SpaceObjectUiComponentData.h"
#include "Implementation/Components/MovementComponents/OnRouteMovementComponent.h"
#include "Implementation/Components/UiComponents/SpaceObjectUiComponent.h"
#include "Implementation/Controllers/AiSpaceshipActorController.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t WeakSpaceShipFactory::s_weakSpaceShipCounter = 0;

std::shared_ptr<SpaceshipActor> WeakSpaceShipFactory::CreateSpaceShip(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    const int32_t textFontSize)
{
    const uint32_t health = static_cast<uint32_t>(Random::Float() * 10) + 20;

    const auto& enemyShipIndexStr = std::to_string(s_weakSpaceShipCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_enemyShip_rootComponent_" + enemyShipIndexStr, translation, rotation, scale, true);
    const auto& a_enemySpaceship
        = std::make_shared<WeakSpaceshipActor>("a_enemyShip_" + enemyShipIndexStr, rootComponent, SpaceshipLevel(health));
    scene->AddActor(a_enemySpaceship);

    bool bAlreadyExists = false;
    const std::shared_ptr<IMaterial>& spaceshipPbs_mat = GetMaterial(scene, bAlreadyExists);
    if (!bAlreadyExists) {
        const auto& albedo_ice_tex = TexturePool::GetInstance()->GetOrAllocateResource("Ice_Cracked_albedo.jpg");
        const auto& normal_ice_tex = TexturePool::GetInstance()->GetOrAllocateResource("Ice_Cracked_normal.jpg");
        const auto& roughness_ice_tex = TexturePool::GetInstance()->GetOrAllocateResource("Ice_Cracked_roughness.jpg");
        const auto& metallic_ice_tex = TexturePool::GetInstance()->GetOrAllocateResource("Ice_Cracked_metallic.jpg");

        const auto& albedo_tex = TexturePool::GetInstance()->GetOrAllocateResource("spaceship_albedo.jpg");
        const auto& normal_tex = TexturePool::GetInstance()->GetOrAllocateResource("spaceship_normal.jpg");
        const auto& roughness_tex = TexturePool::GetInstance()->GetOrAllocateResource("spaceship_roughness.jpg");
        const auto& metallic_tex = TexturePool::GetInstance()->GetOrAllocateResource("spaceship_metallic.jpg");
        const float uvScale = 1.0f;

        MaterialPropertySetter::SetMaterialPropertyValue(spaceshipPbs_mat, "albedo", albedo_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(spaceshipPbs_mat, "normalMap", normal_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(spaceshipPbs_mat, "roughnessMap", roughness_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(spaceshipPbs_mat, "metallicMap", metallic_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(spaceshipPbs_mat, "uvScale", uvScale);
        MaterialPropertySetter::SetMaterialPropertyValue(
            spaceshipPbs_mat, scene->GetMainCamera(), "CameraPosition", "cameraPosition");

        MaterialPropertySetter::SetMaterialPropertyValue(spaceshipPbs_mat, "albedo_ice", albedo_ice_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(spaceshipPbs_mat, "normalMap_ice", normal_ice_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(spaceshipPbs_mat, "roughnessMap_ice", roughness_ice_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(spaceshipPbs_mat, "metallicMap_ice", metallic_ice_tex);
    }

    const auto d_mesh = std::make_shared<InstancedMeshComponentData>(
        "WeakSpaceShipMesh_c_" + enemyShipIndexStr,
        "spaceship.obj",
        glm::vec3(0),
        glm::vec3(0),
        glm::vec3(1.0),
        spaceshipPbs_mat);
    const auto& meshComponentCreator = std::make_shared<InstancedStaticMeshComponentCreator<InstancedStaticMeshComponent>>();
    const auto& c_mesh
        = std::static_pointer_cast<InstancedStaticMeshComponent>(scene->CreateComponent_GameThread(meshComponentCreator, d_mesh));
    a_enemySpaceship->AddComponent(c_mesh);

    MaterialPropertySetter::SetMaterialInstancedPropertyValue(
        spaceshipPbs_mat, c_mesh, a_enemySpaceship, "p_damageEffect", "damageEffect");
    MaterialPropertySetter::SetMaterialInstancedPropertyValue(
        spaceshipPbs_mat, c_mesh, a_enemySpaceship, "p_freezingEffect", "freezingEffect");

    const auto d_movement
        = std::make_shared<MovementComponentData>("NoPhysMoveComponentData_" + enemyShipIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
    const auto& moveComponentCreator = std::make_shared<MovementComponentCreator<OnRouteMovementComponent>>();
    const auto& c_movement
        = std::static_pointer_cast<OnRouteMovementComponent>(scene->CreateComponent_GameThread(moveComponentCreator, d_movement));
    c_movement->SetReferenceSpeed(10.0f);
    c_movement->SetCurrentSpeedToReferenceValue();
    a_enemySpaceship->AddComponent(c_movement);

    const auto& ghostController = std::make_shared<GhostController>(
        scene->GetPhysicsWorld(), std::make_shared<CollisionSphereShape>(glm::length(scale) * 0.5f), 0.0f);
    const auto physData
        = std::make_shared<PhysicsComponentData>("c_spaceShipPhysicsComponent_" + enemyShipIndexStr, ghostController);
    const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
    const auto& c_ghostPhysics = scene->CreateComponent_GameThread(physicsComponentCreator, physData);
    a_enemySpaceship->AddComponent(c_ghostPhysics);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& particles_mat = materialParser.ParseMaterialDescriptor("OpaqueParticleMaterial.m");
    scene->RegisterMaterialInstance(particles_mat);
    MaterialPropertySetter::SetMaterialPropertyValue(particles_mat, "opacity", 1.0f);
    MaterialPropertySetter::SetMaterialPropertyValue(particles_mat, "clipRadius", 0.35f);

    const auto d_particle = std::make_shared<ParticleSystemComponentData>(
        "c_particleSystemComponent_" + enemyShipIndexStr, particles_mat, glm::vec3(0), glm::vec3(1.0f), 100);
    const auto& particleSystemComponentCreator = std::make_shared<ParticleSystemComponentCreator<ParticleSystemComponent>>();
    const auto& c_particleSystemComponent = std::static_pointer_cast<ParticleSystemComponent>(
        scene->CreateComponent_GameThread(particleSystemComponentCreator, d_particle));
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
    velocityModule->SetExtraVelocityPower(1.0f);
    c_particleSystemComponent->AddParticleModule(velocityModule);

    auto colorModule = std::make_shared<SimpleColorModule>();
    colorModule->SetOwner(c_particleSystemComponent);
    colorModule->SetColorBegin(glm::vec4(1.0f, 0.7f, 0.2f, 1.0f));
    colorModule->SetColorEnd(glm::vec4(1.0f, 0.2f, 0.02f, 1.0f));
    c_particleSystemComponent->AddParticleModule(colorModule);

    a_enemySpaceship->AddComponent(c_particleSystemComponent);

    const auto& lightData = std::make_shared<PointLightComponentData>(
        "c_light_" + enemyShipIndexStr,
        glm::vec3(),
        glm::vec3(),
        100.0f,
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(0.4, 0.1, 0.1),
        glm::vec3(0.4, 0.4, 0.4),
        nullptr,
        true,
        true);
    const auto& lightComponentCreator = std::make_shared<LightComponentCreator<PointLightComponent>>();
    const auto& c_pointLight = scene->CreateComponent_GameThread(lightComponentCreator, lightData);
    a_enemySpaceship->AddComponent(c_pointLight);

    scene->AddActorController(std::make_shared<AiSpaceshipActorController>(a_enemySpaceship));

    const auto& hudCanvas = scene->GetUiHandler()->GetHudCanvas();
    ext_assert(hudCanvas != nullptr, "HUD canvas is null in WeakSpaceShipFactory");
    const auto& uiComponentCreator = std::make_shared<UiComponentCreator<SpaceObjectUiComponent>>();
    const auto& c_uiComponent = std::static_pointer_cast<SpaceObjectUiComponent>(scene->CreateComponent_GameThread(
        uiComponentCreator,
        std::make_shared<SpaceObjectUiComponentData>("c_uiComponent_" + enemyShipIndexStr, hudCanvas, rootComponent)));
    a_enemySpaceship->AddComponent(c_uiComponent);
    c_uiComponent->CreateUiElements(
        "Lora-VariableFont_wght",
        textFontSize,
        "",
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::ivec2(50),
        eTextHorizontalAlignmentType::CENTER,
        eTextVerticalAlignmentType::CENTER);

    const auto& engineMaterial = materialParser.ParseMaterialDescriptor("BurnMaterial.m");
    scene->RegisterMaterialInstance(engineMaterial);
    MaterialPropertySetter::SetMaterialPropertyValue(engineMaterial, scene, "GT_DeltaSec", "gt_timeSec");

    auto engineComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(false);
    const auto data = std::make_shared<MeshComponentData>(
        "c_mesh_engine" + enemyShipIndexStr,
        "ufo.obj",
        glm::vec3(0, -0.05f, 1.2f),
        glm::vec3(90, 0, 0.0f),
        glm::vec3(0.2f, 0.2f, 1.1f),
        "",
        engineMaterial);
    const auto& engineComponent
        = std::static_pointer_cast<StaticMeshComponent>(scene->CreateComponent_GameThread(engineComponentCreator, data));
    a_enemySpaceship->AddComponent(engineComponent);

    auto tweenerParser = std::make_unique<TweenerParser>();
    const auto movementTweener = tweenerParser->ParseTweenerDescriptor("spaceshipMove.tween");
    const auto& rotator_binding = movementTweener->GetPropertyBindingByName("b_rotator");
    BindingAttachmentBuilder::SetAttachment(rootComponent, rotator_binding, "p_rotator");
    a_enemySpaceship->AttachTweener(movementTweener);

    tweenerParser = std::make_unique<TweenerParser>();
    const auto lifecycleTweener = tweenerParser->ParseTweenerDescriptor("weakSpaceshipLifecycle.tween");
    const auto& spaceship_enabled_binding = lifecycleTweener->GetPropertyBindingByName("b_isSpaceshipEnabled");
    BindingAttachmentBuilder::SetAttachment(a_enemySpaceship, spaceship_enabled_binding, "p_isEnabled");
    a_enemySpaceship->AttachTweener(lifecycleTweener);

    a_enemySpaceship->SetScene(scene);

    return a_enemySpaceship;
}

std::shared_ptr<::Graphics::IMaterial>
WeakSpaceShipFactory::GetMaterial(const std::shared_ptr<Scene>& scene, bool& alreadyExists) const
{
    MaterialParser materialParser;
    const auto& materialName = materialParser.ReadMaterialNameFromMaterialDescriptor("SpaceshipPBS.m");
    if (const auto& spaceshipMaterialInstance = scene->GetMaterialByName(materialName)) {
        alreadyExists = true;
        return spaceshipMaterialInstance;
    } else {
        alreadyExists = false;
        const std::shared_ptr<IMaterial>& spaceshipPbs_mat = materialParser.ParseMaterialDescriptor("SpaceshipPBS.m");
        scene->RegisterMaterialInstance(spaceshipPbs_mat);
        return spaceshipPbs_mat;
    }
}
} // namespace Game