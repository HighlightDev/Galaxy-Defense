#pragma once
#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"

using namespace EngineCore;
using namespace EngineCore::ShaderImpl;

namespace Graphics
{
   namespace Proxy
   {

      class StaticMeshSceneProxy : public PrimitiveSceneProxy
      {

         using Base = PrimitiveSceneProxy;
         using ShaderType = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, SimpleShader>;
         using PlanarReflectionShaderType = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, CapturePlanarReflectionShader>;

      private:
         bool mIsDeferredShaded;

      private:
         std::shared_ptr<ShaderType> GetShader() const;

         std::shared_ptr<PlanarReflectionShaderType> GetPlanarReflectionShader() const;

      public:
         StaticMeshSceneProxy(const StaticMeshComponent *component);

         ~StaticMeshSceneProxy();

         virtual void Render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) override;

         virtual void RenderPlanarReflection(const glm::vec4 &plane, const glm::mat4 &mirrorMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) override;
         
         virtual bool IsDeferred() const override;

         virtual eMeshFacing GetMeshFrontFace() const override;

         virtual ePrimitiveProxyType GetPrimitiveProxyType() const override;
      };

   }
}
