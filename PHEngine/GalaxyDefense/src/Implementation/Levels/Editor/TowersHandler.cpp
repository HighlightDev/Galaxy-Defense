#include "TowersHandler.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace Graphics;
using namespace EngineCore;

namespace Game {
TowersHandler::TowersHandler(const std::weak_ptr<Scene>& sceneWp, const std::shared_ptr<Actor>& towersActor)
    : mSceneWp(sceneWp)
    , mTowersActor(towersActor)
{
}

void TowersHandler::CreateNewTower(const glm::vec3& position, const glm::vec3& scale)
{
    const auto& sceneSp = mSceneWp.lock();
    assert(sceneSp);

    if (mIdleTowerComponents.empty()) {
        MaterialParser materialParser;
        const auto& towerMaterialPrefab = materialParser.ParseMaterialDescriptor("PhysicalBasedMaterial.m");
        sceneSp->RegisterMaterialInstance(towerMaterialPrefab);

        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, Hash("TowersHandler::CreateNewTower"), 0, [towerMaterialPrefab]() {
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
            });

        const auto& newTowerName = "tower_" + std::to_string(mTowerComponentCounter++);
        const auto& d_mesh = std::make_shared<MeshComponentData>(
            "c_" + newTowerName, "space_station.obj", position, glm::vec3(), glm::vec3(scale), "", towerMaterialPrefab);
        const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
        const auto& c_mesh
            = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
        mTowersActor->AddComponent(c_mesh);
        mActiveTowersComponents.emplace(std::make_tuple(newTowerName, c_mesh));
    } else {
        const auto& idleTowerTuple = mIdleTowerComponents.top();
        const auto& idleTowerComponent = std::get<1>(idleTowerTuple);
        idleTowerComponent->SetIsEnabled(true);
        idleTowerComponent->SetTranslation(position);
        idleTowerComponent->SetScale(scale);
        mActiveTowersComponents.emplace(std::make_tuple(std::get<0>(idleTowerTuple), idleTowerComponent));
        mIdleTowerComponents.pop();
    }
}

void TowersHandler::UndoLastTowerComponent()
{
    if (!mActiveTowersComponents.empty()) {
        const auto& lastActiveTowerTuple = mActiveTowersComponents.top();
        std::get<1>(lastActiveTowerTuple)->SetIsEnabled(false);
        mIdleTowerComponents.emplace(lastActiveTowerTuple);
        mActiveTowersComponents.pop();
    }
}

std::unordered_map<std::string, std::tuple<glm::vec3, glm::vec3>> TowersHandler::CollectTowerPoints() const
{
    std::unordered_map<std::string, std::tuple<glm::vec3, glm::vec3>> towerPoints;
    towerPoints.reserve(mActiveTowersComponents.size());
    auto towersCopy = mActiveTowersComponents;
    while (!towersCopy.empty()) {
        const auto& topItem = towersCopy.top();
        towerPoints.emplace(
            std::get<0>(topItem),
            std::make_tuple(std::get<1>(topItem)->GetTranslation(), std::get<1>(topItem)->GetScale() * 2.0f));
        towersCopy.pop();
    }
    return towerPoints;
}
} // namespace Game
