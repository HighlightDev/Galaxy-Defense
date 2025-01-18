#include "UserInteractionController.h"
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
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"

#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"
#include "Implementation/Levels/CombatLevel/SmartPicker.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"

using namespace IO;
using namespace Graphics;

namespace Game
{
   UserInteractionController::UserInteractionController(const std::weak_ptr<Scene> &sceneWp)
       : mSceneWp(sceneWp),
         mLevelBounds(),
         mInputComponent(std::make_unique<InputComponent>(std::make_shared<ComponentData>("GameFlowController_InputComponent"))),
         mMainSceneCamera(),
         mProjectileMarkerActor(std::make_shared<Actor>("MissileProjectileActor",
                                                        std::make_shared<SceneComponent>("MissileProjectileRootComponent",
                                                                                         glm::vec3(),
                                                                                         glm::vec3(),
                                                                                         glm::vec3(1.0f))))
   {
      mReadyToShootTimer.SetIsPausable(true);
      mReadyToShootTimer.SetIsRepeat(false);
      mReadyToShootTimer.SetIntervalMs(500);
   }

   UserInteractionController::~UserInteractionController()
   {
      ChangeGameModeEvent::GetInstance()->RemoveListener(GetInstanceId());
   }

   void UserInteractionController::Initialize()
   {
      ChangeGameModeEvent::GetInstance()->AddListener(shared_from_this());

      const auto &sceneSp = mSceneWp.lock();
      assert(sceneSp);
      const auto &mainCameraSp = std::dynamic_pointer_cast<ThirdPersonCamera>(sceneSp->GetMainCamera());
      assert(mainCameraSp);
      mMainSceneCamera = mainCameraSp;

      const auto pickerCellSize = 10.0f;
      sceneSp->AddActor(mProjectileMarkerActor);

      MaterialParser materialParser;
      const std::shared_ptr<IMaterial> &missileProjectileMaterial = materialParser.ParseMaterialDescriptor("EditorPickerMaterial.m");
      sceneSp->RegisterMaterialInstance(missileProjectileMaterial);
      MaterialPropertySetter::SetMaterialPropertyValue(missileProjectileMaterial, "opacity", 1.0f);
      MaterialPropertySetter::SetMaterialPropertyValue(missileProjectileMaterial, "color", glm::vec3(1.0f, 0.0f, 0.0f));
      MaterialPropertySetter::SetMaterialPropertyValue(missileProjectileMaterial, sceneSp, "GT_DeltaSec", "gt_timeSec");

      const auto &meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(false);
      const auto &d_mesh = std::make_shared<MeshComponentData>("MissileProjectileMeshComponent",
                                                                             "plane.obj",
                                                                             glm::vec3(),
                                                                             glm::vec3(),
                                                                             glm::vec3(pickerCellSize, 1.0f, pickerCellSize),
                                                                             "",
                                                                             missileProjectileMaterial);
      const auto &c_mesh = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
      c_mesh->SetSortOrderValue(1);
      mProjectileMarkerActor->AddComponent(c_mesh);
      mProjectileMarkerActor->SetIsEnabled(false);
   }

   void UserInteractionController::SetLevelBounds(const BoundingBox3D &levelBounds)
   {
      mLevelBounds = levelBounds;
   }

   void UserInteractionController::OnPreLevelInit()
   {
   }

   void UserInteractionController::OnLevelInit()
   {
      assert(mCombatActorsPoolHandler);
      mSmartPicker = std::make_shared<SmartPicker>(mCombatActorsPoolHandler);
   }

   void UserInteractionController::OnPostLevelInit()
   {
      Initialize();
   }

   void UserInteractionController::PostPlayLevelFinished()
   {
   }

   void UserInteractionController::CleanUp()
   {
   }

   void UserInteractionController::Tick(const float deltaTime)
   {
      const auto &sceneSp = mSceneWp.lock();
      const auto &sceneCameraSp = mMainSceneCamera.lock();
      if (!sceneCameraSp || !sceneSp)
      {
         return;
      }

      const auto &mouseBindings = mInputComponent->GetMouseBindings();
      if (mouseBindings->IsMouseMoveEventDirty())
      {
         const auto &mouseMoveEvent = mouseBindings->FlushMouseMoveEvent();
         /*const glm::ivec2 &screenSpacePosition = glm::ivec2(mouseMoveEvent.x, mouseMoveEvent.y);

         if (eGameModeType::SPACE_STATION_PLACEMENT == mCurrentGameModeType)
         {
            const glm::vec4 planeAtOrigin = glm::vec4(0, 1, 0, 0);
            const auto &worldSpaceRay = mSmartPicker->CreateWorldSpaceRayFromScreenSpacePosition(sceneCameraSp, screenSpacePosition);
            const float tParam = EngineMath::RaycastPlane(sceneCameraSp->GetEyeVector(), worldSpaceRay, planeAtOrigin);
            if (tParam >= 0.0f)
            {
               const auto &placementPosition = sceneCameraSp->GetEyeVector() + (worldSpaceRay * tParam);
               // m_tempActorRootComponent->SetTranslation(placementPosition);
            }

            if (mouseBindings->GetKeyState(eMouseKeys::MouseButtonLeft) == KeyState::PRESSED)
            {
               mCurrentGameModeType = eGameModeType::COMBAT;
            }
         }*/
      }

      if (eGameModeType::COMBAT == mCurrentGameModeType)
      {
         if (mouseBindings->GetKeyState(eMouseKeys::MouseButtonLeft) == KeyState::PRESSED)
         {
            const auto &mousePosition = mouseBindings->GetLastMouseCursorPosition();
            const glm::ivec2 &screenSpacePosition = glm::ivec2(mousePosition.x, mousePosition.y);
            const int32_t collidedObjectId = mSmartPicker->CastScreenSpaceRayIntoScene(sceneSp, sceneCameraSp, screenSpacePosition);

            if (-1 != collidedObjectId &&
                eGameObjectsType::SPACE_STATION == mCombatActorsPoolHandler->GetGameObjectTypeByActorId(collidedObjectId))
            {
               mSelectedSpaceStationId = collidedObjectId;
               PlayerDataProvider::GetInstance()->SetSelectedTowerId(mSelectedSpaceStationId);
            }
            else if (!mProjectileMarkerActor->IsEnabled())
            {
               mSelectedSpaceStationId = -1;
               PlayerDataProvider::GetInstance()->SetSelectedTowerId(-1);
            }
            else if (mProjectileMarkerActor->IsEnabled() &&
                     mShootCallback &&
                     !mReadyToShootTimer.IsRunning())
            {
               mShootCallback();
               mReadyToShootTimer.StartTimer();
            }
         }
         else if (mProjectileMarkerActor->IsEnabled())
         {
            const auto &mousePosition = mouseBindings->GetLastMouseCursorPosition();
            const glm::ivec2 &screenSpacePosition = glm::ivec2(mousePosition.x, mousePosition.y);
            const auto &worldSpaceRay = mSmartPicker->CreateWorldSpaceRayFromScreenSpacePosition(sceneCameraSp, screenSpacePosition);

            const glm::vec4 planeAtOrigin = glm::vec4(0, 1, 0, 0);
            const float tParam = EngineMath::RaycastPlane(sceneCameraSp->GetEyeVector(), worldSpaceRay, planeAtOrigin);
            if (tParam >= 0.0f)
            {
               const auto &placementPosition = sceneCameraSp->GetEyeVector() + (worldSpaceRay * tParam);
               mProjectileMarkerActor->GetRootComponent()->SetTranslation(placementPosition);
            }
         }
      }
   }

   void UserInteractionController::UnpausableTick(const float deltaTime)
   {
   }

   int32_t UserInteractionController::GetSelectedSpaceStationId() const
   {
      return mSelectedSpaceStationId;
   }

   void UserInteractionController::ProcessEvent(const ChangeGameModeEvent* sender, const typename ChangeGameModeEvent::EventData_t &data)
   {
      const eGameModeType newGameModeType = std::get<0>(data);
      if (mCurrentGameModeType != newGameModeType)
      {
         mCurrentGameModeType = newGameModeType;
      }
   }

   void UserInteractionController::SetOnShootCallback(const std::function<void()> &callback)
   {
      mShootCallback = callback;
   }

   void UserInteractionController::SetActorsPoolHandler(const std::shared_ptr<CombatActorsPoolHandler> &combatActorsPoolHandler)
   {
      mCombatActorsPoolHandler = combatActorsPoolHandler;
   }

   void UserInteractionController::ShowMissileProjectile()
   {
      assert(eGameModeType::COMBAT == mCurrentGameModeType);
      mProjectileMarkerActor->SetIsEnabled(true);
   }

   void UserInteractionController::HideMissileProjectile()
   {
      mProjectileMarkerActor->SetIsEnabled(false);
   }

   glm::vec3 UserInteractionController::GetProjectileMarkerPosition() const
   {
      return mProjectileMarkerActor->GetRootComponent()->GetTranslation();
   }
}