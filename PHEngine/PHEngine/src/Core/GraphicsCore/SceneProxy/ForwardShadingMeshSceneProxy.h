#pragma once
#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/ForwardShadingMeshComponent.h"

#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GraphicsCore/RenderData/ForwardShadingMeshRenderData.h"

using namespace EngineCore;
using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;

namespace Graphics
{
   namespace Proxy
   {
      class ForwardShadingMeshSceneProxy
          : public PrimitiveSceneProxy
      {
         using ShaderType = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, SimpleShader>;

      protected:
         ForwardShadingMeshRenderData mRenderData;

      public:
         std::shared_ptr<ShaderType> GetShader() const;

         ForwardShadingMeshSceneProxy(const ForwardShadingMeshComponent *component);

         void PostConstructorInitialize() override;

         void Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) override;

         virtual bool IsDeferred() const;

         eMeshFacing GetMeshFrontFace() const override;

         ePrimitiveProxyType GetPrimitiveProxyType() const override;
      };
   }
}
