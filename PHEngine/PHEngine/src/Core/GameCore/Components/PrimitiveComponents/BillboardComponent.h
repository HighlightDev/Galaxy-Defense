#pragma once
#include "PrimitiveComponent.h"
#include "Core/GameCore/ShaderImplementation/BillboardShader.h"
#include "Core/GraphicsCore/RenderData/BillboardRenderData.h"

#include <glm/mat4x4.hpp>
#include <memory>

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

   public:
      BillboardComponent(const std::shared_ptr<BillboardComponentData> &data, const BillboardRenderData &renderData);

      ~BillboardComponent() override;

      eComponentType GetComponentType() const override;

      void UnpausableTick(const float deltaTime) override;

      void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

      std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      void SetBillboardExtent(const float extent);

      float GetBillboardExtent() const;

      void SetIsEnabled(const bool bEnabled) override;

      void SetIsVisible(bool isVisible) override;

      std::shared_ptr<IMaterial> GetMaterial() const;

      inline const BillboardRenderData &GetRenderData() const
      {
         return m_renderData;
      }

   private:
      void SyncRenderData();
   };

}
