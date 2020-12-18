#pragma once

#include <memory>

#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GameCore/Components/ComponentType.h"
#include "Core/GameCore/GameObject.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"
#include "Core/GraphicsCore/SceneViewInfo/AVisiblePrimitiveBase.h"
#include "Core/GraphicsCore/SceneProxy/SceneProxyBase.h"

using namespace Graphics::OpenGL;
using namespace Graphics::Mesh;
using namespace Graphics::Texture;
using namespace Game;

namespace Graphics
{
   namespace Proxy
   {
      enum class PrimitiveProxyType
      {
         PRIMITIVE_PROXY,
         STATIC_MESH_PROXY,
         SKELETAL_MESH_PROXY
      };

      class PrimitiveSceneProxy
         : public SceneProxyBase
         , public AVisiblePrimitiveBase
      {
      protected:

         glm::mat4 m_relativeMatrix;

         std::shared_ptr<Skin> m_skin;

         std::shared_ptr<ICompositeShader> m_shader;

         std::shared_ptr<IMaterial> mMaterialInstance;

      public:

         PrimitiveSceneProxy(glm::mat4 relativeMatrix, std::shared_ptr<Skin> skin, std::shared_ptr<ICompositeShader> shader, std::shared_ptr<IMaterial> materialInstance);

         virtual ~PrimitiveSceneProxy();

         void SetTransformationMatrix(const glm::mat4& relativeMatrix);

         virtual glm::mat4 GetMatrix() const;

         virtual void PostConstructorInitialize();

         virtual std::shared_ptr<Skin> GetSkin() const;

         virtual PrimitiveProxyType GetPrimitiveProxyType() const;

         virtual bool IsFrustumCullTestNeeded() const override;

         virtual void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) = 0;

         virtual bool IsDeferred() const = 0;

      };

   }
}

