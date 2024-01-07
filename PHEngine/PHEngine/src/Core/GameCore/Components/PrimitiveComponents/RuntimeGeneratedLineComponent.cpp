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
   RuntimeGeneratedLineComponent::RuntimeGeneratedLineComponent(const std::shared_ptr<MeshComponentData> &meshComponentData,
                                                                const StaticMeshRenderData &renderData,
                                                                const RuntimeGeneratedMeshPoolParameters &rtMeshParams)
       : StaticMeshComponent(meshComponentData, renderData),
         mRtMeshParams(rtMeshParams),
         mLineBeginWorldSpacePosition(),
         mLineEndWorldSpacePosition(),
         mLineWidth(1.0f)
   {
   }

   RuntimeGeneratedLineComponent::~RuntimeGeneratedLineComponent()
   {
   }

   std::shared_ptr<PrimitiveSceneProxy> RuntimeGeneratedLineComponent::CreateSceneProxy() const
   {
      return std::make_shared<RuntimeGeneratedLineSceneProxy>(this);
   }

   void RuntimeGeneratedLineComponent::UnpausableTick(const float deltaTime)
   {
      StaticMeshComponent::UnpausableTick(deltaTime);

      if (mIsRenderDataDirty && bIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
      {
         SyncRenderData();
         mIsRenderDataDirty = false;
      }
   }

   const RuntimeGeneratedMeshPoolParameters &RuntimeGeneratedLineComponent::GetRuntimeMeshPoolParameters() const
   {
      return mRtMeshParams;
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
         mIsRenderDataDirty = true;
      }
   }

   void RuntimeGeneratedLineComponent::SetLineEndWorldSpacePosition(const glm::vec3 &position)
   {
      if (!EngineMath::CheckSimilarityVec3(position, mLineEndWorldSpacePosition))
      {
         mLineEndWorldSpacePosition = position;
         mIsRenderDataDirty = true;
      }
   }

   void RuntimeGeneratedLineComponent::SetLineWidth(const float lineWidth)
   {
      if (!EngineMath::FloatsNearEqual(lineWidth, mLineWidth))
      {
         mLineWidth = lineWidth;
         mIsRenderDataDirty = true;
      }
   }

   float RuntimeGeneratedLineComponent::GetLineWidth() const
   {
      return mLineWidth;
   }

   void RuntimeGeneratedLineComponent::SyncRenderData()
   {
      static constexpr uint64_t functionId = Hash64_CT("RuntimeGeneratedLineComponent::SyncRenderData");
      if (const auto &sceneSp = m_sceneWP.lock())
      {
         if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
         {
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetObjectId(), functionId, [sceneRenderer, sceneProxyId = mSceneProxyId, lineBeginWorldSpacePosition = mLineBeginWorldSpacePosition, lineEndWorldSpacePosition = mLineEndWorldSpacePosition, lineWidth = mLineWidth]()
                                                                                {
            if (const auto &lineProxySp = std::static_pointer_cast<RuntimeGeneratedLineSceneProxy>(sceneRenderer->GetPrimitiveProxyByProxyId(sceneProxyId)))
            {
               lineProxySp->SetLineBeginWorldSpacePosition(lineBeginWorldSpacePosition);
               lineProxySp->SetLineEndWorldSpacePosition(lineEndWorldSpacePosition);
               lineProxySp->SetLineWidth(lineWidth);
            } });
         }
      }
   }
}
