#pragma once

#include "PrimitiveComponent.h"
#include "Core/GraphicsCore/RenderData/SkeletalMeshRenderData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

using namespace Graphics;
using namespace Graphics::Data;

namespace EngineCore
{
   namespace Scripts
   {
      class LuaWrapper;
   }

   struct MeshComponentData;
   class SkeletalMeshComponent
       : public PrimitiveComponent
   {
      using Base = PrimitiveComponent;

      bool bIsRenderDataDirty{false};

   protected:
      // todo: should do something with render data on game thread.....
      SkeletalMeshRenderData m_renderData;

      std::string mLuaScriptAbsPath;

      std::unique_ptr<::EngineCore::Scripts::LuaWrapper> mLuaInstance;

      float mUpdateDataResetTimeCounter;

      float update_data_reset_time;

      float mTimeIncreaseMultiply;

   public:
      std::string LuaScriptName;

      /* src is the main animation time counter*/
      std::shared_ptr<EngineGOProperty<float>> SrcAnimationTime;

      /* dst time is used when blending of animations is being calculated*/
      std::shared_ptr<EngineGOProperty<float>> DstAnimationTime;

      /* src is the main animation name*/
      std::shared_ptr<EngineGOProperty<std::string>> SrcAnimationName;

      /* dst animation name is used when blending of animations is being occurred*/
      std::shared_ptr<EngineGOProperty<std::string>> DstAnimationName;

      std::shared_ptr<EngineGOProperty<bool>> bTransitionEnabled;

      std::shared_ptr<EngineGOProperty<float>> TransitionValue;

   public:
      SkeletalMeshComponent(const MeshComponentData &meshComponentData, const SkeletalMeshRenderData &renderData);

      ~SkeletalMeshComponent() override;

      void PostLevelInit() override;

      void SetIsEnabled(const bool bEnabled) override;

      void SetIsVisible(bool isVisible) override;

      eComponentType GetComponentType() const override;

      void Tick(const float deltaTime) override;

      void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

      std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      inline const SkeletalMeshRenderData &GetRenderData() const
      {

         return m_renderData;
      }

      std::shared_ptr<IMaterial> GetMaterial() const;

   protected:
      void SyncDataWithRenderThread();
   };

}
