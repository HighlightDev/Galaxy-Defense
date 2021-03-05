#pragma once
#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/WaterPlaneComponent.h"
#include "Core/GameCore/FramebufferImplementation/WaterPlaneFramebuffer.h"
#include "Core/GameCore/ShaderImplementation/WaterPlaneShader.h"

#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GraphicsCore/Material/WaterDynamicMaterial.h"

using namespace Game;
using namespace Game::FramebufferImpl;
using namespace Game::ShaderImpl;

namespace Graphics
{
   namespace Proxy
   {

      class WaterPlaneSceneProxy :
         public PrimitiveSceneProxy
      {
         using ShaderType = CompositeShader<StaticMeshVertexFactory, SimpleShader>;

         std::unique_ptr<WaterPlaneFramebuffer> m_waterPlaneFramebuffer;

         float m_moveFactor;
         float m_waveStrength;
         float m_transparencyDepth;
         float m_nearClipPlane;
         float m_farClipPlane;

      public:
      
         std::shared_ptr<ShaderType> GetShader() const;

         WaterPlaneSceneProxy(const WaterPlaneComponent* component);

         virtual ~WaterPlaneSceneProxy();

         virtual void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

         virtual bool IsDeferred() const;

         virtual PrimitiveProxyType GetPrimitiveProxyType() const override;

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

