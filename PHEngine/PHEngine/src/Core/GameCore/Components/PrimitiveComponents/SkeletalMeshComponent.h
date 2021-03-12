#pragma once

#include "PrimitiveComponent.h"
#include "Core/GraphicsCore/RenderData/SkeletalMeshRenderData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

using namespace Graphics;
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

      float mTimeIncreaseMultiply;

   public:

      std::string LuaScriptName;

      /* src is the main animation time counter*/
      EngineGOProperty<float> SrcAnimationTime;

      /* dst time is used when blending of animations is being calculated*/
      EngineGOProperty<float> DstAnimationTime;

      /* src is the main animation name*/
      EngineGOProperty<std::string> SrcAnimationName;

      /* dst animation name is used when blending of animations is being occurred*/
      EngineGOProperty<std::string> DstAnimationName;

      EngineGOProperty<bool> bTransitionEnabled;

      EngineGOProperty<float> TransitionValue;

   public:

      SkeletalMeshComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const std::string& luaScriptAbsPath, const SkeletalMeshRenderData& renderData);

      virtual ~SkeletalMeshComponent();

      virtual void PostLevelInit() override;

      virtual ComponentType GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      inline const SkeletalMeshRenderData& GetRenderData() const {

         return m_renderData;
      }

      std::shared_ptr<IMaterial> GetMaterial() const;

   protected:

      void SyncDataWithRenderThread();

   };

}
