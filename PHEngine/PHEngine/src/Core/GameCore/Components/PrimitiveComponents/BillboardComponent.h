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

   class BillboardComponent : public PrimitiveComponent
   {
   protected:
      using Base = PrimitiveComponent;

      float mBillboardExtent;

   private:
      BillboardRenderData m_renderData;

      bool bIsExtentDataDirty{false};
      bool bIsTextureDataDirty{false};

   public:
      BillboardComponent(const BillboardComponentData &data, const BillboardRenderData &renderData);

      ~BillboardComponent() override;

      eComponentType GetComponentType() const override;

      void Tick(float deltaTime) override;

      void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

      std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      void SetBillboardExtent(const float extent);

      float GetBillboardExtent() const;

      void SetBillboardTexture(const std::shared_ptr<ITexture> &texture);

      inline const BillboardRenderData &GetRenderData() const
      {

         return m_renderData;
      }

   private:
      void SyncRenderData();
   };

}
