#pragma once
#include "PrimitiveComponent.h"
#include "Core/GraphicsCore/RenderData/ForwardShadingMeshRenderData.h"

using namespace Graphics::Data;

namespace EngineCore
{
   struct ForwardShadingMeshComponentData;

   class ForwardShadingMeshComponent
       : public PrimitiveComponent
   {
      ForwardShadingMeshRenderData m_renderData;

      bool bIsRenderDataDirty{false};

   public:
      using Base = PrimitiveComponent;

      ForwardShadingMeshComponent(const std::shared_ptr<ForwardShadingMeshComponentData> &data,
                                  const ForwardShadingMeshRenderData &renderData);

      eComponentType GetComponentType() const override;

      inline const ForwardShadingMeshRenderData &GetRenderData() const
      {
         return m_renderData;
      }

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const;

   private:
      void SyncRenderData();
   };

}
