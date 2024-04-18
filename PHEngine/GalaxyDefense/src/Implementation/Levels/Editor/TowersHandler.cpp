#include "TowersHandler.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace Graphics;
using namespace EngineCore;

namespace Game
{
  TowersHandler::TowersHandler(const std::weak_ptr<Scene> &sceneWp,
                               const std::shared_ptr<Actor> &towersActor)
      : mSceneWp(sceneWp),
        mTowersActor(towersActor)
  {
  }

  void TowersHandler::CreateNewTower(const glm::vec3 &position,
                                     const glm::vec3 &scale)
  {
    const auto &sceneSp = mSceneWp.lock();
    assert(sceneSp);

    if (mIdleTowerComponents.empty())
    {
      MaterialParser materialParser;
      const auto &towerMaterialPrefab = materialParser.ParseMaterialDescriptor("PbrSingleValueMaterial.m");
      MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "albedo", EngineMath::CreateRandomColor());
      MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "metallicValue", 1.0f);
      MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "roughnessValue", 0.5f);
      sceneSp->RegisterMaterialInstance(towerMaterialPrefab);

      const auto &newTowerName = "tower_" + std::to_string(mTowerComponentCounter++);
      const auto &d_mesh = std::make_shared<MeshComponentData>("c_" + newTowerName,
                                                               "sphere.obj",
                                                               position,
                                                               glm::vec3(),
                                                               glm::vec3(scale),
                                                               "",
                                                               towerMaterialPrefab);
      const auto &meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
      const auto &c_mesh = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
      mTowersActor->AddComponent(c_mesh);
      mActiveTowersComponents.emplace(std::make_tuple(newTowerName, c_mesh));
    }
    else
    {
      const auto &idleTowerTuple = mIdleTowerComponents.top();
      const auto &idleTowerComponent = std::get<1>(idleTowerTuple);
      idleTowerComponent->SetIsEnabled(true);
      idleTowerComponent->SetTranslation(position);
      idleTowerComponent->SetScale(scale);
      mActiveTowersComponents.emplace(std::make_tuple(std::get<0>(idleTowerTuple), idleTowerComponent));
      mIdleTowerComponents.pop();
    }
  }

  void TowersHandler::UndoLastTowerComponent()
  {
    if (!mActiveTowersComponents.empty())
    {
      const auto &lastActiveTowerTuple = mActiveTowersComponents.top();
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
    while (!towersCopy.empty())
    {
      const auto &topItem = towersCopy.top();
      towerPoints.emplace(std::get<0>(topItem), std::make_tuple(std::get<1>(topItem)->GetTranslation(), std::get<1>(topItem)->GetScale()));
      towersCopy.pop();
    }
    return towerPoints;
  }
}
