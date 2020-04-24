#include "Level.h"

#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"

#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkyboxComponent.h"
#include "Core/GameCore/Components/PointLightComponent.h"
#include "Core/GameCore/Components/DirectionalLightComponent.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/MovementComponent.h"

#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/ComponentData/StaticMeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/SkeletalMeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/SkyboxComponentData.h"
#include "Core/GameCore/Components/ComponentData/PointLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/InputComponentData.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"

#include "Core/GraphicsCore/Material/PBRMaterial.h"

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

   void Level::PostConstructorInitialize() 
   {
      mScene->PostConstructorInitialize();
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


