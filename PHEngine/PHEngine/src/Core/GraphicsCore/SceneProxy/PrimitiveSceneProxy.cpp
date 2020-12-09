#include "PrimitiveSceneProxy.h"


namespace Graphics
{
   namespace Proxy
   {

      PrimitiveSceneProxy::PrimitiveSceneProxy(glm::mat4 relativeMatrix, std::shared_ptr<Skin> skin, std::shared_ptr<ICompositeShader> materialShader,
         std::shared_ptr<IMaterial> materialInstance)
         : SceneProxyBase()
         , AVisiblePrimitiveBase()
         , m_relativeMatrix(relativeMatrix)
         , m_skin(skin)
         , m_shader(materialShader)
         , mMaterialInstance(materialInstance)
      {
      }

      PrimitiveSceneProxy::~PrimitiveSceneProxy()
      {
      }

      void PrimitiveSceneProxy::PostConstructorInitialize()
      {
      }

      glm::mat4 PrimitiveSceneProxy::GetMatrix() const
      {
         return m_relativeMatrix;
      }

      void PrimitiveSceneProxy::SetTransformationMatrix(const glm::mat4& relativeMatrix)
      {
         m_relativeMatrix = relativeMatrix;
      }

      std::shared_ptr<Skin> PrimitiveSceneProxy::GetSkin() const
      {
         return m_skin;
      }

      PrimitiveProxyType PrimitiveSceneProxy::GetPrimitiveProxyType() const
      {
         return PrimitiveProxyType::PRIMITIVE_PROXY;
      }

      const BoundingBox& PrimitiveSceneProxy::GetTransformedBoundingBox() const
      {
         return mBoundingBox;
      }

   }
}