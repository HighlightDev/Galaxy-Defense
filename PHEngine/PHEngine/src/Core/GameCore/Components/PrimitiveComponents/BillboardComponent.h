#pragma once
#include "PrimitiveComponent.h"
#include "Core/GameCore/ShaderImplementation/BillboardShader.h"
#include "Core/GraphicsCore/RenderData/BillboardRenderData.h"

#include <glm/mat4x4.hpp>

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;

namespace EngineCore
{
   struct BillboardComponentData;

   class BillboardComponent :
      public PrimitiveComponent
   {
   protected:

      using Base = PrimitiveComponent;

      float mBillboardExtent;

   private:

      BillboardRenderData m_renderData;

   public:

      BillboardComponent(const BillboardComponentData& data, const BillboardRenderData& renderData);

      virtual ~BillboardComponent();

      virtual eComponentType GetComponentType() const override;

      virtual void Tick(float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      void SetBillboardExtent(const float extent);

      float GetBillboardExtent() const;

      void SetBillboardTexture(const std::shared_ptr<ITexture>& texture);

      inline const BillboardRenderData& GetRenderData() const {

         return m_renderData;
      }
   };

}

