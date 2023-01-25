#pragma once

#include <vector>

#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkeletalMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GraphicsCore/Mesh/AnimationPlayer.h"
#include "Core/GraphicsCore/RenderData/SkeletalMeshRenderData.h"

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;

namespace Graphics
{
   namespace Proxy
   {

      class SkeletalMeshSceneProxy
          : public PrimitiveSceneProxy
      {
         using ShaderType = VertexFactoryMaterialCompositeShader<SkeletalMeshVertexFactory<4>, SimpleShader>;
         using PlanarReflectionShaderType = VertexFactoryMaterialCompositeShader<SkeletalMeshVertexFactory<4>, CapturePlanarReflectionShader>;
         using Base = PrimitiveSceneProxy;

         SkeletalMeshRenderData mRenderData;

      protected:
         mutable std::shared_ptr<AnimationPlayer> mAnimationPlayer;

      private:
         mutable bool bAnimationDataIsDirty = true;

      private:
         std::shared_ptr<ShaderType> GetShader() const;

         std::shared_ptr<PlanarReflectionShaderType> GetPlanarReflectionShader() const;

      public:
         SkeletalMeshSceneProxy(const SkeletalMeshComponent *component);

         ~SkeletalMeshSceneProxy() override;

         void PostConstructorInitialize() override;

         void Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix,
                     const glm::mat4 &projectionMatrix) override;

         void RenderPlanarReflection(const glm::vec4 &plane,
                                     const glm::mat4 &mirrorMatrix,
                                     const glm::mat4 &viewMatrix,
                                     const glm::mat4 &projectionMatrix) override;

         void UpdateAnimationData(bool transtionEnabled,
                                  const float transitionValue,
                                  const float srcAnimationTime,
                                  const float dstAnimationTime,
                                  const std::string &srcAnimationName,
                                  const std::string &dstAnimationName);

         void UpdateAnimationData(bool transtionEnabled,
                                  const float transitionValue,
                                  const float srcAnimationTime,
                                  const float dstAnimationTime,
                                  const size_t srcAnimationIndex,
                                  const size_t dstAnimationIndex);

         ePrimitiveProxyType GetPrimitiveProxyType() const override;

         const std::vector<glm::mat4> &GetSkinningMatrices() const;

         virtual bool IsDeferred() const;

         eMeshFacing GetMeshFrontFace() const override;
      };

   }
}
