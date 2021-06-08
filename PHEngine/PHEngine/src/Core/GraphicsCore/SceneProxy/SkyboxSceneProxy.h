#pragma once

#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkyboxComponent.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkyboxVertexFactory.h"

using namespace Game::ShaderImpl;
using namespace Game;

namespace Graphics
{
   namespace Proxy
   {

      class SkyboxSceneProxy :
         public PrimitiveSceneProxy
      {
         using ShaderType = VertexFactoryMaterialCompositeShader<SkyboxVertexFactory, SimpleShader>;
         using PlanarReflectionShaderType = VertexFactoryMaterialCompositeShader<SkyboxVertexFactory, CapturePlanarReflectionShader>;

      protected:

         using Base = PrimitiveSceneProxy;

      public:

         std::shared_ptr<ShaderType> GetShader() const;

         std::shared_ptr<PlanarReflectionShaderType> GetPlanarReflectionShader() const;

         SkyboxSceneProxy(const SkyboxComponent* component);

         ~SkyboxSceneProxy();

         virtual void PostConstructorInitialize() override;

         virtual void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

         virtual void RenderPlanarReflection(const glm::vec4& plane, const glm::mat4& mirrorMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

         virtual ePrimitiveProxyType GetPrimitiveProxyType() const override;

         virtual bool IsFrustumCullTestNeeded() const override;

         virtual bool IsDeferred() const;

         virtual eMeshFacing GetMeshFrontFace() const override;
      };

   }
}

