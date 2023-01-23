#include "PrimitiveSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"

using namespace Graphics::Renderer;

namespace Graphics
{
   namespace Proxy
   {

      PrimitiveSceneProxy::PrimitiveSceneProxy(const bool isEnabled,
                                               const bool isVisible,
                                               const glm::mat4 &relativeMatrix,
                                               const std::shared_ptr<Skin> &skin,
                                               const std::shared_ptr<IShader> &materialShader,
                                               const std::shared_ptr<IShader> &planarReflectionShader,
                                               const std::shared_ptr<MaterialProxy> &materialProxy)
          : SceneProxyBase(isEnabled), AProxyVisibilityController(isVisible),
            bTransformInitialized(false),
            m_relativeMatrix(relativeMatrix),
            m_skin(skin),
            m_shader(materialShader),
            m_planarReflectionShader(planarReflectionShader),
            mMaterialProxy(materialProxy)
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

      void PrimitiveSceneProxy::SetTransformationMatrix(const glm::mat4 &relativeMatrix)
      {
         bTransformInitialized = true;
         m_relativeMatrix = relativeMatrix;
      }

      std::shared_ptr<Skin> PrimitiveSceneProxy::GetSkin() const
      {
         return m_skin;
      }

      ePrimitiveProxyType PrimitiveSceneProxy::GetPrimitiveProxyType() const
      {
         return ePrimitiveProxyType::PRIMITIVE_PROXY;
      }

      bool PrimitiveSceneProxy::IsFrustumCullTestNeeded() const
      {
         return true;
      }

      bool PrimitiveSceneProxy::IsTransformIntialized() const
      {
         return bTransformInitialized;
      }

      ePrimitiveSortOrder PrimitiveSceneProxy::GetPrimitiveSortOrder() const
      {
         return ePrimitiveSortOrder::ORDER_FIRST;
      }

      void PrimitiveSceneProxy::SetDeferredShadingSceneRenderer(const std::weak_ptr<DeferredShadingSceneRenderer>& deferredShadingSceneRenderer)
      {
         mDeferredShadingSceneRenderer = deferredShadingSceneRenderer;
      }

      const std::weak_ptr<DeferredShadingSceneRenderer>& PrimitiveSceneProxy::GetDeferredShadingSceneRendererWp() const
      {
         return mDeferredShadingSceneRenderer;
      }

   }
}