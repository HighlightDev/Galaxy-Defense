#pragma once

#include "PrimitiveComponent.h"
#include "Core/GraphicsCore/RenderData/SkeletalMeshRenderData.h"

using namespace Graphics::Data;

namespace Game
{

   class SkeletalMeshComponent :
      public PrimitiveComponent
   {

      using Base = PrimitiveComponent;

      SkeletalMeshRenderData m_renderData;

      float m_animationDeltaTime;

   public:

      SkeletalMeshComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const SkeletalMeshRenderData& renderData);

      virtual ~SkeletalMeshComponent();

      virtual uint64_t GetComponentType() const override;

      virtual void Tick(float deltaTime) override;

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      inline const SkeletalMeshRenderData& GetRenderData() const {

         return m_renderData;
      }

      float GetAnimationDeltaTime() const;
   };

}
