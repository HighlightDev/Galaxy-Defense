#pragma once
#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/WaterPlaneComponent.h"
#include "Core/GameCore/FramebufferImplementation/WaterPlaneFramebuffer.h"
#include "Core/GameCore/ShaderImplementation/WaterPlaneShader.h"

#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"
#include "Core/GameCore/ShaderImplementation/DeferredCollectShader.h"
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
         using ShaderType = CompositeShader<StaticMeshVertexFactory, DeferredCollectShader>;
         using MaterialType = WaterDynamicMaterial;

         std::unique_ptr<WaterPlaneFramebuffer> m_waterPlaneFramebuffer;
         std::shared_ptr<ITexture> m_waterDistortionMap;
         std::shared_ptr<ITexture> m_waterNormalMap;

         float m_moveFactor;
         float m_waveStrength;
         float m_transparencyDepth;
         float m_nearClipPlane;
         float m_farClipPlane;

      public:
      
         std::shared_ptr<ShaderType> GetShader() const;

         std::shared_ptr<MaterialType> GetMaterialInstance() const;

         WaterPlaneSceneProxy(const WaterPlaneComponent* component);

         virtual ~WaterPlaneSceneProxy();

         virtual void Render(glm::mat4& viewMatrix, glm::mat4& projectionMatrix) override;

         virtual bool IsDeferred() const;

         virtual uint64_t GetComponentType() const override;

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

