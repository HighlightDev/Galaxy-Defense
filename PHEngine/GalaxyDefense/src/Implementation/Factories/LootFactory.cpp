#include "LootFactory.h"

#include "Core/GameCore/Components/ComponentCreators/InstancedStaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/InstancedMeshComponentData.h"
#include "Core/GameCore/Components/PrimitiveComponents/InstancedStaticMeshComponent.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Implementation/Actors/LootActor.h"

using namespace Resources;
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

    return a_loot;
}
} // namespace Game