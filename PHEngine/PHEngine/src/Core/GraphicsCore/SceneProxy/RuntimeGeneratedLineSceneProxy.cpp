#include "RuntimeGeneratedLineSceneProxy.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Graphics
{
   namespace Proxy
   {
      RuntimeGeneratedLineSceneProxy::RuntimeGeneratedLineSceneProxy(const RuntimeGeneratedLineComponent *component)
          : StaticMeshSceneProxy(component),
            mLineBeginWorldSpacePosition(component->GetLineBeginWorldSpacePosition()),
            mLineEndWorldSpacePosition(component->GetLineEndWorldSpacePosition())
      {
      }

      void RuntimeGeneratedLineSceneProxy::Render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         UpdateGeometry(viewMatrix);

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

      void RuntimeGeneratedLineSceneProxy::UpdateGeometry(const glm::mat4 &viewMatrix)
      {
         if (bUpdateLineGeometry)
         {
            auto *const verticesVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::POSITION);
            auto *const textureCoordinatesVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::TEXTURE_COORDINATES);

            assert(verticesVBO && textureCoordinatesVBO);
            auto lineBeginViewSpacePosition = glm::vec3(viewMatrix * glm::vec4(mLineBeginWorldSpacePosition.x, mLineBeginWorldSpacePosition.y, mLineBeginWorldSpacePosition.z, 1.0f));
            auto lineEndViewSpacePosition = glm::vec3(viewMatrix * glm::vec4(mLineEndWorldSpacePosition.x, mLineEndWorldSpacePosition.y, mLineEndWorldSpacePosition.z, 1.0f));

            const float halfWidth = 5.5f;

            const auto forwardVec = glm::normalize(lineEndViewSpacePosition - lineBeginViewSpacePosition);
            const auto rightVec = glm::normalize(glm::cross(forwardVec, EngineMath::AXIS_UP));

            const auto viewP1 = lineBeginViewSpacePosition - (rightVec * halfWidth);
            const auto viewP2 = lineBeginViewSpacePosition + (rightVec * halfWidth);
            const auto viewP3 = lineEndViewSpacePosition - (rightVec * halfWidth);
            const auto viewP4 = lineEndViewSpacePosition + (rightVec * halfWidth);

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
