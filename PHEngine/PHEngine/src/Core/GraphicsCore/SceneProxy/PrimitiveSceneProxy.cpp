#include "PrimitiveSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"

using namespace Graphics::Renderer;

namespace Graphics
{
   namespace Proxy
   {

      PrimitiveSceneProxy::PrimitiveSceneProxy(const ::EngineCore::PrimitiveComponent *component,
                                               const std::shared_ptr<Skin> &skin,
                                               const std::shared_ptr<IShader> &materialShader,
                                               const std::shared_ptr<IShader> &planarReflectionShader,
                                               const std::shared_ptr<MaterialProxy> &materialProxy)
          : SceneProxyBase(component->IsEnabled()),
            AProxyVisibilityController(component->IsVisible()),
            bTransformInitialized(false),
            m_relativeMatrix(component->GetRelativeMatrix()),
            m_skin(skin),
            m_shader(materialShader),
            m_planarReflectionShader(planarReflectionShader),
            mMaterialProxy(materialProxy),
            mSortOrderValue(component->GetSortOrderValue())
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

      int32_t PrimitiveSceneProxy::GetPrimitiveSortOrder() const
      {
         return mSortOrderValue;
      }

      void PrimitiveSceneProxy::SetDeferredShadingSceneRenderer(const std::weak_ptr<DeferredShadingSceneRenderer> &deferredShadingSceneRenderer)
      {
         mDeferredShadingSceneRenderer = deferredShadingSceneRenderer;
      }

      const std::weak_ptr<DeferredShadingSceneRenderer> &PrimitiveSceneProxy::GetDeferredShadingSceneRendererWp() const
      {
         return mDeferredShadingSceneRenderer;
      }

      void PrimitiveSceneProxy::SetSortOrderValue(const int32_t sortOrderValue)
      {
         mSortOrderValue = sortOrderValue;
      }

      int32_t PrimitiveSceneProxy::GetSortOrderValue() const
      {
         return mSortOrderValue;
      }

   }
}