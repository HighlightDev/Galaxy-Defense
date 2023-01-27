#include "SkeletalMeshSceneProxy.h"
#include "Core/GraphicsCore/Mesh/AnimatedSkin.h"
#include "Core/GameCore/Scene.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"

using namespace Graphics::Renderer;
using namespace Graphics::Mesh;
using namespace EngineCore;
using namespace Resources;

namespace Graphics
{
   namespace Proxy
   {
      SkeletalMeshSceneProxy::SkeletalMeshSceneProxy(const SkeletalMeshComponent *component)
          : PrimitiveSceneProxy(component,
                                nullptr,
                                component->GetRenderData().m_materialShader,
                                component->GetRenderData().m_planarReflectionShader,
                                component->GetRenderData().mMaterialProxy),
            mRenderData(component->GetRenderData()),
            mAnimationPlayer(nullptr)
      {
      }

      SkeletalMeshSceneProxy::~SkeletalMeshSceneProxy()
      {
      }

      void SkeletalMeshSceneProxy::PostConstructorInitialize()
      {
         static constexpr uint64_t functionId = Hash64_CT("SkeletalMeshSceneProxy::PostConstructorInitialize");
         m_skin = MeshPool::GetInstance()->GetOrAllocateResource(mRenderData.mModelName);
         std::shared_ptr<AnimatedSkin> animatedSkinSp = std::dynamic_pointer_cast<AnimatedSkin>(m_skin);
         assert((animatedSkinSp));
         mAnimationPlayer = std::make_shared<AnimationPlayer>(animatedSkinSp);

         if (const auto &deferredShadingSceneRendererSp = GetDeferredShadingSceneRendererWp().lock())
         {
            if (const auto &sceneSp = deferredShadingSceneRendererSp->GetThreadManager().GetSceneWP().lock())
            {
               const auto boundingBox = m_skin->GetBoundingBox();
               sceneSp->ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mSceneProxyId, functionId,
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

      std::shared_ptr<SkeletalMeshSceneProxy::ShaderType> SkeletalMeshSceneProxy::GetShader() const
      {
         return std::static_pointer_cast<SkeletalMeshSceneProxy::ShaderType>(m_shader);
      }

      std::shared_ptr<SkeletalMeshSceneProxy::PlanarReflectionShaderType> SkeletalMeshSceneProxy::GetPlanarReflectionShader() const
      {
         return std::static_pointer_cast<SkeletalMeshSceneProxy::PlanarReflectionShaderType>(m_planarReflectionShader);
      }

      void SkeletalMeshSceneProxy::Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         const auto &shader = GetShader();

         shader->ExecuteShader();
         shader->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
         shader->GetVertexFactoryShader()->SetSkinningMatrices(GetSkinningMatrices());
         shader->GetMaterialShader()->LoadUniformValues(mMaterialProxy);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         shader->StopShader();
      }

      void SkeletalMeshSceneProxy::RenderPlanarReflection(const glm::vec4 &plane, const glm::mat4 &mirrorMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         const auto &planarReflectionShader = GetPlanarReflectionShader();

         planarReflectionShader->ExecuteShader();
         planarReflectionShader->GetShader()->SetClipPlane(plane);
         planarReflectionShader->GetVertexFactoryShader()->SetMatrices(mirrorMatrix * m_relativeMatrix, viewMatrix, projectionMatrix);
         planarReflectionShader->GetVertexFactoryShader()->SetSkinningMatrices(GetSkinningMatrices());
         planarReflectionShader->GetMaterialShader()->LoadUniformValues(mMaterialProxy);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         planarReflectionShader->StopShader();
      }

      void SkeletalMeshSceneProxy::UpdateAnimationData(bool transtionEnabled, const float transitionValue,
                                                       const float srcAnimationTime, const float dstAnimationTime, const std::string &srcAnimationName, const std::string &dstAnimationName)
      {
         mAnimationPlayer->SetSrcAnimationName(srcAnimationName);
         mAnimationPlayer->SetDstAnimationName(dstAnimationName);
         mAnimationPlayer->SetSrcAnimationTime(srcAnimationTime);
         mAnimationPlayer->SetDstAnimationTime(dstAnimationTime);
         mAnimationPlayer->SetTransitionParameter(transtionEnabled, transitionValue);

         bAnimationDataIsDirty = true;
      }

      void SkeletalMeshSceneProxy::UpdateAnimationData(bool transtionEnabled, const float transitionValue, const float srcAnimationTime,
                                                       const float dstAnimationTime, const size_t srcAnimationIndex, const size_t dstAnimationIndex)
      {
         mAnimationPlayer->SetSrcAnimationTime(srcAnimationTime);
         mAnimationPlayer->SetDstAnimationTime(dstAnimationTime);
         mAnimationPlayer->SetSrcAnimationByIndex(srcAnimationIndex);
         mAnimationPlayer->SetDstAnimationByIndex(dstAnimationIndex);
         mAnimationPlayer->SetTransitionParameter(transtionEnabled, transitionValue);

         bAnimationDataIsDirty = true;
      }

      ePrimitiveProxyType SkeletalMeshSceneProxy::GetPrimitiveProxyType() const
      {
         return ePrimitiveProxyType::SKELETAL_MESH_PROXY;
      }

      const std::vector<glm::mat4> &SkeletalMeshSceneProxy::GetSkinningMatrices() const
      {
         if (bAnimationDataIsDirty)
         {
            mAnimationPlayer->UpdateAnimationMatrices();
            bAnimationDataIsDirty = false;
         }

         return mAnimationPlayer->GetAnimatedMatrices();
      }

      bool SkeletalMeshSceneProxy::IsDeferred() const
      {
         return true;
      }

      eMeshFacing SkeletalMeshSceneProxy::GetMeshFrontFace() const
      {
         return eMeshFacing::COUNTER_CLOCK_WISE;
      }
   }
}
