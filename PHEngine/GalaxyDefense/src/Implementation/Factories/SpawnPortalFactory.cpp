#include "SpawnPortalFactory.h"

#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Implementation/Actors/PortalActor.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t SpawnPortalFactory::s_portalCounter = 0;

std::shared_ptr<PortalActor> SpawnPortalFactory::CreatePortal(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    const float billboardSize)
{
    const auto& portalIndexStr = std::to_string(s_portalCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_portal_root_" + portalIndexStr, translation, glm::vec3(0), glm::vec3(1), true);
    const auto& a_portal = std::make_shared<PortalActor>("a_portal_" + portalIndexStr, rootComponent);
    scene->AddActor(a_portal);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& billboard_material = materialParser.ParseMaterialDescriptor("PortalMaterial.m");
    scene->RegisterMaterialInstance(billboard_material);
    MaterialPropertySetter::SetMaterialPropertyValue(billboard_material, scene, "GT_DeltaSec", "gt_timeSec");
    MaterialPropertySetter::SetMaterialPropertyValue(billboard_material, scene, "ScreenResolution", "screenResolution");
    auto portalComponentCreator = std::make_shared<BillboardComponentCreator<BillboardComponent>>();
    const auto data = std::make_shared<BillboardComponentData>(
        "c_billboard_portal", billboardSize, true, glm::vec3(), 0.0f, glm::vec3(1.0f), billboard_material);
    const auto& portalComponent
        = std::static_pointer_cast<BillboardComponent>(scene->CreateComponent_GameThread(portalComponentCreator, data));
    portalComponent->SetSortOrderValue(-1000);
    portalComponent->SetDepthWriteMaskEnabled(false);
    a_portal->AddComponent(portalComponent);

    a_portal->SetScene(scene);

    return a_portal;
}
} // namespace Game