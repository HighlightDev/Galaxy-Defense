#include "RuntimeGeneratedLineComponent.h"
#include "Core/GraphicsCore/SceneProxy/RuntimeGeneratedLineSceneProxy.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/UtilityCore/EngineMath.h"

#include <memory>
#include <algorithm>

using namespace Graphics;
using namespace Graphics::Renderer;

namespace EngineCore
{

   RuntimeGeneratedLineComponent::RuntimeGeneratedLineComponent(const MeshComponentData &meshComponentData, const StaticMeshRenderData &renderData)
       : StaticMeshComponent(meshComponentData, renderData),
         mLineBeginWorldSpacePosition(),
         mLineEndWorldSpacePosition()
   {
   }

   std::shared_ptr<PrimitiveSceneProxy> RuntimeGeneratedLineComponent::CreateSceneProxy() const
   {
      return std::make_shared<RuntimeGeneratedLineSceneProxy>(this);
   }

   void RuntimeGeneratedLineComponent::Tick(const float deltaTime)
   {
      StaticMeshComponent::Tick(deltaTime);

      if (const auto &sceneSp = m_sceneWP.lock())
      {
         const auto &a_player = sceneSp->GetActorByName("SpaceshipActor");
         const auto &a_enemy = sceneSp->GetActorByName("a_enemyShip_0");
         if (a_player && a_enemy)
         {
            const auto &beginLinePoint = a_player->GetRootComponent()->GetTranslation();
            const auto &endLinePoint = a_enemy->GetRootComponent()->GetTranslation();
            SetLineBeginWorldSpacePosition(beginLinePoint);
            SetLineEndWorldSpacePosition(endLinePoint);
         }
      }
   }

   glm::vec3 RuntimeGeneratedLineComponent::GetLineBeginWorldSpacePosition() const
   {
      return mLineEndWorldSpacePosition;
   }

   glm::vec3 RuntimeGeneratedLineComponent::GetLineEndWorldSpacePosition() const
   {
      return mLineBeginWorldSpacePosition;
   }

   void RuntimeGeneratedLineComponent::SetLineBeginWorldSpacePosition(const glm::vec3 &position)
   {
      if (!EngineMath::CheckSimilarityVec3(position, mLineBeginWorldSpacePosition))
      {
         mLineBeginWorldSpacePosition = position;

         static constexpr uint64_t functionId = Hash64_CT("RuntimeGeneratedLineComponent:SetLineBeginWorldSpacePosition");
         if (const auto &sceneSp = m_sceneWP.lock())
         {
            if (const auto &sceneRenderer = sceneSp->GetThreadManager().TryGetSceneRendererWP().lock())
            {
               sceneSp->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetObjectId(), functionId, [=]()
                                              {
                  const auto& primitiveProxySp = sceneRenderer->GetPrimitiveProxyByProxyId(SceneProxyId);
                  if (const auto& lineProxySp = std::static_pointer_cast<RuntimeGeneratedLineSceneProxy>(primitiveProxySp))
                  {
                     lineProxySp->SetLineBeginWorldSpacePosition(mLineBeginWorldSpacePosition);
                  } });
            }
         }
      }
   }

   void RuntimeGeneratedLineComponent::SetLineEndWorldSpacePosition(const glm::vec3 &position)
   {
      if (!EngineMath::CheckSimilarityVec3(position, mLineEndWorldSpacePosition))
      {
         mLineEndWorldSpacePosition = position;

         static constexpr uint64_t functionId = Hash64_CT("RuntimeGeneratedLineComponent:SetLineEndWorldSpacePosition");
         if (const auto &sceneSp = m_sceneWP.lock())
         {
            if (const auto &sceneRenderer = sceneSp->GetThreadManager().TryGetSceneRendererWP().lock())
            {
               sceneSp->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetObjectId(), functionId, [=]()
                                              {
                  const auto& primitiveProxySp = sceneRenderer->GetPrimitiveProxyByProxyId(SceneProxyId);
                  if (const auto& lineProxySp = std::static_pointer_cast<RuntimeGeneratedLineSceneProxy>(primitiveProxySp))
                  {
                     lineProxySp->SetLineEndWorldSpacePosition(mLineEndWorldSpacePosition);
                  } });
            }
         }
      }
   }
}
