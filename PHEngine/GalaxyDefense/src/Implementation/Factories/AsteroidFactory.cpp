#include "AsteroidFactory.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ParticleSystemComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
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
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Implementation/Actors/AsteroidActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Controllers/AiActorController.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t AsteroidFactory::s_asteroidCounter = 0;

std::shared_ptr<SpaceObjectActor> AsteroidFactory::CreateSpaceObject(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale)
{
    const auto& asteroidIndexStr = std::to_string(s_asteroidCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_asteroid_root_" + asteroidIndexStr, translation, glm::vec3(0), glm::vec3(1), true);
    const auto& a_asteroid = std::make_shared<AsteroidActor>("a_asteroid_" + asteroidIndexStr, rootComponent);
    scene->AddActor(a_asteroid);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& asteroidPbs_mat = materialParser.ParseMaterialDescriptor("PhysicalBasedMaterial.m");
    scene->RegisterMaterialInstance(asteroidPbs_mat);

    const std::string albedoName = "Asteroid_albedo.jpg";
    const std::string normalName = "Asteroid_normal.jpg";
    const std::string roughnessName = "Asteroid_roughness.jpg";
    const std::string metallicName = "Asteroid_metallic.jpg";

    const auto& albedo_tex = TexturePool::GetInstance()->GetOrAllocateResource(albedoName);
    const auto& normal_tex = TexturePool::GetInstance()->GetOrAllocateResource(normalName);
    const auto& roughness_tex = TexturePool::GetInstance()->GetOrAllocateResource(roughnessName);
    const auto& metallic_tex = TexturePool::GetInstance()->GetOrAllocateResource(metallicName);
    const float uvScale = 1.0f;

    MaterialPropertySetter::SetMaterialPropertyValue(asteroidPbs_mat, "albedo", albedo_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(asteroidPbs_mat, "normalMap", normal_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(asteroidPbs_mat, "roughnessMap", roughness_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(asteroidPbs_mat, "metallicMap", metallic_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(asteroidPbs_mat, "uvScale", uvScale);

    const auto d_mesh = std::make_shared<MeshComponentData>(
        "c_asteroid_mesh_" + asteroidIndexStr, "asteroid.fbx", glm::vec3(0), rotation, scale, asteroidPbs_mat, true, true);
    const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
    const auto& c_mesh
        = std::static_pointer_cast<StaticMeshComponent>(scene->CreateComponent_GameThread(meshComponentCreator, d_mesh));
    a_asteroid->AddComponent(c_mesh);

    const auto d_movement = std::make_shared<MovementComponentData>(
        "c_asteroid_no_phys_movement_" + asteroidIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
    const auto& moveComponentCreator = std::make_shared<MovementComponentCreator<NoPhysicsMovementComponent>>();
    const auto& c_movement = std::static_pointer_cast<NoPhysicsMovementComponent>(
        scene->CreateComponent_GameThread(moveComponentCreator, d_movement));
    c_movement->SetReferenceSpeed(5.0f);
    c_movement->SetCurrentSpeedToReferenceValue();
    c_movement->SetDirection(glm::vec3(1.0f, .0f, 0.0f));
    a_asteroid->AddComponent(c_movement);

    const auto& sphereShape = std::make_shared<CollisionSphereShape>(3.0f);
    const auto& ghostController = std::make_shared<GhostController>(scene->GetPhysicsWorld(), sphereShape, 0.0f);
    const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
    const auto& c_ghostPhysics = std::static_pointer_cast<PhysicsComponent>(scene->CreateComponent_GameThread(
        physicsComponentCreator,
        std::make_shared<PhysicsComponentData>("c_asteroid_physics_" + asteroidIndexStr, ghostController)));
    sphereShape->SetParentPhysicsComponent(c_ghostPhysics);
    a_asteroid->AddComponent(c_ghostPhysics);

    scene->AddActorController(std::make_shared<AiActorController>(a_asteroid));
    a_asteroid->SetScene(scene);

    return a_asteroid;
}
} // namespace Game