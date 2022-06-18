#pragma once

#include <memory>

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GameCore/GameObject.h"
#include "Core/GraphicsCore/Material/MaterialProxy.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GraphicsCore/SceneViewInfo/AProxyVisibilityController.h"
#include "Core/GraphicsCore/SceneProxy/SceneProxyBase.h"

using namespace Graphics::OpenGL;
using namespace Graphics::Mesh;
using namespace Graphics::Texture;
using namespace EngineCore;

namespace Graphics
{
   namespace Proxy
   {
      enum class ePrimitiveProxyType
      {
         PRIMITIVE_PROXY,
         STATIC_MESH_PROXY,
         SKELETAL_MESH_PROXY
      };

      enum class eMeshFacing
      {
         CLOCK_WISE,
         COUNTER_CLOCK_WISE,
      };

      class PrimitiveSceneProxy
          : public SceneProxyBase,
            public AProxyVisibilityController
      {

         bool bTransformInitialized;

      protected:
         glm::mat4 m_relativeMatrix;

         std::shared_ptr<Skin> m_skin;

         std::shared_ptr<IShader> m_shader;

         std::shared_ptr<IShader> m_planarReflectionShader;

         std::shared_ptr<MaterialProxy> mMaterialProxy;

      public:
         PrimitiveSceneProxy(const bool isEnabled,
                             const bool isVisible,
                             const glm::mat4 &relativeMatrix,
                             const std::shared_ptr<Skin> &skin,
                             const std::shared_ptr<IShader> &materialShader,
                             const std::shared_ptr<IShader> &planarReflectionShader,
                             const std::shared_ptr<MaterialProxy> &materialProxy);

         virtual ~PrimitiveSceneProxy();

         void SetTransformationMatrix(const glm::mat4 &relativeMatrix);

         virtual glm::mat4 GetMatrix() const;

         virtual void PostConstructorInitialize();

         virtual std::shared_ptr<Skin> GetSkin() const;

         virtual ePrimitiveProxyType GetPrimitiveProxyType() const;

         virtual bool IsFrustumCullTestNeeded() const override;

         virtual void Render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) = 0;

         virtual void RenderPlanarReflection(const glm::vec4 &plane, const glm::mat4 &mirrorMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {}

         virtual bool IsDeferred() const = 0;

         virtual eMeshFacing GetMeshFrontFace() const = 0;

         virtual bool IsTransformIntialized() const;
      };

   }
}
