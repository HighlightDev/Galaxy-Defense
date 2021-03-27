#include "SkeletalMeshSceneProxy.h"
#include "Core/GraphicsCore/Mesh/AnimatedSkin.h"
#include <TinyLogger/LogInterface.h>

using namespace Graphics::Mesh;
using namespace Game;

namespace Graphics
{
   namespace Proxy
   {

      SkeletalMeshSceneProxy::SkeletalMeshSceneProxy(const SkeletalMeshComponent* component)
         : PrimitiveSceneProxy(component->IsVisible()
            , component->GetRelativeMatrix()
            , component->GetRenderData().m_skin
            , component->GetRenderData().m_materialShader
            , component->GetRenderData().m_planarReflectionShader
            , component->GetRenderData().mMaterialProxy)
         , mAnimationPlayer(nullptr)
      {
         std::shared_ptr<AnimatedSkin> spt_AnimatedSkin = std::dynamic_pointer_cast<AnimatedSkin>(m_skin);

         assert((spt_AnimatedSkin));

         mAnimationPlayer = std::make_shared<AnimationPlayer>(spt_AnimatedSkin->GetAnimatedMeshData());
      }

      SkeletalMeshSceneProxy::~SkeletalMeshSceneProxy()
      {
      }

      std::shared_ptr<SkeletalMeshSceneProxy::ShaderType> SkeletalMeshSceneProxy::GetShader() const
      {
         return std::static_pointer_cast<SkeletalMeshSceneProxy::ShaderType>(m_shader);
      }

      std::shared_ptr<SkeletalMeshSceneProxy::PlanarReflectionShaderType> SkeletalMeshSceneProxy::GetPlanarReflectionShader() const
      {
         return std::static_pointer_cast<SkeletalMeshSceneProxy::PlanarReflectionShaderType>(m_planarReflectionShader);
      }

      void SkeletalMeshSceneProxy::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         const auto& shader = GetShader();

         shader->ExecuteShader();
         shader->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
         shader->GetVertexFactoryShader()->SetSkinningMatrices(GetSkinningMatrices());
         shader->GetMaterialShader()->LoadUniformValues(mMaterialProxy);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         shader->StopShader();
      }

      void SkeletalMeshSceneProxy::RenderPlanarReflection(const glm::vec4& plane, const glm::mat4& mirrorMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         const auto& planarReflectionShader = GetPlanarReflectionShader();

         planarReflectionShader->ExecuteShader();
         planarReflectionShader->GetShader()->SetClipPlane(plane);
         planarReflectionShader->GetVertexFactoryShader()->SetMatrices(mirrorMatrix * m_relativeMatrix, viewMatrix, projectionMatrix);
         planarReflectionShader->GetVertexFactoryShader()->SetSkinningMatrices(GetSkinningMatrices());
         planarReflectionShader->GetMaterialShader()->LoadUniformValues(mMaterialProxy);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         planarReflectionShader->StopShader();
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

      PrimitiveProxyType SkeletalMeshSceneProxy::GetPrimitiveProxyType() const
      {
         return PrimitiveProxyType::SKELETAL_MESH_PROXY;
      }

      const std::vector<glm::mat4>& SkeletalMeshSceneProxy::GetSkinningMatrices()
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
