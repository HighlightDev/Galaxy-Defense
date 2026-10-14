#include "ForceBarrierRayFactory.h"

#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ElectricBeamComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/ComponentData/ElectricBeamComponentData.h"
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/ForceBarrierBeamComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Implementation/Actors/ForceBarrierRayActor.h"
#include "Implementation/Actors/MissileActor.h"

#include <string>

using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t ForceBarrierRayFactory::s_forceBarrierCounter = 0;

std::shared_ptr<MissileActor> ForceBarrierRayFactory::CreateMissile(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale)
{
    constexpr float c_beamThickness = 2.0f; // -> tube radius (thickness * 0.1)
    constexpr int32_t c_beamCount = 1; // a single clean strand
    constexpr float c_spiralRadius = 2.5f; // repurposed "jitter amount": the corkscrew radius (world units)
    constexpr float c_updateFrequency = 0.05f; // frame step; loop length = framesCount * this

    // Golden palette (reuses the repair beam's three-layer material; only the colours change).
    const glm::vec3 c_coreColor = glm::vec3(1.0f, 0.97f, 0.8f);
    const glm::vec3 c_beamColor = glm::vec3(1.0f, 0.78f, 0.25f);
    const glm::vec3 c_glowColor = glm::vec3(0.7f, 0.45f, 0.1f);
    constexpr float c_opacity = 1.0f;

    // Shield deployed at the barrier.
    constexpr float c_shieldExtent = 5.0f;
    const glm::vec3 c_shieldAmberColor = glm::vec3(0.55f, 0.35f, 0.08f); // dark amber (bottom of the gradient)
    const glm::vec3 c_shieldBrightColor = glm::vec3(1.0f, 0.95f, 0.78f); // bright/white (apex + highlights)

    const auto& indexStr = std::to_string(s_forceBarrierCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_forceBarrier_rootComponent_" + indexStr, translation, rotation, scale, true);
    const auto& a_forceBarrier
        = std::make_shared<ForceBarrierRayActor>("a_forceBarrier_" + indexStr, rootComponent, combatActorsPoolHandler);
    scene->AddActor(a_forceBarrier);

    MaterialParser materialParser;

    // --- Golden spiral beam ---
    const std::shared_ptr<IMaterial>& beam_mat = materialParser.ParseMaterialDescriptor("RepairBeamMaterial.m");
    scene->RegisterMaterialInstance(beam_mat);
    MaterialPropertySetter::SetMaterialPropertyValue(beam_mat, "coreColor", c_coreColor);
    MaterialPropertySetter::SetMaterialPropertyValue(beam_mat, "beamColor", c_beamColor);
    MaterialPropertySetter::SetMaterialPropertyValue(beam_mat, "glowColor", c_glowColor);
    MaterialPropertySetter::SetMaterialPropertyValue(beam_mat, "opacity", c_opacity);
    MaterialPropertySetter::SetMaterialPropertyValue(beam_mat, scene, "GT_DeltaSec", "gt_timeSec");

    const auto d_beam = std::make_shared<ElectricBeamComponentData>(
        "c_forceBarrierBeam_" + indexStr,
        glm::vec3(),
        glm::vec3(),
        c_beamThickness,
        c_beamCount,
        c_spiralRadius,
        c_updateFrequency,
        beam_mat,
        true,
        true);
    const auto& beamComponentCreator = std::make_shared<ElectricBeamComponentCreator<ForceBarrierBeamComponent>>();
    const auto& c_beam
        = std::static_pointer_cast<ForceBarrierBeamComponent>(scene->CreateComponent_GameThread(beamComponentCreator, d_beam));
    a_forceBarrier->AddComponent(c_beam);
    a_forceBarrier->SetBeamComponent(c_beam);

    // --- Shield at the target (self-animating billboard) ---
    const std::shared_ptr<IMaterial>& shield_mat = materialParser.ParseMaterialDescriptor("ForceShieldMaterial.m");
    scene->RegisterMaterialInstance(shield_mat);
    MaterialPropertySetter::SetMaterialPropertyValue(shield_mat, "amberColor", c_shieldAmberColor);
    MaterialPropertySetter::SetMaterialPropertyValue(shield_mat, "brightColor", c_shieldBrightColor);
    MaterialPropertySetter::SetMaterialPropertyValue(shield_mat, "opacity", c_opacity);
    MaterialPropertySetter::SetMaterialPropertyValue(shield_mat, scene, "GT_DeltaSec", "gt_timeSec");

    const auto& billboardCreator = std::make_shared<BillboardComponentCreator<BillboardComponent>>();
    const auto d_shield = std::make_shared<BillboardComponentData>(
        "c_forceBarrierShield_" + indexStr, c_shieldExtent, false, glm::vec3(), 0.0f, false, glm::vec3(1.0f), shield_mat);
    const auto& c_shield
        = std::static_pointer_cast<BillboardComponent>(scene->CreateComponent_GameThread(billboardCreator, d_shield));
    c_shield->SetCanBloomBeApplied(true);
    a_forceBarrier->AddComponent(c_shield);
    a_forceBarrier->SetShield(c_shield);

    a_forceBarrier->SetScene(scene);

    return a_forceBarrier;
}
} // namespace Game
