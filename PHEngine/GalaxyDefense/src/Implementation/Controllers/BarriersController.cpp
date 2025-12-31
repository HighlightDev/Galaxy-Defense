#include "BarriersController.h"

#include "Core/GameCore/Components/ComponentCreators/ElectricBeamComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/PrimitiveComponents/ElectricBeamComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Implementation/Actors/BarrierActor.h"

#include <algorithm>

using namespace Graphics;
using namespace EngineCore;
using namespace Resources;
using namespace Graphics::Texture;

namespace Game {
BarriersController::BarriersController(const std::weak_ptr<Scene>& sceneWp)
    : mSceneWp(sceneWp)
{
}

void BarriersController::OnPostLevelInit()
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "BarriersController scene pointer is null in OnPostLevelInit");

    const std::shared_ptr<::Graphics::Texture::ITexture>& noiseTex
        = TexturePool::GetInstance()->GetOrAllocateResource("perlin_noise_128x128.png");

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& barrierPbs_mat = materialParser.ParseMaterialDescriptor("PbrSingleValueMaterial.m");
    sceneSp->RegisterMaterialInstance(barrierPbs_mat);
    MaterialPropertySetter::SetMaterialPropertyValue(barrierPbs_mat, "albedo", glm::vec3(1.0, 1.0, 0.0));
    MaterialPropertySetter::SetMaterialPropertyValue(barrierPbs_mat, "metallicValue", 1.8f);
    MaterialPropertySetter::SetMaterialPropertyValue(barrierPbs_mat, "roughnessValue", 0.5f);

    const std::shared_ptr<IMaterial>& electroRay_material = materialParser.ParseMaterialDescriptor("ElectroBeamMaterial.m");
    sceneSp->RegisterMaterialInstance(electroRay_material);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "noise", noiseTex);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "beamMainColor", glm::vec3(0.6, 0.4, 1.0));
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "beamGlowColor", glm::vec3(0.2, 1.0, 1.0));
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, sceneSp, "GT_DeltaSec", "gt_timeSec");

    mBarrierMaterial = barrierPbs_mat;
    mRayMaterial = electroRay_material;
}

void BarriersController::SelectNewBarrier(const std::string& barrierName)
{
    if (mCurrentActiveBarrierName == barrierName)
        return;

    mCurrentActiveBarrierName = barrierName;
    mCurrentBarrierPillarsCount = 0;
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "BarriersController scene pointer is null in SelectNewBarrier");
    const auto& barrierIndexStr = std::to_string(mBarrierActorsCount++);
    const auto& rootComponent
        = std::make_shared<SceneComponent>("c_barrier_root_" + barrierIndexStr, glm::vec3(), glm::vec3(), glm::vec3(1.0), true);
    const auto& a_barrier = std::make_shared<BarrierActor>("a_barrier_" + barrierIndexStr, rootComponent);
    sceneSp->AddActor(a_barrier);
    mBarrierActors.emplace(mCurrentActiveBarrierName, a_barrier);
}

void BarriersController::CreateNewBarrierPillar(const glm::vec3& position)
{
    if ("" == mCurrentActiveBarrierName)
        return;

    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "BarriersController scene pointer is null in CreateNewBarrierPillar");
    const auto& a_barrier = mBarrierActors.at(mCurrentActiveBarrierName);
    const auto pillarsSize = mCurrentBarrierPillarsCount++;
    const auto& barrierIndexStr = std::to_string(mBarrierActorsCount);
    const auto d_mesh = std::make_shared<MeshComponentData>(
        "c_barrier_mesh_barrier_" + barrierIndexStr + "_pillar_" + std::to_string(pillarsSize),
        "ufo.obj",
        position,
        glm::vec3(),
        glm::vec3(3.0, 12.0, 3.0),
        "",
        mBarrierMaterial);
    const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
    const auto& c_mesh
        = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
    a_barrier->AddBarrierPillarMesh(c_mesh);

    if (pillarsSize) {
        const auto rayIndex = a_barrier->GetComponentsByType<ElectricBeamComponent>().size();
        const auto d_mesh = std::make_shared<ElectricBeamComponentData>(
            "c_barrier_mesh_line_" + barrierIndexStr + "_ray_" + std::to_string(rayIndex),
            glm::vec3(0),
            glm::vec3(0),
            1.0f,
            3,
            1.0f,
            0.05f,
            mRayMaterial);
        const auto& meshComponentCreator = std::make_shared<ElectricBeamComponentCreator<ElectricBeamComponent>>();
        const auto& c_mesh
            = std::static_pointer_cast<ElectricBeamComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
        c_mesh->SetSortOrderValue(200 + rayIndex);
        a_barrier->AddRayLineMesh(c_mesh);
    }
    a_barrier->TrySetBarrierPillarMeshRelativeTransform(pillarsSize, position, glm::vec3(), glm::vec3(3.0, 12.0, 3.0));
}

} // namespace Game
