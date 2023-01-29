#pragma once
#include "BillboardComponent.h"

using namespace Graphics::Data;

namespace EngineCore
{
   struct BillboardComponentData;

   class FullscreenBillboardComponent : public BillboardComponent
   {
   protected:
      using Base = BillboardComponent;

   public:
      FullscreenBillboardComponent(const BillboardComponentData &data, const BillboardRenderData &renderData);

      ~FullscreenBillboardComponent() override;

      std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;
   };

}
