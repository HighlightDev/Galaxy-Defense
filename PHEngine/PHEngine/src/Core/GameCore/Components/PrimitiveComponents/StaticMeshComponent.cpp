#include "StaticMeshComponent.h"
#include "Core/GraphicsCore/SceneProxy/StaticMeshSceneProxy.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeData.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"

#include <memory>
#include <algorithm>

using namespace Graphics;

namespace Game
{

   StaticMeshComponent::StaticMeshComponent(const MeshComponentData& meshComponentData, const StaticMeshRenderData& renderData)
      : PrimitiveComponent(meshComponentData.GameObjectName
      , meshComponentData.m_translation
      , meshComponentData.m_eulerRotationDegrees
      , meshComponentData.m_scale
      , renderData.m_skin->GetBoundingBox())
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

   void StaticMeshComponent::PostLevelInit()
   {
      
   }

   std::shared_ptr<IMaterial> StaticMeshComponent::GetMaterial() const
   {
      std::shared_ptr<IMaterial> materialResult = nullptr;
      if (const auto& sceneSP = m_sceneWP.lock())
      {
         materialResult = sceneSP->GetMaterialByProxyId(m_renderData.mMaterialProxy->GetSceneProxyId());
      }
      assert(materialResult != nullptr);
      return materialResult;
   }

   void StaticMeshComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      auto& actorData = GetSerializeDataActor(dataContainer);

      auto staticCompData = SerializeHelper::GetSerializedDataStaticMesh(this);
      actorData.ComponentsData.emplace_back(staticCompData);
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
