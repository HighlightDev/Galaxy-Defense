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

   protected:

      SkeletalMeshRenderData m_renderData;

      float mUpdateDataResetTimeCounter;

      float update_data_reset_time;

      /* src is the main animation time counter*/
      float mSrcAnimationTime;

      /* dst time is used when blending of animations is being calculated*/
      float mDstAnimationTime;

      /* src is the main animation name*/
      std::string mSrcAnimationName;

      /* dst animation name is used when blending of animations is being occurred*/
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

      float& GetSrcAnimationTimeRef() {
         return mSrcAnimationTime;
      }

      float& GetDstAnimationTimeRef() {
         return mDstAnimationTime;
      }

      std::string& GetSrcAnimationNameRef() {
         return mSrcAnimationName;
      }

      std::string& GetDstAnimationNameRef() {
         return mDstAnimationName;
      }

      float& GetTransitionValueRef() {
         return mTransitionValue;
      }

      bool& GetIsTransitionEnabledRef() {
         return bTransitionEnabled;
      }

   };

}
