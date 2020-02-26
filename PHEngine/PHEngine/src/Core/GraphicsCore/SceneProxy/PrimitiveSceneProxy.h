#pragma once

#include <memory>

#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GameCore/Components/ComponentType.h"
#include "Core/GameCore/GameObject.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"

using namespace Graphics::OpenGL;
using namespace Graphics::Mesh;
using namespace Graphics::Texture;
using namespace Game;

namespace Graphics
{
   namespace Proxy
   {
      class PrimitiveSceneProxy
         : public GameObject
      {
      protected:

         glm::mat4 m_relativeMatrix;

         std::shared_ptr<Skin> m_skin;

         std::shared_ptr<ICompositeShader> m_shader;

      public:

         PrimitiveSceneProxy(glm::mat4 relativeMatrix, std::shared_ptr<Skin> skin, std::shared_ptr<ICompositeShader> shader);

         virtual ~PrimitiveSceneProxy();

         virtual void PostConstructorInitialize();

         virtual void Render(glm::mat4& viewMatrix, glm::mat4& projectionMatrix) = 0;

         virtual glm::mat4 GetMatrix() const;

         void SetTransformationMatrix(const glm::mat4& relativeMatrix);

         virtual std::shared_ptr<Skin> GetSkin() const;

         virtual uint64_t GetComponentType() const;

         virtual bool IsDeferred() const = 0;
         
      };

   }
}

