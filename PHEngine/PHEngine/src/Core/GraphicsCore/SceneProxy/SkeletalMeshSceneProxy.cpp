#include "SkeletalMeshSceneProxy.h"
#include "Core/GraphicsCore/Mesh/AnimatedSkin.h"

#include "Core/GameCore/StateMachine/StateMachine.h"
#include "Core/GameCore/StateMachine/AnimationStateMachineController.h"

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
      {
         std::shared_ptr<AnimatedSkin> spt_AnimatedSkin = std::dynamic_pointer_cast<AnimatedSkin>(m_skin);

         assert((spt_AnimatedSkin));

         mAnimationPlayer = std::make_shared<AnimationPlayer>(spt_AnimatedSkin->GetAnimatedMeshData());
      }

      void SkeletalMeshSceneProxy::InitStateMachine()
      {
         State* stateIdle = new State("State Idle");
         State* stateWalking = new State("State Walking");

         StateProperty<StatePropertyType::Animation>* animationProp = new StateProperty<StatePropertyType::Animation>("Idle");

         StateTransition transitionFromIdleToWalking(stateIdle, stateWalking, 1.0f);
         stateIdle->AddStateTransition(transitionFromIdleToWalking);

         StateTransition transitionFromWalkingToIdle(stateWalking, stateIdle, 1.0f);
         stateWalking->AddStateTransition(transitionFromWalkingToIdle);

         mStateMachine = new StateMachine(stateIdle);
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

      void SkeletalMeshSceneProxy::SetAnimationDeltaTime(float animationDeltaTime)
      {
         mAnimationPlayer->UpdateAnimationTime(animationDeltaTime);
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
            mAnimationPlayer->UpdateAnimationMatrices();
            bAnimationTransformationDirty = false;
         }

         return mAnimationPlayer->GetAnimatedMatrices();
      }

      bool SkeletalMeshSceneProxy::IsDeferred() const
      {
         return true;
      }
   }
}
