#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GraphicsCore/RenderData/PhyShapeDebugRenderData.h"

using namespace Graphics::Data;

namespace Game
{
   
   class PhysicsShapeDebugRenderComponent
      : public PrimitiveComponent
   {
   protected:

      using Base = PrimitiveComponent;

   private:

      PhyShapeDebugRenderData m_renderData;

   public:

      PhysicsShapeDebugRenderComponent(PhyShapeDebugRenderData renderData);

      virtual ~PhysicsShapeDebugRenderComponent();

      virtual uint64_t GetComponentType() const override;

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      inline const PhyShapeDebugRenderData& GetRenderData() const {

         return m_renderData;
      }
   };

}