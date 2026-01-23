#include "BackgroundPlanetsFactory.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ParticleSystemComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/ParticleComponents/CpuParticleSystemComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
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
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Implementation/Actors/BackgroundSpaceObjectActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Controllers/AiActorController.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t BackgroundPlanetsFactory::s_backgroundPlanetCounter = 0;

std::shared_ptr<BackgroundSpaceObjectActor> BackgroundPlanetsFactory::CreateSpaceObject(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    const std::string& imageName,
    const float billboardSize)
{
    const auto& backgroundPlanetIndexStr = std::to_string(s_backgroundPlanetCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_backgroundPlanet_root_" + backgroundPlanetIndexStr, translation, glm::vec3(0), glm::vec3(1), true);
    const auto& a_backgroundPlanet
        = std::make_shared<BackgroundSpaceObjectActor>("a_backgroundPlanet_" + backgroundPlanetIndexStr, rootComponent);
    scene->AddActor(a_backgroundPlanet);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& billboard_material = materialParser.ParseMaterialDescriptor("BillboardMaterial.m");
    scene->RegisterMaterialInstance(billboard_material);
    const auto albedo_texture = TexturePool::GetInstance()->GetOrAllocateResource(imageName);

    MaterialPropertySetter::SetMaterialPropertyValue(billboard_material, "albedo", albedo_texture);
    MaterialPropertySetter::SetMaterialPropertyValue(billboard_material, "inverse_y", (int32_t) false);

    auto billboardComponentCreator = std::make_shared<BillboardComponentCreator<BillboardComponent>>();
    const auto data = std::make_shared<BillboardComponentData>(
        "c_billboard_" + backgroundPlanetIndexStr,
        billboardSize,
        true,
        glm::vec3(0.0f),
        0.0f,
        false,
        glm::vec3(1.0f),
        billboard_material);
    const auto& billboardComponent
        = std::static_pointer_cast<BillboardComponent>(scene->CreateComponent_GameThread(billboardComponentCreator, data));
    billboardComponent->SetSortOrderValue(-10000);
    a_backgroundPlanet->AddComponent(billboardComponent);

    const auto d_movement = std::make_shared<MovementComponentData>(
        "c_backgroundPlanet_no_phys_movement_" + backgroundPlanetIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
    const auto& moveComponentCreator = std::make_shared<MovementComponentCreator<NoPhysicsMovementComponent>>();
    const auto& c_movement = std::static_pointer_cast<NoPhysicsMovementComponent>(
        scene->CreateComponent_GameThread(moveComponentCreator, d_movement));
    c_movement->SetReferenceSpeed(8.5f);
    c_movement->SetCurrentSpeedToReferenceValue();
    c_movement->SetDirection(glm::vec3(0.0f, .0f, -1.0f));
    a_backgroundPlanet->AddComponent(c_movement);

    scene->AddActorController(std::make_shared<AiActorController>(a_backgroundPlanet));
    a_backgroundPlanet->SetScene(scene);

    return a_backgroundPlanet;
}
} // namespace Game