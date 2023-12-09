#include "GameFlowController.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Input/KeyboardBindings.h"
#include "Core/GameCore/Input/MouseBindings.h"
#include "Core/GraphicsCore/SceneProxy/MainCameraSceneProxy.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/UtilityCore/ScreenRayCaster.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/SceneComponent.h"

using namespace IO;

namespace Game
{
   GameFlowController::GameFlowController(const std::weak_ptr<Scene> &sceneWp)
       : mSceneWp(sceneWp),
         mLevelBounds(),
         mInputComponent(std::make_unique<InputComponent>(std::make_shared<ComponentData>("GameFlowController_InputComponent"))),
         mMainSceneCamera(),
         mProjectionMatrix(),
         m_tempActorRootComponent()
   {
   }

   GameFlowController::~GameFlowController()
   {
      ChangeGameModeEvent::GetInstance()->RemoveListener(GetInstanceId());
   }

   void GameFlowController::Initialize()
   {
      ChangeGameModeEvent::GetInstance()->AddListener(shared_from_this());

      const auto &sceneSp = mSceneWp.lock();
      assert(sceneSp);
      const auto &mainCameraSp = std::dynamic_pointer_cast<ThirdPersonCamera>(sceneSp->GetMainCamera());
      assert(mainCameraSp);
      mMainSceneCamera = mainCameraSp;
   }

   void GameFlowController::SetLevelBounds(const BoundingBox3D &levelBounds)
   {
      mLevelBounds = levelBounds;
   }

   void GameFlowController::OnPreLevelInit()
   {
      Initialize();
   }

   void GameFlowController::OnLevelInit()
   {
   }

   void GameFlowController::OnPostLevelInit()
   {
   }

   void GameFlowController::PostPlayLevelFinished()
   {
   }

   void GameFlowController::CleanUp()
   {
   }

   void GameFlowController::Tick(const float deltaTime)
   {
      if (const auto &sceneCameraSp = mMainSceneCamera.lock())
      {
         if (eGameModeType::SPACE_STATION_PLACEMENT == mCurrentGameModeType)
         {
            const auto &mouseBindings = mInputComponent->GetMouseBindings();
            if (mouseBindings->IsMouseMoveEventDirty())
            {
               const auto &mouseMoveEvent = mouseBindings->FlushMouseMoveEvent();
               const glm::ivec2 &screenSpacePosition = glm::ivec2(mouseMoveEvent.x, mouseMoveEvent.y);
               const ScreenRayCaster screenRayCaster;
               const glm::vec3 &worldSpaceRay = screenRayCaster.CastRayFromScreenSpaceToWorldSpace(screenSpacePosition,
                                                                                                   glm::ivec2(DisplayDeviceDataProvider::GetInstance()->GetWindowWidth() - 1,
                                                                                                              DisplayDeviceDataProvider::GetInstance()->GetWindowHeight() - 1),
                                                                                                   mProjectionMatrix,
                                                                                                   sceneCameraSp->GetViewMatrix());
               const glm::vec4 planeAtOrigin = glm::vec4(0, 1, 0, 0);
               const float tParam = EngineMath::RaycastPlane(sceneCameraSp->GetEyeVector(), worldSpaceRay, planeAtOrigin);
               if (tParam >= 0.0f)
               {
                  const auto &placementPosition = sceneCameraSp->GetEyeVector() + (worldSpaceRay * tParam);
                  m_tempActorRootComponent->SetTranslation(placementPosition);
               }
            }

            if (mouseBindings->GetKeyState(eMouseKeys::MouseButtonLeft) == KeyState::PRESSED)
            {
               mCurrentGameModeType = eGameModeType::IDLE;
            }
         }
      }
   }

   void GameFlowController::UnpausableTick(const float deltaTime)
   {
   }

   void GameFlowController::ProcessEvent(const typename ChangeGameModeEvent::EventData_t &data)
   {
      const eGameModeType newGameModeType = std::get<0>(data);
      if (mCurrentGameModeType != newGameModeType)
      {
         mCurrentGameModeType = newGameModeType;
         if (eGameModeType::SPACE_STATION_PLACEMENT == mCurrentGameModeType)
         {
            PrepareForSpaceStationPlacementMode();
         }
      }
   }

   void GameFlowController::SetTempRootComponent(const std::shared_ptr<SceneComponent> &tempActorRootComponent)
   {
      m_tempActorRootComponent = tempActorRootComponent;
   }

   void GameFlowController::PrepareForSpaceStationPlacementMode()
   {
      if (const auto &sceneCameraSp = mMainSceneCamera.lock())
      {
         const auto viewPerspectiveInfo = sceneCameraSp->GetViewPerspectiveInfo();
         mProjectionMatrix = glm::perspective<float>(viewPerspectiveInfo.FoV,
                                                     viewPerspectiveInfo.AspectRatio,
                                                     viewPerspectiveInfo.NearPlane,
                                                     viewPerspectiveInfo.FarPlane);
      }
   }
}