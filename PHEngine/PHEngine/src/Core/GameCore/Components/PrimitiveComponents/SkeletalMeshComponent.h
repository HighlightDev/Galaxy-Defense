#pragma once

#include "PrimitiveComponent.h"
#include "Core/GraphicsCore/RenderData/SkeletalMeshRenderData.h"

using namespace Graphics::Data;

namespace Game
{

   class LuaWrapper;

   class SkeletalMeshComponent :
      public PrimitiveComponent
   {

      using Base = PrimitiveComponent;

   protected:

      SkeletalMeshRenderData m_renderData;

      std::string mLuaScriptRelPath;

      std::unique_ptr<LuaWrapper> mLuaInstance;

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

      SkeletalMeshComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const std::string& mLuaScriptRelPath, const SkeletalMeshRenderData& renderData);

      virtual ~SkeletalMeshComponent();

      virtual uint64_t GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      inline const SkeletalMeshRenderData& GetRenderData() const {

         return m_renderData;
      }

      float* GetSrcAnimationTimePtr() {
         return &mSrcAnimationTime;
      }

      float* GetDstAnimationTimePtr() {
         return &mDstAnimationTime;
      }

      std::string* GetSrcAnimationNamePtr() {
         return &mSrcAnimationName;
      }

      std::string* GetDstAnimationNamePtr() {
         return &mDstAnimationName;
      }

      float* GetTransitionValuePtr() {
         return &mTransitionValue;
      }

      bool* GetIsTransitionEnabledPtr() {
         return &bTransitionEnabled;
      }

   protected:

      void SyncDataWithRenderThread();

   };

}
