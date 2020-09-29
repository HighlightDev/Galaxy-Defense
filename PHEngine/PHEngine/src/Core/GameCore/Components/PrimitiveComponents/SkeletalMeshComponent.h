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

      std::string mLuaScriptAbsPath;

      std::unique_ptr<LuaWrapper> mLuaInstance;

      float mUpdateDataResetTimeCounter;

      float update_data_reset_time;

   public:

      /* src is the main animation time counter*/
      GenericObjectProperty<float> SrcAnimationTime;

      /* dst time is used when blending of animations is being calculated*/
      GenericObjectProperty<float> DstAnimationTime;

      /* src is the main animation name*/
      GenericObjectProperty<std::string> SrcAnimationName;

      /* dst animation name is used when blending of animations is being occurred*/
      GenericObjectProperty<std::string> DstAnimationName;

      GenericObjectProperty<bool> bTransitionEnabled;

      GenericObjectProperty<float> TransitionValue;

   public:

      SkeletalMeshComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const std::string& luaScriptAbsPath, const SkeletalMeshRenderData& renderData);

      virtual ~SkeletalMeshComponent();

      virtual uint64_t GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      inline const SkeletalMeshRenderData& GetRenderData() const {

         return m_renderData;
      }

   protected:

      void SyncDataWithRenderThread();

   };

}
