#pragma once

#include "PhysicsShapeDebugRenderComponent.h"
#include "Core/GraphicsCore/SceneProxy/PhysicsShapeDebugRenderSceneProxy.h"

namespace Game
{

   PhysicsShapeDebugRenderComponent::PhysicsShapeDebugRenderComponent(PhyShapeDebugRenderData renderData)
      : PrimitiveComponent(glm::vec3(), glm::vec3(), glm::vec3(1))
      , m_renderData(renderData)
   {
   }
   PhysicsShapeDebugRenderComponent::~PhysicsShapeDebugRenderComponent()
   {
   }

   uint64_t PhysicsShapeDebugRenderComponent::GetComponentType() const {
      return PRIMITIVE_COMPONENT;
   }

   std::shared_ptr<PrimitiveSceneProxy> PhysicsShapeDebugRenderComponent::CreateSceneProxy() const 
   {
      return std::make_shared<PhysicsShapeDebugRenderSceneProxy>(this);
   }
}