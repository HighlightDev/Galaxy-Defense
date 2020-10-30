#pragma once

#include <vector>

#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/ShaderImplementation/SkeletalMeshShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkeletalMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GraphicsCore/Mesh/AnimationPlayer.h"

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

         std::shared_ptr<AnimationPlayer> mAnimationPlayer;

         bool bIsDirty = true;

      private:

         std::shared_ptr<ShaderType> GetShader() const;

      public:
         SkeletalMeshSceneProxy(const SkeletalMeshComponent* component);

         virtual ~SkeletalMeshSceneProxy();

         virtual void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

         void UpdateAnimationData(bool transtionEnabled, const float transitionValue, const float srcAnimationTime,
            const float dstAnimationTime, const std::string& srcAnimationName, const std::string& dstAnimationName);

         void UpdateAnimationData(bool transtionEnabled, const float transitionValue, const float srcAnimationTime,
            const float dstAnimationTime, const size_t srcAnimationIndex, const size_t dstAnimationIndex);

         virtual PrimitiveProxyType GetPrimitiveProxyType() const override;

         const std::vector<glm::mat4>& GetSkinningMatrices();

         virtual bool IsDeferred() const;
      };

   }
}
