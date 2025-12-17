#include "BarrierFactory.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentCreators/ElectricBeamComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/PrimitiveComponents/ElectricBeamComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Implementation/Actors/BarrierActor.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t BarrierFactory::s_barrierCounter = 0;

std::shared_ptr<BarrierActor> BarrierFactory::CreateBarrier(
    const int32_t pillarsMeshCount,
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale)
{
    const auto& barrierIndexStr = std::to_string(s_barrierCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_barrier_root_" + barrierIndexStr, translation, glm::vec3(), glm::vec3(1.0));
    const auto& a_barrier = std::make_shared<BarrierActor>("a_barrier_" + barrierIndexStr, rootComponent);
    scene->AddActor(a_barrier);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& barrierPbs_mat = materialParser.ParseMaterialDescriptor("PbrSingleValueMaterial.m");
    scene->RegisterMaterialInstance(barrierPbs_mat);

    MaterialPropertySetter::SetMaterialPropertyValue(barrierPbs_mat, "albedo", glm::vec3(1.0, 1.0, 0.0));
    MaterialPropertySetter::SetMaterialPropertyValue(barrierPbs_mat, "metallicValue", 1.8f);
    MaterialPropertySetter::SetMaterialPropertyValue(barrierPbs_mat, "roughnessValue", 0.5f);

    int32_t pillarIndex = 0;
    while (pillarIndex < pillarsMeshCount) {
        const auto d_mesh = std::make_shared<MeshComponentData>(
            "c_barrier_mesh_barrier_" + barrierIndexStr + "_pillar_" + std::to_string(pillarIndex),
            "ufo.obj",
            glm::vec3(0),
            rotation,
            scale,
            "",
            barrierPbs_mat);
        const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
        const auto& c_mesh
            = std::static_pointer_cast<StaticMeshComponent>(scene->CreateComponent_GameThread(meshComponentCreator, d_mesh));
        a_barrier->AddBarrierPillarMesh(c_mesh);
        ++pillarIndex;
    }

    const auto noiseTex = TexturePool::GetInstance()->GetOrAllocateResource("perlin_noise_128x128.png");
    const std::shared_ptr<IMaterial>& electroRay_material = materialParser.ParseMaterialDescriptor("ElectroBeamMaterial.m");
    scene->RegisterMaterialInstance(electroRay_material);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "noise", noiseTex);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "beamGlowColor", glm::vec3(0.2, 1.0, 1.0));
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "beamMainColor", glm::vec3(0.6, 0.4, 1.0));
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, scene, "GT_DeltaSec", "gt_timeSec");

    int32_t rayIndex = 0;
    const auto currentBarrierIndex = s_barrierCounter - 1;
    while (rayIndex < (pillarsMeshCount - 1)) {
        const auto d_mesh = std::make_shared<ElectricBeamComponentData>(
            "c_barrier_mesh_line_" + barrierIndexStr + "_ray_" + std::to_string(rayIndex),
            glm::vec3(),
            glm::vec3(),
            2.0f,
            3,
            2.0f,
            0.05f,
            electroRay_material);
        const std::shared_ptr<IComponentCreatable>& meshComponentCreator
            = std::make_shared<ElectricBeamComponentCreator<ElectricBeamComponent>>();
        const auto& c_mesh
            = std::static_pointer_cast<ElectricBeamComponent>(scene->CreateComponent_GameThread(meshComponentCreator, d_mesh));
        a_barrier->AddRayLineMesh(c_mesh);
        ++rayIndex;
    }

    a_barrier->SetScene(scene);

    return a_barrier;
}
} // namespace Game