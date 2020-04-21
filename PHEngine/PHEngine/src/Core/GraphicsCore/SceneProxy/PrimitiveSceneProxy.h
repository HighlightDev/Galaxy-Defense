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

         bool mIsVisible;

         glm::mat4 m_relativeMatrix;

         std::shared_ptr<Skin> m_skin;

         std::shared_ptr<ICompositeShader> m_shader;

         std::shared_ptr<IMaterial> mMaterialInstance;

      public:

         PrimitiveSceneProxy(glm::mat4 relativeMatrix, std::shared_ptr<Skin> skin, std::shared_ptr<ICompositeShader> shader, std::shared_ptr<IMaterial> materialInstance);

         virtual ~PrimitiveSceneProxy();

         void SetTransformationMatrix(const glm::mat4& relativeMatrix);

         void SetVisibility(const bool visibility);

         virtual glm::mat4 GetMatrix() const;

         virtual void PostConstructorInitialize();

         virtual std::shared_ptr<Skin> GetSkin() const;

         virtual uint64_t GetComponentType() const;

         virtual void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) = 0;

         virtual bool IsDeferred() const = 0;

         inline bool IsVisible() const {
            return mIsVisible;
         }
      };

   }
}

