#pragma once
#include "StaticMeshSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedMeshPoolParameters.h"

#include <glm/vec3.hpp>

using namespace EngineCore;
using namespace EngineCore::ShaderImpl;

namespace Graphics
{
   namespace Proxy
   {
      class RuntimeGeneratedLineSceneProxy : public StaticMeshSceneProxy
      {
         using Base = PrimitiveSceneProxy;
         using ShaderType = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, SimpleShader>;
         using PlanarReflectionShaderType = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, CapturePlanarReflectionShader>;

         RuntimeGeneratedMeshPoolParameters mRtMeshPoolParams;

         glm::vec3 mLineBeginWorldSpacePosition;

         glm::vec3 mLineEndWorldSpacePosition;

         float mLineWidth;

         bool bUpdateLineGeometry{true};

      public:
         RuntimeGeneratedLineSceneProxy(const RuntimeGeneratedLineComponent *component);

         void PostConstructorInitialize() override;

         void Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) override;

         void SetLineBeginWorldSpacePosition(const glm::vec3 &position);

         void SetLineEndWorldSpacePosition(const glm::vec3 &position);

         void SetLineWidth(const float lineWidth);

         bool CanBloomBeApplied() const override;

      private:
         void UpdateGeometry(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix);
      };

   }
}
