#pragma once
#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/CubemapComponent.h"
#include "Core/GameCore/ShaderImplementation/CubemapShader.h"

using namespace Game;
using namespace Game::ShaderImpl;

namespace Graphics
{
   namespace Proxy
   {

      class CubemapSceneProxy :
         public PrimitiveSceneProxy
      {

         std::shared_ptr<CubemapShader> m_shaderCubemap;

         TextureAtlasSpaceRequest m_textureObtainer;

      protected:

         using Base = PrimitiveSceneProxy;

      public:

         CubemapSceneProxy(const CubemapComponent* component);

         ~CubemapSceneProxy();

         std::shared_ptr<IShader> GetShader() const;

         virtual void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

         virtual bool IsDeferred() const;
      };

   }
}

