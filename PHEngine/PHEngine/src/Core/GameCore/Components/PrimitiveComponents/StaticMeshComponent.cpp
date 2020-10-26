#include "StaticMeshComponent.h"
#include "Core/GraphicsCore/SceneProxy/StaticMeshSceneProxy.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeData.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"

#include <memory>
#include <algorithm>

namespace Game
{

   StaticMeshComponent::StaticMeshComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const StaticMeshRenderData& renderData)
      : PrimitiveComponent(gameObjectName, std::move(translation), std::move(rotation), std::move(scale))
      , m_renderData(renderData)
   {

   }

   StaticMeshComponent::~StaticMeshComponent()
   {

   }

   void StaticMeshComponent::Tick(const float deltaTime)
   {
      Base::Tick(deltaTime);
   }

   void StaticMeshComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      auto& actorData = GetSerializeDataActor(dataContainer);

      auto meshData = std::make_shared<SerializeDataMesh>();
      meshData->ComponentName = GameObjectName;
      meshData->ModelName = GameObjectName;
      meshData->Translation = GetTranslation();
      meshData->Rotation = GetRotationEuler();
      meshData->Scale = GetScale();
      meshData->LuaScriptName = ""; // TODO: for now
      
      const SerializeDataMaterial& material = SerializeHelper::GetSerializeDataMaterial(m_renderData.mMaterialInstance);
      
      meshData->MeshMaterial = material;

      actorData.ComponentsData.emplace_back(meshData);
   }

   std::shared_ptr<PrimitiveSceneProxy> StaticMeshComponent::CreateSceneProxy() const
   {
      return std::make_shared<StaticMeshSceneProxy>(this);
   }

   ComponentType StaticMeshComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

}
