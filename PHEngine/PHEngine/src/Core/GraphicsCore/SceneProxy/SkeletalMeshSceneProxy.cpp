#include "SkeletalMeshSceneProxy.h"
#include "Core/GraphicsCore/Mesh/AnimatedSkin.h"

using namespace Graphics::Mesh;
using namespace Game;

namespace Graphics
{
   namespace Proxy
   {

      SkeletalMeshSceneProxy::SkeletalMeshSceneProxy(const SkeletalMeshComponent* component)
         : PrimitiveSceneProxy(component->GetRelativeMatrix()
            , component->GetRenderData().m_skin
            , component->GetRenderData().m_materialShader
            , component->GetRenderData().mMaterialInstance)
         , mAnimationPlayer(nullptr)
      {
         std::shared_ptr<AnimatedSkin> spt_AnimatedSkin = std::dynamic_pointer_cast<AnimatedSkin>(m_skin);

         assert((spt_AnimatedSkin));

         mAnimationPlayer = std::make_shared<AnimationPlayer>(spt_AnimatedSkin->GetAnimatedMeshData());
      }

      SkeletalMeshSceneProxy::~SkeletalMeshSceneProxy()
      {
      }

      void SkeletalMeshSceneProxy::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         GetShader()->ExecuteShader();
         GetShader()->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
         GetShader()->GetVertexFactoryShader()->SetSkinningMatrices(GetSkinningMatrices());
         GetShader()->GetMaterialShader()->SetUniformValues(mMaterialInstance);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         GetShader()->StopShader();
      }

      std::shared_ptr<SkeletalMeshSceneProxy::ShaderType> SkeletalMeshSceneProxy::GetShader() const
      {
         return std::static_pointer_cast<SkeletalMeshSceneProxy::ShaderType>(m_shader);
      }

      void SkeletalMeshSceneProxy::UpdateAnimationData(bool transtionEnabled, const float transitionValue,
         const float srcAnimationTime, const float dstAnimationTime, const std::string& srcAnimationName, const std::string& dstAnimationName)
      {
         mAnimationPlayer->SetSrcAnimationName(srcAnimationName);
         mAnimationPlayer->SetDstAnimationName(dstAnimationName);
         mAnimationPlayer->SetSrcAnimationTime(srcAnimationTime);
         mAnimationPlayer->SetDstAnimationTime(dstAnimationTime);
         mAnimationPlayer->SetTransitionParameter(transtionEnabled, transitionValue);
      
         bIsDirty = true;
      }

      void SkeletalMeshSceneProxy::UpdateAnimationData(bool transtionEnabled, const float transitionValue, const float srcAnimationTime,
         const float dstAnimationTime, const size_t srcAnimationIndex, const size_t dstAnimationIndex)
      {
         mAnimationPlayer->SetSrcAnimationTime(srcAnimationTime);
         mAnimationPlayer->SetDstAnimationTime(dstAnimationTime);
         mAnimationPlayer->SetSrcAnimationByIndex(srcAnimationIndex);
         mAnimationPlayer->SetDstAnimationByIndex(dstAnimationIndex);
         mAnimationPlayer->SetTransitionParameter(transtionEnabled, transitionValue);

         bIsDirty = true;
      }

      uint64_t SkeletalMeshSceneProxy::GetComponentType() const
      {
         return SKELETAL_MESH_COMPONENT;
      }

      std::vector<glm::mat4> SkeletalMeshSceneProxy::GetSkinningMatrices()
      {
         if (bIsDirty)
         {
            mAnimationPlayer->UpdateAnimationMatrices();
            bIsDirty = false;
         }

         return mAnimationPlayer->GetAnimatedMatrices();
      }

      bool SkeletalMeshSceneProxy::IsDeferred() const
      {
         return true;
      }
   }
}
