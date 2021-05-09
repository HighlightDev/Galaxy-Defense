#include "PrimitiveSceneProxy.h"

namespace Graphics
{
   namespace Proxy
   {

      PrimitiveSceneProxy::PrimitiveSceneProxy(bool isVisible, glm::mat4 relativeMatrix, std::shared_ptr<Skin> skin,
         std::shared_ptr<IShader> materialShader,
         std::shared_ptr<IShader> planarReflectionShader,
         std::shared_ptr<MaterialProxy> materialProxy)
         : SceneProxyBase()
         , AProxyVisibilityController(isVisible)
         , m_relativeMatrix(relativeMatrix)
         , m_skin(skin)
         , m_shader(materialShader)
         , m_planarReflectionShader(planarReflectionShader)
         , mMaterialProxy(materialProxy)
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

      bool PrimitiveSceneProxy::IsFrustumCullTestNeeded() const
      {
         return true;
      }

   }
}