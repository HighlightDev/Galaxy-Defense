#include "SkeletalMeshSceneProxy.h"
#include "Core/GraphicsCore/Mesh/AnimatedSkin.h"

using namespace Graphics::Mesh;

namespace Graphics
{
   namespace Proxy
   {

      SkeletalMeshSceneProxy::SkeletalMeshSceneProxy(const SkeletalMeshComponent* component)
         : PrimitiveSceneProxy(component->GetRelativeMatrix()
            , component->GetRenderData().m_skin
            , component->GetRenderData().m_materialShader
            , component->GetRenderData().mMaterialInstance)
         , mTimeTick(0.0f)
      {
         std::shared_ptr<AnimatedSkin> spt_AnimatedSkin = std::dynamic_pointer_cast<AnimatedSkin>(m_skin);

         assert((spt_AnimatedSkin));

         m_animatedMeshData = spt_AnimatedSkin->GetAnimatedMeshData();
      }

      SkeletalMeshSceneProxy::~SkeletalMeshSceneProxy()
      {
      }

      void SkeletalMeshSceneProxy::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         std::vector<glm::mat4> skinningMatrices = GetSkinningMatrices();

         GetShader()->ExecuteShader();
         GetShader()->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
         GetShader()->GetVertexFactoryShader()->SetSkinningMatrices(skinningMatrices);
         GetShader()->GetMaterialShader()->SetUniformValues(mMaterialInstance);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         GetShader()->StopShader();
      }

      std::shared_ptr<SkeletalMeshSceneProxy::ShaderType> SkeletalMeshSceneProxy::GetShader() const
      {
         return std::static_pointer_cast<SkeletalMeshSceneProxy::ShaderType>(m_shader);
      }

      void SkeletalMeshSceneProxy::SetAnimationDeltaTime(float animationDeltaTime)
      {
         mTimeTick += animationDeltaTime;
         bAnimationTransformationDirty = true;
      }

      uint64_t SkeletalMeshSceneProxy::GetComponentType() const
      {
         return SKELETAL_MESH_COMPONENT;
      }

      std::vector<glm::mat4> SkeletalMeshSceneProxy::GetSkinningMatrices()
      {
         if (bAnimationTransformationDirty)
         {
            // todo: update cached skinning matrices
            bAnimationTransformationDirty = false;
         }

         if (m_animatedMeshData->AnimationIndices.size() > 5)
         {
            return m_animatedMeshData->GetAnimatedMatricesByIndex(8, mTimeTick);
         }

         return m_animatedMeshData->GetAnimatedMatricesByIndex(0, mTimeTick);
      }

      bool SkeletalMeshSceneProxy::IsDeferred() const
      {
         return true;
      }
   }
}
