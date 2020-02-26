#pragma once

#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/ShaderImplementation/SkyboxShader.h"
#include "Core/GameCore/Components/SkyboxComponent.h"

using namespace Game::ShaderImpl;
using namespace Game;

namespace Graphics
{
   namespace Proxy
   {

      class SkyboxSceneProxy :
         public PrimitiveSceneProxy
      {
         std::shared_ptr<SkyboxShader> m_skyboxShader;
         std::shared_ptr<ITexture> m_dayTexture;
         std::shared_ptr<ITexture> m_nightTexture;

      protected:

         using Base = PrimitiveSceneProxy;

      public:

         SkyboxSceneProxy(const SkyboxComponent* component);

         ~SkyboxSceneProxy();

         virtual void PostConstructorInitialize() override;

         virtual void Render(glm::mat4& viewMatrix, glm::mat4& projectionMatrix) override;

         virtual uint64_t GetComponentType() const override;

         virtual bool IsDeferred() const;
      };

   }
}

