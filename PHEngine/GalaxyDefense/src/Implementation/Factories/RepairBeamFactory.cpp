#include "RepairBeamFactory.h"

#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ElectricBeamComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/InstancedStaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/ComponentData/ElectricBeamComponentData.h"
#include "Core/GameCore/Components/ComponentData/InstancedMeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/InstancedStaticMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RepairBeamComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/RepairBeamActor.h"

#include <string>
#include <vector>

using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t RepairBeamFactory::s_repairBeamCounter = 0;

std::shared_ptr<MissileActor> RepairBeamFactory::CreateMissile(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale)
{
    // Built on the baked-frame beam pipeline (like the electro/freezing rays). The actor drives targeting/endpoints in
    // its Tick; the beam starts at the origin and is positioned on spawn.
    constexpr float c_beamThickness = 2.0f;    // -> tube radius (thickness * 0.1)
    constexpr int32_t c_beamCount = 1;         // a single clean strand
    constexpr float c_waveAmplitude = 2.5f;    // perpendicular wave offset (world units)
    constexpr float c_updateFrequency = 0.05f; // frame step; loop length = framesCount * this
    const glm::vec3 c_coreColor = glm::vec3(0.85f, 1.0f, 0.9f);
    const glm::vec3 c_beamColor = glm::vec3(0.2f, 1.0f, 0.45f);
    const glm::vec3 c_glowColor = glm::vec3(0.1f, 0.55f, 0.28f);
    constexpr float c_opacity = 1.0f;

    constexpr int32_t c_nanobotCount = 8;
    constexpr float c_nanobotScale = 0.6f;
    const glm::vec3 c_nanobotCoreColor = glm::vec3(0.9f, 1.0f, 0.95f);
    const glm::vec3 c_nanobotHaloColor = glm::vec3(0.15f, 1.0f, 0.4f);

    constexpr float c_crossExtent = 2.0f;
    const glm::vec3 c_crossCoreColor = glm::vec3(1.0f, 1.0f, 1.0f);
    const glm::vec3 c_crossArmColor = glm::vec3(0.2f, 1.0f, 0.45f);
    const glm::vec3 c_crossGlowColor = glm::vec3(0.15f, 0.85f, 0.4f);

    constexpr float c_pulseBallScale = 1.2f;
    constexpr float c_burstExtent = 4.0f;
    const glm::vec3 c_burstRingColor = glm::vec3(0.2f, 1.0f, 0.5f);

    const auto& indexStr = std::to_string(s_repairBeamCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_repairBeam_rootComponent_" + indexStr, translation, rotation, scale, true);
    const auto& a_repairBeam
        = std::make_shared<RepairBeamActor>("a_repairBeam_" + indexStr, rootComponent, combatActorsPoolHandler);
    scene->AddActor(a_repairBeam);

    MaterialParser materialParser;

    // --- Beam ---
    const std::shared_ptr<IMaterial>& beam_mat = materialParser.ParseMaterialDescriptor("RepairBeamMaterial.m");
    scene->RegisterMaterialInstance(beam_mat);
    MaterialPropertySetter::SetMaterialPropertyValue(beam_mat, "coreColor", c_coreColor);
    MaterialPropertySetter::SetMaterialPropertyValue(beam_mat, "beamColor", c_beamColor);
    MaterialPropertySetter::SetMaterialPropertyValue(beam_mat, "glowColor", c_glowColor);
    MaterialPropertySetter::SetMaterialPropertyValue(beam_mat, "opacity", c_opacity);
    MaterialPropertySetter::SetMaterialPropertyValue(beam_mat, scene, "GT_DeltaSec", "gt_timeSec");

    const auto d_beam = std::make_shared<ElectricBeamComponentData>(
        "c_repairBeam_" + indexStr, glm::vec3(), glm::vec3(), c_beamThickness, c_beamCount, c_waveAmplitude, c_updateFrequency,
        beam_mat, true, true);
    const auto& beamComponentCreator = std::make_shared<ElectricBeamComponentCreator<RepairBeamComponent>>();
    const auto& c_beam
        = std::static_pointer_cast<RepairBeamComponent>(scene->CreateComponent_GameThread(beamComponentCreator, d_beam));
    a_repairBeam->AddComponent(c_beam);
    a_repairBeam->SetBeamComponent(c_beam);

    // --- Nanobots (instanced; auto-batched into one draw) ---
    const std::shared_ptr<IMaterial>& nanobot_mat = materialParser.ParseMaterialDescriptor("RepairNanobotMaterial.m");
    scene->RegisterMaterialInstance(nanobot_mat);
    MaterialPropertySetter::SetMaterialPropertyValue(nanobot_mat, "coreColor", c_nanobotCoreColor);
    MaterialPropertySetter::SetMaterialPropertyValue(nanobot_mat, "haloColor", c_nanobotHaloColor);
    MaterialPropertySetter::SetMaterialPropertyValue(nanobot_mat, "opacity", 1.0f);
    MaterialPropertySetter::SetMaterialPropertyValue(nanobot_mat, scene->GetMainCamera(), "CameraPosition", "cameraPosition");

    const auto& nanobotCreator = std::make_shared<InstancedStaticMeshComponentCreator<InstancedStaticMeshComponent>>(false);
    std::vector<std::shared_ptr<InstancedStaticMeshComponent>> nanobots;
    nanobots.reserve(c_nanobotCount);
    for (int32_t i = 0; i < c_nanobotCount; ++i) {
        const auto d_dot = std::make_shared<InstancedMeshComponentData>(
            "c_repairNanobot_" + indexStr + "_" + std::to_string(i), "sphere.obj", glm::vec3(0), glm::vec3(0),
            glm::vec3(c_nanobotScale), nanobot_mat);
        const auto& c_dot
            = std::static_pointer_cast<InstancedStaticMeshComponent>(scene->CreateComponent_GameThread(nanobotCreator, d_dot));
        c_dot->SetCanBloomBeApplied(true);
        a_repairBeam->AddComponent(c_dot);
        nanobots.push_back(c_dot);
    }
    a_repairBeam->SetNanobots(nanobots);

    const auto& billboardCreator = std::make_shared<BillboardComponentCreator<BillboardComponent>>();

    // --- Healing cross at the target ---
    const std::shared_ptr<IMaterial>& cross_mat = materialParser.ParseMaterialDescriptor("RepairCrossMaterial.m");
    scene->RegisterMaterialInstance(cross_mat);
    MaterialPropertySetter::SetMaterialPropertyValue(cross_mat, "coreColor", c_crossCoreColor);
    MaterialPropertySetter::SetMaterialPropertyValue(cross_mat, "armColor", c_crossArmColor);
    MaterialPropertySetter::SetMaterialPropertyValue(cross_mat, "glowColor", c_crossGlowColor);
    MaterialPropertySetter::SetMaterialPropertyValue(cross_mat, "opacity", c_opacity);
    MaterialPropertySetter::SetMaterialPropertyValue(cross_mat, scene, "GT_DeltaSec", "gt_timeSec");

    const auto d_cross = std::make_shared<BillboardComponentData>(
        "c_repairCross_" + indexStr, c_crossExtent, false, glm::vec3(), 0.0f, false, glm::vec3(1.0f), cross_mat);
    const auto& c_cross
        = std::static_pointer_cast<BillboardComponent>(scene->CreateComponent_GameThread(billboardCreator, d_cross));
    c_cross->SetCanBloomBeApplied(true);
    a_repairBeam->AddComponent(c_cross);
    a_repairBeam->SetHealingCross(c_cross);

    // --- Pulse ball (single moving non-instanced mesh) ---
    const auto& pulseBallCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(false);
    const auto d_ball = std::make_shared<MeshComponentData>(
        "c_repairPulseBall_" + indexStr, "sphere.obj", glm::vec3(0), glm::vec3(0), glm::vec3(c_pulseBallScale), nanobot_mat);
    const auto& c_ball = std::static_pointer_cast<StaticMeshComponent>(scene->CreateComponent_GameThread(pulseBallCreator, d_ball));
    c_ball->SetCanBloomBeApplied(true);
    a_repairBeam->AddComponent(c_ball);
    a_repairBeam->SetPulseBall(c_ball);

    // --- Burst rings (self-animating billboard, loops on the scene clock in step with the ball) ---
    const std::shared_ptr<IMaterial>& burst_mat = materialParser.ParseMaterialDescriptor("RepairBurstMaterial.m");
    scene->RegisterMaterialInstance(burst_mat);
    MaterialPropertySetter::SetMaterialPropertyValue(burst_mat, "ringColor", c_burstRingColor);
    MaterialPropertySetter::SetMaterialPropertyValue(burst_mat, "opacity", c_opacity);
    MaterialPropertySetter::SetMaterialPropertyValue(burst_mat, "cyclePeriod", RepairBeamActor::c_pulseCyclePeriodSec);
    MaterialPropertySetter::SetMaterialPropertyValue(burst_mat, "burstStart", RepairBeamActor::c_pulseFlightDurationSec);
    MaterialPropertySetter::SetMaterialPropertyValue(burst_mat, "ringDelay", RepairBeamActor::c_pulseRingDelaySec);
    MaterialPropertySetter::SetMaterialPropertyValue(burst_mat, "ringLife", RepairBeamActor::c_pulseRingLifeSec);
    MaterialPropertySetter::SetMaterialPropertyValue(burst_mat, scene, "GT_DeltaSec", "gt_timeSec");

    const auto d_burst = std::make_shared<BillboardComponentData>(
        "c_repairBurst_" + indexStr, c_burstExtent, false, glm::vec3(), 0.0f, false, glm::vec3(1.0f), burst_mat);
    const auto& c_burst
        = std::static_pointer_cast<BillboardComponent>(scene->CreateComponent_GameThread(billboardCreator, d_burst));
    c_burst->SetCanBloomBeApplied(true);
    a_repairBeam->AddComponent(c_burst);
    a_repairBeam->SetBurstRings(c_burst);

    a_repairBeam->SetScene(scene);

    return a_repairBeam;
}
} // namespace Game
