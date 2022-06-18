#pragma once
#include "PrimitiveComponent.h"
#include "Core/GameCore/ShaderImplementation/CubemapShader.h"
#include "Core/GraphicsCore/RenderData/CubemapRenderData.h"

#include <glm/mat4x4.hpp>

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;

namespace EngineCore
{
   struct CubemapComponentData;

   class CubemapComponent :
      public PrimitiveComponent
   {
   protected:

      using Base = PrimitiveComponent;

   private:

      CubemapRenderData m_renderData;

   public:

      CubemapComponent(const CubemapComponentData& data, const CubemapRenderData& renderData);

      virtual ~CubemapComponent();

      virtual eComponentType GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      inline const CubemapRenderData& GetRenderData() const {

         return m_renderData;
      }
   };

}

