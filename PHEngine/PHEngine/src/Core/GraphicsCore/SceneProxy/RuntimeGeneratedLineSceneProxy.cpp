#include "RuntimeGeneratedLineSceneProxy.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/ResourceManagerCore/Pool/RuntimeGeneratedMeshPool.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GraphicsCore/SceneViewInfo/SceneView.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace Resources;
using namespace EngineCore;
using namespace Graphics::Renderer;
using namespace Graphics;

namespace Graphics
{
   namespace Proxy
   {
      RuntimeGeneratedLineSceneProxy::RuntimeGeneratedLineSceneProxy(const RuntimeGeneratedLineComponent *component)
          : StaticMeshSceneProxy(component),
            mLineBeginWorldSpacePosition(component->GetLineBeginWorldSpacePosition()),
            mLineEndWorldSpacePosition(component->GetLineEndWorldSpacePosition()),
            mLineWidth(component->GetLineWidth()),
            mRtMeshPoolParams(component->GetRuntimeMeshPoolParameters())
      {
      }

      void RuntimeGeneratedLineSceneProxy::PostConstructorInitialize()
      {
         static constexpr uint64_t functionId = Hash64_CT("RuntimeGeneratedLineSceneProxy::PostConstructorInitialize");
         m_skin = RuntimeGeneratedMeshPool::GetInstance()->GetOrAllocateResource(mRtMeshPoolParams);

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

      void RuntimeGeneratedLineSceneProxy::Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         UpdateGeometry(cameraSceneProxy, viewMatrix);

         const auto &shader = GetShader();

         GLboolean isCullFaceEnabled;
         glGetBooleanv(GL_CULL_FACE, &isCullFaceEnabled);

         glDisable(GL_CULL_FACE);
         shader->ExecuteShader();
         shader->GetMaterialShader()->LoadUniformValues(mMaterialProxy);
         shader->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
         m_skin->GetBuffer()->RenderVAO(0, 4, GL_TRIANGLE_STRIP);
         shader->StopShader();

         if (isCullFaceEnabled)
         {
            glEnable(GL_CULL_FACE);
         }
      }

      void RuntimeGeneratedLineSceneProxy::SetLineBeginWorldSpacePosition(const glm::vec3 &position)
      {
         mLineBeginWorldSpacePosition = position;
         bUpdateLineGeometry = true;
      }

      void RuntimeGeneratedLineSceneProxy::SetLineEndWorldSpacePosition(const glm::vec3 &position)
      {
         mLineEndWorldSpacePosition = position;
         bUpdateLineGeometry = true;
      }

      void RuntimeGeneratedLineSceneProxy::SetLineWidth(const float lineWidth)
      {
         mLineWidth = lineWidth;
         bUpdateLineGeometry = true;
      }

      bool RuntimeGeneratedLineSceneProxy::CanBloomBeApplied() const
      {
         return false;
      }

      void RuntimeGeneratedLineSceneProxy::UpdateGeometry(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix)
      {
         if (bUpdateLineGeometry)
         {
            auto *const verticesVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::POSITION);
            auto *const textureCoordinatesVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::TEXTURE_COORDINATES);

            assert(verticesVBO && textureCoordinatesVBO);
            auto lineBeginViewSpacePosition = glm::vec3(viewMatrix * glm::vec4(mLineBeginWorldSpacePosition.x, mLineBeginWorldSpacePosition.y, mLineBeginWorldSpacePosition.z, 1.0f));
            auto lineEndViewSpacePosition = glm::vec3(viewMatrix * glm::vec4(mLineEndWorldSpacePosition.x, mLineEndWorldSpacePosition.y, mLineEndWorldSpacePosition.z, 1.0f));

            const float halfWidth = mLineWidth * 0.5f;
            const auto forwardVec = glm::normalize(mLineEndWorldSpacePosition - mLineBeginWorldSpacePosition);
            const auto &cameraForwardVec = cameraSceneProxy->GetForwardVector();
            const auto &lineBasisVector = glm::normalize(glm::cross(glm::normalize(cameraForwardVec), forwardVec));

            const auto viewP1 = lineBeginViewSpacePosition - (lineBasisVector * halfWidth);
            const auto viewP2 = lineBeginViewSpacePosition + (lineBasisVector * halfWidth);
            const auto viewP3 = lineEndViewSpacePosition - (lineBasisVector * halfWidth);
            const auto viewP4 = lineEndViewSpacePosition + (lineBasisVector * halfWidth);

            const auto texP1 = glm::vec2(0, 1);
            const auto texP2 = glm::vec2(0, 0);
            const auto texP3 = glm::vec2(1, 1);
            const auto texP4 = glm::vec2(1, 0);

            std::vector<float> vertices = std::vector<float>({viewP1.x, viewP1.y, viewP1.z,
                                                              viewP2.x, viewP2.y, viewP2.z,
                                                              viewP3.x, viewP3.y, viewP3.z,
                                                              viewP4.x, viewP4.y, viewP4.z});

            std::vector<float> texCoords = {texP1.x, texP1.y,
                                            texP2.x, texP2.y,
                                            texP3.x, texP3.y,
                                            texP4.x, texP4.y};

            const size_t verticesBufferSize = sizeof(float) * vertices.size();
            const size_t texCoordsBufferSize = sizeof(float) * texCoords.size();

            verticesVBO->BindVBO();
            verticesVBO->BufferSubData(0, verticesBufferSize, (void *)vertices.data());

            textureCoordinatesVBO->BindVBO();
            textureCoordinatesVBO->BufferSubData(0, texCoordsBufferSize, (void *)texCoords.data());
            textureCoordinatesVBO->UnbindVBO();

            bUpdateLineGeometry = false;
         }
      }
   }
}
