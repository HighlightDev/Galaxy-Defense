#pragma once
#include "PrimitiveComponent.h"
#include "Core/GameCore/ShaderImplementation/CubemapShader.h"
#include "Core/GraphicsCore/RenderData/CubemapRenderData.h"

#include <glm/mat4x4.hpp>

using namespace Game::ShaderImpl;
using namespace Graphics::Data;

namespace Game
{

   class CubemapComponent :
      public PrimitiveComponent
   {
   protected:

      using Base = PrimitiveComponent;

   private:

      CubemapRenderData m_renderData;

   public:

      CubemapComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const CubemapRenderData& renderData);

      virtual ~CubemapComponent();

      virtual uint64_t GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      inline const CubemapRenderData& GetRenderData() const {

         return m_renderData;
      }
   };

}

