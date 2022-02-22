#pragma once

#include <vector>

#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkeletalMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GraphicsCore/Mesh/AnimationPlayer.h"

using namespace EngineCore::ShaderImpl;

namespace Graphics
{
   namespace Proxy
   {

      class SkeletalMeshSceneProxy :
         public PrimitiveSceneProxy
      {
         using ShaderType = VertexFactoryMaterialCompositeShader<SkeletalMeshVertexFactory<4>, SimpleShader>;
         using PlanarReflectionShaderType = VertexFactoryMaterialCompositeShader<SkeletalMeshVertexFactory<4>, CapturePlanarReflectionShader>;
         using Base = PrimitiveSceneProxy;

         mutable std::shared_ptr<AnimationPlayer> mAnimationPlayer;

         mutable bool bAnimationDataIsDirty = true;

      private:

         std::shared_ptr<ShaderType> GetShader() const;

         std::shared_ptr<PlanarReflectionShaderType> GetPlanarReflectionShader() const;

      public:
         SkeletalMeshSceneProxy(const SkeletalMeshComponent* component);

         virtual ~SkeletalMeshSceneProxy();

         virtual void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

         virtual void RenderPlanarReflection(const glm::vec4& plane, const glm::mat4& mirrorMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

         void UpdateAnimationData(bool transtionEnabled, const float transitionValue, const float srcAnimationTime,
            const float dstAnimationTime, const std::string& srcAnimationName, const std::string& dstAnimationName);

         void UpdateAnimationData(bool transtionEnabled, const float transitionValue, const float srcAnimationTime,
            const float dstAnimationTime, const size_t srcAnimationIndex, const size_t dstAnimationIndex);

         virtual ePrimitiveProxyType GetPrimitiveProxyType() const override;

         const std::vector<glm::mat4>& GetSkinningMatrices() const;

         virtual bool IsDeferred() const;

         virtual eMeshFacing GetMeshFrontFace() const override;
      };

   }
}
