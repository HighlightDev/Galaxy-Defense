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

      int32_t m_tickCounter = 0;

   protected:

      /* this is the main animation time counter*/
      float mSrcAnimationTime;

      /* this time is used when blending of animations is being calculated*/
      float mDstAnimationTime;

      /* this is the main animation name*/
      std::string mSrcAnimationName;

      /* this animation name is used when blending of animations is being occurred*/
      std::string mDstAnimationName;

      bool bTransitionEnabled;

      float mTransitionValue;

   public:

      SkeletalMeshComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const SkeletalMeshRenderData& renderData);

      virtual ~SkeletalMeshComponent();

      virtual uint64_t GetComponentType() const override;

      virtual void Tick(float deltaTime) override;

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      inline const SkeletalMeshRenderData& GetRenderData() const {

         return m_renderData;
      }
   };

}
