#pragma once
#include "PrimitiveComponent.h"
#include "Core/GraphicsCore/RenderData/ForwardShadingMeshRenderData.h"

using namespace Graphics::Data;

namespace EngineCore
{
   struct ForwardShadingMeshComponentData;

   enum WaterQualityFlag
   {
      REFLECT_STATIC_MESH = 0x000001,
      REFLECT_SKELETAL_MESH = 0x000010,
      REFLECT_PARTICLES = 0x000100,
      REFRACT_STATIC_MESH = 0x001000,
      REFRACT_SKELETAL_MESH = 0x010000,
      REFRACT_PARTICLES = 0x100000
   };

   class WaterPlaneComponent : public PrimitiveComponent
   {

      float m_transparencyDepth;
      float m_waveSpeed;
      float m_waveStrength;
      float m_nearClipPlane;
      float m_farClipPlane;
      WaterQualityFlag m_waterQuality;
      ForwardShadingMeshRenderData m_renderData;

      bool bIsRenderDataDirty{false};

   public:
      using Base = PrimitiveComponent;

      WaterPlaneComponent(const std::shared_ptr<ForwardShadingMeshComponentData> &data,
                          const ForwardShadingMeshRenderData &renderData,
                          WaterQualityFlag waterQuality = (WaterQualityFlag)(REFLECT_SKELETAL_MESH | REFRACT_STATIC_MESH | REFLECT_STATIC_MESH | REFRACT_SKELETAL_MESH));

      ~WaterPlaneComponent() override;

      eComponentType GetComponentType() const override;

      void Tick(const float deltaTime) override;

      inline const ForwardShadingMeshRenderData &GetRenderData() const
      {
         return m_renderData;
      }

      float GetWaveStrength() const;

      float GetTransparencyDepth() const;

      float GetNearClipPlane() const;

      float GetFarClipPlane() const;

      void SetWaveStrength(float waveStr);

      void SetTransparencyDepth(float transparencyDepth);

      void SetNearClipPlane(float nearClipPlane);

      void SetFarClipPlane(float farClipPlane);

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const;

   private:
      void SyncRenderData();
   };

}
