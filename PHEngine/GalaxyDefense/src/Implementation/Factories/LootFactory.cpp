#include "LootFactory.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t LootFactory::s_lootCounter = 0;

std::shared_ptr<PortalActor> LootFactory::CreateLoot(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    const eLootCategory lootCategory)
{
    const auto& lootIndexStr = std::to_string(s_lootCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_loot_root_" + lootIndexStr, translation, glm::vec3(0), glm::vec3(1), true);
    const auto& a_loot = std::make_shared<PortalActor>("a_loot_" + lootIndexStr, rootComponent);
    scene->AddActor(a_loot);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& billboard_material = materialParser.ParseMaterialDescriptor("PortalMaterial.m");
    scene->RegisterMaterialInstance(billboard_material);
    MaterialPropertySetter::SetMaterialPropertyValue(billboard_material, scene, "GT_DeltaSec", "gt_timeSec");
    MaterialPropertySetter::SetMaterialPropertyValue(billboard_material, scene, "ScreenResolution", "screenResolution");
    MaterialPropertySetter::SetMaterialPropertyValue(billboard_material, a_loot, "p_colorIntensity", "b_colorIntensity");
    auto portalComponentCreator = std::make_shared<BillboardComponentCreator<BillboardComponent>>();
    const auto data = std::make_shared<BillboardComponentData>(
        "c_billboard_loot", billboardSize, true, glm::vec3(), 0.0f, false, glm::vec3(1.0f), billboard_material);
    const auto& portalComponent
        = std::static_pointer_cast<BillboardComponent>(scene->CreateComponent_GameThread(portalComponentCreator, data));
    portalComponent->SetSortOrderValue(-1000);
    portalComponent->SetDepthWriteMaskEnabled(false);
    a_loot->AddComponent(portalComponent);

    a_loot->SetScene(scene);

    return a_loot;
}
} // namespace Game