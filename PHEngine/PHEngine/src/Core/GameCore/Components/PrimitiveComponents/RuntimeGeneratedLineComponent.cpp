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
   RuntimeGeneratedLineComponent::RuntimeGeneratedLineComponent(const MeshComponentData &meshComponentData,
                                                                const StaticMeshRenderData &renderData,
                                                                const RuntimeGeneratedMeshPoolParameters &rtMeshParams)
       : StaticMeshComponent(meshComponentData, renderData),
         mRtMeshParams(rtMeshParams),
         mLineBeginWorldSpacePosition(),
         mLineEndWorldSpacePosition(),
         mLineWidth(1.0f)
   {
      Event::CameraTransformChangedEvent::GetInstance()->AddListener(this);
   }

   RuntimeGeneratedLineComponent::~RuntimeGeneratedLineComponent()
   {
      Event::CameraTransformChangedEvent::GetInstance()->RemoveListener(this);
   }

   std::shared_ptr<PrimitiveSceneProxy> RuntimeGeneratedLineComponent::CreateSceneProxy() const
   {
      return std::make_shared<RuntimeGeneratedLineSceneProxy>(this);
   }

   void RuntimeGeneratedLineComponent::ProcessEvent(const typename Event::CameraTransformChangedEvent::EventData_t &data)
   {
      if (const auto &sceneSp = m_sceneWP.lock())
      {
         if (std::get<0>(data)->GetCameraName() == sceneSp->GetMainCamera()->GetCameraName())
         {
            mIsRenderDataDirty = true;
         }
      }
   }

   void RuntimeGeneratedLineComponent::UnpausableTick(const float deltaTime)
   {
       StaticMeshComponent::UnpausableTick(deltaTime);

       if (mIsRenderDataDirty)
      {
         SyncRenderData();
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
            if (const auto &lineProxySp = std::static_pointer_cast<RuntimeGeneratedLineSceneProxy>(sceneRenderer->GetPrimitiveProxyByProxyId(SceneProxyId)))
            {
               mIsRenderDataDirty = false;
               sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetObjectId(), functionId, [=]()
                                              {
                  lineProxySp->SetLineBeginWorldSpacePosition(mLineBeginWorldSpacePosition);
                  lineProxySp->SetLineEndWorldSpacePosition(mLineEndWorldSpacePosition);
                  lineProxySp->SetLineWidth(mLineWidth); });
            }
         }
      }
   }
}
