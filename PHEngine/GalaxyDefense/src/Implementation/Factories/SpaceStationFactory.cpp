#include "SpaceStationFactory.h"

#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionSphereShape.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Implementation/Actors/SpaceStationActor.h"

using namespace Graphics;
using namespace EngineCore;
using namespace Resources;

namespace Game {
std::shared_ptr<SpaceStationActor> SpaceStationFactory::CreateSpaceStation(
    const std::shared_ptr<Scene>& scene,
    const std::string& towerName,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale) const
{
    const auto& towerActor = std::make_shared<SpaceStationActor>(
        towerName, std::make_shared<SceneComponent>("c_root_" + towerName, translation, glm::vec3(), glm::vec3(1)));
    MaterialParser materialParser;
    const auto& towerMaterialPrefab = materialParser.ParseMaterialDescriptor("PhysicalBasedMaterial.m");
    const auto& albedoName = "Space_Station_COLOR.png";
    const auto& normalName = "Space_Station_NORMAL.png";
    const auto& roughnessName = "Space_Station_ROUGHNESS.jpg";
    const auto& metallicName = "Space_Station_METALLIC.jpg";

    const auto& albedo_tex = TexturePool::GetInstance()->GetOrAllocateResource(albedoName);
    const auto& normal_tex = TexturePool::GetInstance()->GetOrAllocateResource(normalName);
    const auto& roughness_tex = TexturePool::GetInstance()->GetOrAllocateResource(roughnessName);
    const auto& metallic_tex = TexturePool::GetInstance()->GetOrAllocateResource(metallicName);
    MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "albedo", albedo_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "normalMap", normal_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "roughnessMap", roughness_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "metallicMap", metallic_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "uvScale", 1.0f);
    scene->RegisterMaterialInstance(towerMaterialPrefab);

    const auto& d_mesh = std::make_shared<MeshComponentData>(
        "c_mesh_" + towerName, "space_station.obj", glm::vec3(), glm::vec3(), scale, "", towerMaterialPrefab);
    const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
    const auto& c_mesh
        = std::static_pointer_cast<StaticMeshComponent>(scene->CreateComponent_GameThread(meshComponentCreator, d_mesh));
    towerActor->AddComponent(c_mesh);

    const auto& ghostController = std::make_shared<GhostController>(
        scene->GetPhysicsWorld(), std::make_shared<CollisionSphereShape>(glm::length(scale) * 0.5f), 0.0f);
    const auto physData = std::make_shared<PhysicsComponentData>("c_physics_" + towerName, ghostController);
    const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
    const auto& c_ghostPhysics = scene->CreateComponent_GameThread(physicsComponentCreator, physData);
    towerActor->AddComponent(c_ghostPhysics);

    scene->AddActor(towerActor);

    towerActor->SetScene(scene);
    return towerActor;
}
} // namespace Game