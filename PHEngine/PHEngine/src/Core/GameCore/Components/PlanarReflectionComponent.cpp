#include "PlanarReflectionComponent.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GraphicsCore/SceneProxy/PlanarReflectionProxy.h"

using namespace Graphics;

namespace Game {

   PlanarReflectionComponent::PlanarReflectionComponent(const std::string& gameObjectName, glm::vec3 translation,
      glm::vec3 rotation, glm::vec3 scale, ACamera* ownerCamera, const ::Graphics::ViewPortInfo& renderTargetViewPortInfo)
      : SceneComponent(gameObjectName, translation, rotation, scale)
      , mOwnerCamera(ownerCamera)
      , mRenderTargetViewPortInfo(renderTargetViewPortInfo)
   {
   }

   PlanarReflectionComponent::~PlanarReflectionComponent()
   {
   }

   ::Graphics::ViewPortInfo PlanarReflectionComponent::GetRenderTargetViewPortInfo() const {

      return mRenderTargetViewPortInfo;
   }

   std::shared_ptr<PlanarReflectionProxy> PlanarReflectionComponent::CreatePlanarReflectionProxy()
   {
      return std::make_shared<PlanarReflectionProxy>(this);
   }

   void PlanarReflectionComponent::Tick(const float deltaTime)
   {
      if (mIsEnabled)
      {
         if (bTransformationDirty)
         {
            UpdateRelativeMatrix();
         }
      }
   }

   ACamera* PlanarReflectionComponent::GetOwnerCamera() const
   {
      return mOwnerCamera;
   }

   size_t PlanarReflectionComponent::GetSceneProxyId() const
   {
      return mPlanarReflectionSceneProxyId;
   }

   void PlanarReflectionComponent::SetSceneProxyId(const size_t sceneProxyId)
   {
      mPlanarReflectionSceneProxyId = sceneProxyId;
   }

   ComponentType PlanarReflectionComponent::GetComponentType() const {
      return PLANAR_REFLECTION_COMPONENT;
   }

}