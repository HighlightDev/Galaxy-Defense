#include "CubemapSceneProxy.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlasFactory.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"

using namespace Graphics::Renderer;
using namespace EngineCore;
using namespace Resources;

namespace Graphics
{
   namespace Proxy
   {
      CubemapSceneProxy::CubemapSceneProxy(const CubemapComponent *component)
          : PrimitiveSceneProxy(component,
                                nullptr,
                                nullptr,
                                nullptr,
                                nullptr),
            m_shaderCubemap(std::static_pointer_cast<CubemapShader>(component->GetRenderData().m_shader)),
            m_textureObtainer(component->GetRenderData().m_texture)
      {
      }

      CubemapSceneProxy::~CubemapSceneProxy()
      {
      }

      void CubemapSceneProxy::PostConstructorInitialize()
      {
         static constexpr uint64_t functionId = Hash64_CT("CubemapSceneProxy::PostConstructorInitialize");
         m_skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource((int32_t)SimplePrimitiveType::CUBE);

         if (const auto &deferredShadingSceneRendererSp = GetDeferredShadingSceneRendererWp().lock())
         {
            if (const auto &sceneSp = deferredShadingSceneRendererSp->GetInterThreadCommunicationManager().GetSceneWP().lock())
            {
               const auto boundingBox = m_skin->GetBoundingBox();
               sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mSceneProxyId, functionId,
                                            [this, sceneSp, boundingBox]()
                                            {
                                               const auto &engineObject = sceneSp->GetEngineObjectById(GetGameObjectId());
                                               assert(engineObject);
                                               const auto &primitiveComponent = static_cast<PrimitiveComponent *>(engineObject);
                                               assert(primitiveComponent);
                                               primitiveComponent->SetBoundingBox(boundingBox);
                                            });
            }
         }
      }

      std::shared_ptr<IShader> CubemapSceneProxy::GetShader() const
      {
         return m_shaderCubemap;
      }

      void CubemapSceneProxy::Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         std::shared_ptr<TextureAtlasHandler> texHandler = TextureAtlasFactory::GetInstance()->GetTextureAtlasCellByRequestId(m_textureObtainer.MyRequestId);
         if (texHandler && texHandler->GetTextureType() == eTextureType::TEXTURE_CUBE)
         {
            std::shared_ptr<ITexture> texture = texHandler->GetAtlasResource();

            auto cubemapShader = std::static_pointer_cast<CubemapShader>(m_shaderCubemap);

            cubemapShader->ExecuteShader();
            texture->BindTexture(0);
            cubemapShader->SetTexture(0);
            cubemapShader->SetTransformMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
            m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
            cubemapShader->StopShader();
         }
      }

      bool CubemapSceneProxy::IsDeferred() const
      {
         return false;
      }

      eMeshFacing CubemapSceneProxy::GetMeshFrontFace() const
      {
         return eMeshFacing::COUNTER_CLOCK_WISE;
      }

   }
}
