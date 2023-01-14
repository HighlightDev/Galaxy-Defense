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

   void RuntimeGeneratedLineComponent::Tick(const float deltaTime)
   {
      StaticMeshComponent::Tick(deltaTime);

      if (mIsRenderDataDirty)
      {
         SyncRenderData();
         mIsRenderDataDirty = false;
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

   void RuntimeGeneratedLineComponent::SyncRenderData()
   {
      static constexpr uint64_t functionId = Hash64_CT("RuntimeGeneratedLineComponent::SyncRenderData");
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
                  lineProxySp->SetLineEndWorldSpacePosition(mLineEndWorldSpacePosition);
               } 
            });
         }
      }
   }
}
