#include "MainCameraSceneProxy.h"
#include "Core/GameCore/ACamera.h"

using namespace Game;

namespace Graphics
{

   MainCameraSceneProxy::MainCameraSceneProxy(const ACamera* camera)
      : CameraSceneProxy(camera)
   {
   }

   MainCameraSceneProxy::~MainCameraSceneProxy()
   {
   }

   eCameraSceneProxyType MainCameraSceneProxy::GetCameraSceneType() const
   {
      return eCameraSceneProxyType::MAIN_SCENE_CAMERA;
   }

}