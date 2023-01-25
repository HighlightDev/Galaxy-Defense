#pragma once
#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/WaterPlaneComponent.h"
#include "Core/GameCore/ShaderImplementation/WaterPlaneShader.h"

#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"

using namespace EngineCore;
using namespace EngineCore::ShaderImpl;

namespace Graphics
{
   namespace Proxy
   {

      class WaterPlaneSceneProxy : public PrimitiveSceneProxy
      {
         using ShaderType = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, SimpleShader>;

         float m_moveFactor;
         float m_waveStrength;
         float m_transparencyDepth;
         float m_nearClipPlane;
         float m_farClipPlane;

      public:
         std::shared_ptr<ShaderType> GetShader() const;

         WaterPlaneSceneProxy(const WaterPlaneComponent *component);

         ~WaterPlaneSceneProxy() override;

         void PostConstructorInitialize() override;

         void Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) override;

         virtual bool IsDeferred() const;

         eMeshFacing GetMeshFrontFace() const override;

         ePrimitiveProxyType GetPrimitiveProxyType() const override;

         void SetMoveFactor(float moveFactor);
         void SetWaveStrength(float waveStr);
         void SetTransparencyDepth(float transparencyDepth);
         void SetNearClipPlane(float nearClipPlane);
         void SetFarClipPlane(float farClipPlane);

      private:
         void Init();
      };
   }
}
