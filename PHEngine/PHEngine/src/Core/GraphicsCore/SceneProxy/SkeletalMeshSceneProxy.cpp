#include "SkeletalMeshSceneProxy.h"
#include "Core/GraphicsCore/Mesh/AnimatedSkin.h"

using namespace Graphics::Mesh;

namespace Graphics
{
   namespace Proxy
   {

      SkeletalMeshSceneProxy::SkeletalMeshSceneProxy(const SkeletalMeshComponent* component)
         : PrimitiveSceneProxy(component->GetRelativeMatrix(), component->GetRenderData().m_skin, component->GetRenderData().m_materialShader, component->GetRenderData().mMaterialInstance)
         , m_animations(component->GetRenderData().m_animations)
         , m_animationHolder(m_animations)
         , m_animationDeltaTime(component->GetAnimationDeltaTime())
      {
         m_animationHolder.SetAnimationByNameNoBlend(m_animations->at(0).GetName());

         m_animatedMeshData = std::static_pointer_cast<AnimatedSkin>(m_skin)->GetAnimatedMeshData();
      }

      SkeletalMeshSceneProxy::~SkeletalMeshSceneProxy()
      {
      }


      float timeFlow = 0.0f;


      void SkeletalMeshSceneProxy::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         std::shared_ptr<AnimatedSkin> animatedSkin = std::static_pointer_cast<AnimatedSkin>(m_skin);

         if (bAnimationTransformationDirty)
         {
            m_animationHolder.UpdateAnimationLoopTime(m_animationDeltaTime);
            bAnimationTransformationDirty = false;
         }

         std::vector<glm::mat4> skinningMatrices = m_animatedMeshData->GetAnimatedMatrices(m_animations->at(0).GetName(), timeFlow);
            //m_animationHolder.GetAnimatedOffsetedMatrices(animatedSkin->GetRootBone().get());

         GetShader()->ExecuteShader();
         GetShader()->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
         GetShader()->GetVertexFactoryShader()->SetSkinningMatrices(skinningMatrices);
         GetShader()->GetMaterialShader()->SetUniformValues(mMaterialInstance);
         animatedSkin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         GetShader()->StopShader();
      }

      std::shared_ptr<SkeletalMeshSceneProxy::ShaderType> SkeletalMeshSceneProxy::GetShader() const
      {
         return std::static_pointer_cast<SkeletalMeshSceneProxy::ShaderType>(m_shader);
      }

      void SkeletalMeshSceneProxy::SetAnimationDeltaTime(float animationDeltaTime)
      {
         m_animationDeltaTime = animationDeltaTime;
         bAnimationTransformationDirty = true;
      }

      uint64_t SkeletalMeshSceneProxy::GetComponentType() const
      {
         return SKELETAL_MESH_COMPONENT;
      }

      std::vector<glm::mat4> SkeletalMeshSceneProxy::GetSkinningMatrices()
      {
         AnimatedSkin* animatedSkin = static_cast<AnimatedSkin*>(m_skin.get());

         if (bAnimationTransformationDirty)
         {
            m_animationHolder.UpdateAnimationLoopTime(m_animationDeltaTime);
            bAnimationTransformationDirty = false;
         }

         timeFlow += m_animationDeltaTime * 1000.0f;

        return m_animatedMeshData->GetAnimatedMatrices(m_animations->at(0).GetName(), timeFlow);


         //return m_animationHolder.GetAnimatedOffsetedMatrices(animatedSkin->GetRootBone().get());
      }

      bool SkeletalMeshSceneProxy::IsDeferred() const
      {
         return true;
      }
   }
}
