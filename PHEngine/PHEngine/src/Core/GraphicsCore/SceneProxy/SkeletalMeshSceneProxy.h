#pragma once

#include <vector>

#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/ShaderImplementation/SkeletalMeshShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkeletalMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GraphicsCore/Mesh/AnimationPlayer.h"

namespace Game
{
   class StateMachine;
}

using namespace Game::ShaderImpl;

namespace Graphics
{
   namespace Proxy
   {

      class SkeletalMeshSceneProxy :
         public PrimitiveSceneProxy
      {
         using ShaderType = CompositeShader<SkeletalMeshVertexFactory<4>, SimpleShader>;
         using Base = PrimitiveSceneProxy;

         bool bAnimationTransformationDirty = true;

         std::shared_ptr<AnimationPlayer> mAnimationPlayer;

      private:

         std::shared_ptr<ShaderType> GetShader() const;

      public:
         SkeletalMeshSceneProxy(const SkeletalMeshComponent* component);

         virtual ~SkeletalMeshSceneProxy();

         virtual void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

         float mSrcAnimationTime;

         /* this time is used when blending of animations is being calculated*/
         float mDstAnimationTime;

         /* this is the main animation name*/
         std::string mSrcAnimationName;

         /* this animation name is used when blending of animations is being occurred*/
         std::string mDstAnimationName;

         float mTransitionParameter = 0.0f;

         bool bTransitionEnabled = false;

         void SetAnimationData(bool transtionEnabled, const float transitionValue, const float srcAnimationTime,
            const float dstAnimationTime, const std::string& srcAnimationName, const std::string& dstAnimationName);

         virtual uint64_t GetComponentType() const override;

         std::vector<glm::mat4> GetSkinningMatrices();

         virtual bool IsDeferred() const;
      };

   }
}
