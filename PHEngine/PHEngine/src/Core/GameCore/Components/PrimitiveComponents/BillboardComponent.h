#pragma once
#include "PrimitiveComponent.h"
#include "Core/GameCore/ShaderImplementation/BillboardShader.h"
#include "Core/GraphicsCore/RenderData/BillboardRenderData.h"

#include <glm/mat4x4.hpp>

using namespace Game::ShaderImpl;
using namespace Graphics::Data;

namespace Game
{

   class BillboardComponent :
      public PrimitiveComponent
   {
   protected:

      using Base = PrimitiveComponent;

   private:

      BillboardRenderData m_renderData;

   public:

      BillboardComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const BillboardRenderData& renderData);

      virtual ~BillboardComponent();

      virtual uint64_t GetComponentType() const override;

      virtual void Tick(float deltaTime) override;

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      inline const BillboardRenderData& GetRenderData() const {

         return m_renderData;
      }
   };

}

