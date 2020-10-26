#include "Level.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"

#include <glm/vec3.hpp>

namespace Game
{

   Level::Level(InterThreadCommunicationMgr& interThreadMgr)
      : mScene(std::make_shared<Scene>(interThreadMgr))
   {
   }

   Level::~Level()
   {
   }

   void Level::PreLevelInit()
   {
   }

   void Level::PostLevelInit()
   {
      mScene->PostLevelInit(mScene);
   }

   void Level::PostPhysicsInitialize()
   {
      mScene->PostPhysicsInitialize();
   }

   void Level::InitLevel()
   {
      LoadLevel();
   }

   std::weak_ptr<Scene> Level::GetSceneWP() const
   {
      return mScene;
   }

   void Level::LoadLevel()
   {
   }

   void Level::TickLevel(const float deltaTime)
   {
      mScene->Tick_GameThread(deltaTime);
   }

   ICamera* Level::GetCamera() const
   {
      return mScene->GetCamera();
   }

   void Level::CameraRotate()
   {
      mScene->GetCamera()->Rotate();
   }

   void Level::CameraMove()
   {
      if (ICamera* camera = mScene->GetCamera(); camera->GetCameraType() == ICamera::CameraType::FIRST_PERSON)
      {
         (static_cast<FirstPersonCamera*>(camera))->MoveCamera(0);
      }
   }
}


