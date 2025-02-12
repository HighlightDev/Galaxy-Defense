#include "BarriersHandler.h"

#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
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
BarriersHandler::BarriersHandler(const std::weak_ptr<Scene>& sceneWp)
    : mSceneWp(sceneWp)
{
}

void BarriersHandler::OnPostLevelInit()
{
    mNoiseTex = TexturePool::GetInstance()->GetOrAllocateResource("perlin_noise.png");
}

void BarriersHandler::SelectNewBarrier(const std::string& barrierName)
{
    if (mCurrentActiveBarrierName == barrierName)
        return;

    mCurrentActiveBarrierName = barrierName;
    mCurrentBarrierPillarsCount = 0;
    const auto& sceneSp = mSceneWp.lock();
    assert(sceneSp);
    const auto& barrierIndexStr = std::to_string(mBarrierActorsCount++);
    const auto& rootComponent
        = std::make_shared<SceneComponent>("c_barrier_root_" + barrierIndexStr, glm::vec3(), glm::vec3(), glm::vec3(1.0));
    const auto& a_barrier = std::make_shared<BarrierActor>("a_barrier_" + barrierIndexStr, rootComponent);
    sceneSp->AddActor(a_barrier);
    mBarrierActors.emplace(mCurrentActiveBarrierName, a_barrier);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& barrierPbs_mat = materialParser.ParseMaterialDescriptor("PbrSingleValueMaterial.m");
    sceneSp->RegisterMaterialInstance(barrierPbs_mat);
    MaterialPropertySetter::SetMaterialPropertyValue(barrierPbs_mat, "albedo", glm::vec3(1.0, 1.0, 0.0));
    MaterialPropertySetter::SetMaterialPropertyValue(barrierPbs_mat, "metallicValue", 1.8f);
    MaterialPropertySetter::SetMaterialPropertyValue(barrierPbs_mat, "roughnessValue", 0.5f);

    const std::shared_ptr<IMaterial>& electroRay_material = materialParser.ParseMaterialDescriptor("ElectroBarrierMaterial.m");
    sceneSp->RegisterMaterialInstance(electroRay_material);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "noise", mNoiseTex);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "rayColor", glm::vec3(1.0, 0.5, 1.0));
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "rayWidthCoef", 1.2f);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, sceneSp, "GT_DeltaSec", "gt_timeSec");
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "opacity", 1.0f);

    mBarrierMaterials.emplace(mCurrentActiveBarrierName, std::make_pair(barrierPbs_mat, electroRay_material));
}

void BarriersHandler::CreateNewBarrierPillar(const glm::vec3& position, const glm::vec3& scale)
{
    if ("" == mCurrentActiveBarrierName)
        return;

    const auto& sceneSp = mSceneWp.lock();
    assert(sceneSp);
    const auto& a_barrier = mBarrierActors.at(mCurrentActiveBarrierName);
    const auto pillarsSize = mCurrentBarrierPillarsCount++;
    const auto& barrierIndexStr = std::to_string(mBarrierActorsCount);
    const auto& barrier_mat = mBarrierMaterials.at(mCurrentActiveBarrierName);
    const auto d_mesh = std::make_shared<MeshComponentData>(
        "c_barrier_mesh_barrier_" + barrierIndexStr + "_pillar_" + std::to_string(pillarsSize),
        "ufo.obj",
        position,
        glm::vec3(),
        glm::vec3(3.0, 12.0, 3.0),
        "",
        barrier_mat.first);
    const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
    const auto& c_mesh
        = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
    a_barrier->AddBarrierPillarMesh(c_mesh);

    if (pillarsSize) {
        const auto rayIndex = a_barrier->GetComponentsByType<RuntimeGeneratedLineComponent>().size();
        const auto d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>(
            "c_barrier_mesh_line_" + barrierIndexStr + "_ray_" + std::to_string(rayIndex),
            4,
            glm::vec3(0),
            glm::vec3(),
            glm::vec3(1),
            "",
            barrier_mat.second);
        const auto& meshComponentCreator
            = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedLineComponent>>();
        const auto& c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(
            sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
        c_mesh->SetSortOrderValue(200 + rayIndex);
        c_mesh->SetLineWidth(10.0f);
        a_barrier->AddRayLineMesh(c_mesh);
    }
    a_barrier->TrySetBarrierPillarMeshRelativeTransform(pillarsSize, position, glm::vec3(), glm::vec3(3.0, 12.0, 3.0));
}

void BarriersHandler::SetBarrierColor(const glm::vec3& currentBarrierColor)
{
    const auto& barrierPbs_mat = mBarrierMaterials.at(mCurrentActiveBarrierName).first;
    MaterialPropertySetter::SetMaterialPropertyValue(barrierPbs_mat, "albedo", currentBarrierColor);
}

void BarriersHandler::SetRayColor(const glm::vec3& currentRayColor)
{
    const auto& ray_mat = mBarrierMaterials.at(mCurrentActiveBarrierName).second;
    MaterialPropertySetter::SetMaterialPropertyValue(ray_mat, "rayColor", currentRayColor);
}

void BarriersHandler::UndoLastBarrier()
{
    if ("" != mCurrentActiveBarrierName && mBarrierActors.count(mCurrentActiveBarrierName)) {
        const auto& lastBarrierSp = mBarrierActors.at(mCurrentActiveBarrierName);
        mBarrierActors.erase(mCurrentActiveBarrierName);
        lastBarrierSp->SetIsEnabled(false);
        const auto& sceneSp = mSceneWp.lock();
        assert(sceneSp);
        sceneSp->RemoveActor(lastBarrierSp);
        if (mBarrierMaterials.count(mCurrentActiveBarrierName)) {
            mBarrierMaterials.erase(mCurrentActiveBarrierName);
        }
        const auto newName = mCurrentActiveBarrierName;
        mCurrentActiveBarrierName = "";
        SelectNewBarrier(newName);
    }
}

std::unordered_map<std::string, std::vector<glm::vec3>> BarriersHandler::CollectBarrierPoints() const
{
    if (mBarrierActors.size()) {
        std::unordered_map<std::string, std::vector<glm::vec3>> barriersResultMap;

        for (const auto& [barrierName, barrier] : mBarrierActors) {
            const auto pillarsMeshVector = barrier->GetBarrierPillarsMeshComponents();

            std::vector<glm::vec3> barrierPillarsPositions;
            barrierPillarsPositions.reserve(pillarsMeshVector.size());
            std::transform(
                pillarsMeshVector.cbegin(),
                pillarsMeshVector.cend(),
                std::back_inserter(barrierPillarsPositions),
                [](const auto& pillarMeshComp) { return pillarMeshComp->GetTranslation(); });
            barriersResultMap.emplace(barrierName, std::move(barrierPillarsPositions));
        }
        return barriersResultMap;
    }
    return {};
}
} // namespace Game
