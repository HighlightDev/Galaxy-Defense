#include "PhysicsShapeDebugRenderSceneProxy.h"

namespace Graphics
{
   namespace Proxy
   {
      PhysicsShapeDebugRenderSceneProxy::PhysicsShapeDebugRenderSceneProxy(const PhysicsShapeDebugRenderComponent* component)
         : PrimitiveSceneProxy(component->GetRelativeMatrix(), component->GetRenderData().m_skin, nullptr, nullptr)
         , mShader(std::static_pointer_cast<DefaultRenderShader>(component->GetRenderData().m_shader))
      {
      }

      PhysicsShapeDebugRenderSceneProxy::~PhysicsShapeDebugRenderSceneProxy()
      {
      }

      void PhysicsShapeDebugRenderSceneProxy::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         glm::mat4 modelViewProjectionMatrix(1);

         modelViewProjectionMatrix = projectionMatrix * viewMatrix *  m_relativeMatrix;

         mShader->ExecuteShader();
         mShader->SetTransformMatrix(modelViewProjectionMatrix);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         mShader->StopShader();
      }

      bool PhysicsShapeDebugRenderSceneProxy::IsDeferred() const
      {
         return false;
      }
   }
}