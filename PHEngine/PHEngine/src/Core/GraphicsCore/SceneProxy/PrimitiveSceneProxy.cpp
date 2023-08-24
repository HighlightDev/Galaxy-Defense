#include "PrimitiveSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"

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

      void PrimitiveSceneProxy::CleanUp()
      {
         LogInfo("PrimitiveSceneProxy::CleanUp => proxyId: ", mSceneProxyId, ", gameObjectId: ", mGameObjectId);

         if (m_skin)
         {
            MeshPool::GetInstance()->TryToFreeMemory(m_skin);
            m_skin = nullptr;
         }

         if (m_shader)
         {
            CompositeShaderPool::GetInstance()->TryToFreeMemory(m_shader);
            m_shader = nullptr;
         }

         if (m_planarReflectionShader)
         {
            CompositeShaderPool::GetInstance()->TryToFreeMemory(m_planarReflectionShader);
            m_planarReflectionShader = nullptr;
         }
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

      bool PrimitiveSceneProxy::CanBloomBeApplied() const
      {
         return true;
      }

   }
}