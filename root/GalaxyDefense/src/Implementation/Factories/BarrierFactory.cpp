#include "BarrierFactory.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Implementation/Actors/BarrierActor.h"
#include "Implementation/DataProviders/GameConstants.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;
using namespace EnginePhysics;

namespace Game {
size_t BarrierFactory::s_barrierCounter = 0;

std::shared_ptr<BarrierActor> BarrierFactory::CreateBarrier(
    const int32_t pillarsMeshCount,
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale)
{
    using namespace Constants::Barrier;

    const auto& barrierIndexStr = std::to_string(s_barrierCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_barrier_root_" + barrierIndexStr, translation, glm::vec3(), glm::vec3(1.0), true);
    const auto& a_barrier = std::make_shared<BarrierActor>("a_barrier_" + barrierIndexStr, rootComponent);
    scene->AddActor(a_barrier);
    a_barrier->SetScene(scene);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& barrierPbs_mat = materialParser.ParseMaterialDescriptor("PbrSingleValueMaterial.m");
    scene->RegisterMaterialInstance(barrierPbs_mat);

    MaterialPropertySetter::SetMaterialPropertyValue(barrierPbs_mat, "albedo", c_albedo);
    MaterialPropertySetter::SetMaterialPropertyValue(barrierPbs_mat, "metallicValue", c_metallicValue);
    MaterialPropertySetter::SetMaterialPropertyValue(barrierPbs_mat, "roughnessValue", c_roughnessValue);

    const auto noiseTex = TexturePool::GetInstance()->GetOrAllocateResource("perlin_noise_128x128.png");
    const std::shared_ptr<IMaterial>& electroRay_material = materialParser.ParseMaterialDescriptor("ElectroBeamMaterial.m");
    scene->RegisterMaterialInstance(electroRay_material);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "noise", noiseTex);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "beamGlowColor", c_beamGlowColor);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "beamMainColor", c_beamMainColor);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, scene, "GT_DeltaSec", "gt_timeSec");

    a_barrier->SetBarrierMaterials(barrierPbs_mat, electroRay_material);

    a_barrier->SetNominalPillarHealth(c_pillarHealth);

    a_barrier->SetBarrierPillarSize(scale);
    a_barrier->SetBarrierProtoData(
        {"JetBrainsMono-VariableFont_wght",
         c_protoFontSize,
         "",
         c_protoDamageTextColor,
         c_protoHealTextColor,
         c_protoPadding,
         eTextHorizontalAlignmentType::CENTER,
         eTextVerticalAlignmentType::CENTER});

    for (int32_t i = 0; i < pillarsMeshCount; ++i) {
        a_barrier->CreateNewBarrierPillar(glm::vec3(0), rotation, scale);
    }

    return a_barrier;
}
} // namespace Game