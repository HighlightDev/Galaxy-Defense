#include "ForwardShadingMeshComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/SceneProxy/ForwardShadingMeshSceneProxy.h"
#include "Core/GameCore/Components/ComponentData/ForwardShadingMeshComponentData.h"

#include <glm/vec3.hpp>

using namespace Graphics::Renderer;

namespace EngineCore
{

   ForwardShadingMeshComponent::ForwardShadingMeshComponent(const std::shared_ptr<ForwardShadingMeshComponentData> &data, const ForwardShadingMeshRenderData &renderData)
       : PrimitiveComponent(data->EngineObjectName, data->m_translation, data->m_eulerRotationDegrees, data->m_scale),
         m_renderData(renderData)
   {
   }

   eComponentType ForwardShadingMeshComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   std::shared_ptr<PrimitiveSceneProxy> ForwardShadingMeshComponent::CreateSceneProxy() const
   {
      return std::make_shared<ForwardShadingMeshSceneProxy>(this);
   }
}
