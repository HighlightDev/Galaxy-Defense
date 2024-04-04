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
    assert(mNewTowerName != "");
    MaterialParser materialParser;
    const auto &towerMaterialPrefab = materialParser.ParseMaterialDescriptor("PbrSingleValueMaterial.m");
    MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "albedo", mNewTowerColor);
    MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "metallicValue", 1.0f);
    MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "roughnessValue", 0.5f);
    sceneSp->RegisterMaterialInstance(towerMaterialPrefab);

    if (mIdleTowerComponents.empty())
    {
      const auto &d_mesh = std::make_shared<MeshComponentData>("c_towerMesh_" + std::to_string(mTowerComponentCounter++),
                                                               "cube.obj",
                                                               position,
                                                               glm::vec3(),
                                                               glm::vec3(scale),
                                                               "",
                                                               towerMaterialPrefab);
      const auto &meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
      const auto &c_mesh = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
      mTowersActor->AddComponent(c_mesh);
      mActiveTowersComponents.emplace(std::make_tuple(mNewTowerName, c_mesh));
    }
    else
    {
      const auto &idleTowerTuple = mIdleTowerComponents.top();
      const auto &idleTowerComponent = std::get<1>(idleTowerTuple);
      idleTowerComponent->SetIsEnabled(true);
      mActiveTowersComponents.emplace(std::make_tuple(mNewTowerName, idleTowerComponent));
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

  std::unordered_map<std::string, glm::vec3> TowersHandler::CollectTowerPoints() const
  {
    std::unordered_map<std::string, glm::vec3> towerPoints;
    towerPoints.reserve(mActiveTowersComponents.size());
    auto towersCopy = mActiveTowersComponents;
    while (!towersCopy.empty())
    {
      const auto &topItem = towersCopy.top();
      towerPoints.emplace(std::get<0>(topItem), std::get<1>(topItem)->GetTranslation());
      towersCopy.pop();
    }
    return towerPoints;
  }

  void TowersHandler::SetNewTowerName(const std::string &newTowerName)
  {
    assert(newTowerName != "");
    if (newTowerName != mNewTowerName)
    {
      mNewTowerName = newTowerName;
    }
  }

  void TowersHandler::SetNewTowerColor(const glm::vec3 &color)
  {
    if (!EngineMath::CheckSimilarityVec3(color, mNewTowerColor))
    {
      mNewTowerColor = color;
    }
  }
}
