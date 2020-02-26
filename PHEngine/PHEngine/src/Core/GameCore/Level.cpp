#include "Level.h"

#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"

#include "Core/GameCore/Components/BillboardComponent.h"
#include "Core/GameCore/Components/StaticMeshComponent.h"
#include "Core/GameCore/Components/SkeletalMeshComponent.h"
#include "Core/GameCore/Components/SkyboxComponent.h"
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
      : mScene(new Scene(interThreadMgr))
   {

   }

   Level::~Level()
   {
      delete mScene;
   }

   void Level::PostConstructorInitialize() 
   {
      mScene->PostConstructorInitialize();
   }

   void Level::InitLevel()
   {
      LoadLevel();
   }

   void Level::LoadLevel()
   {
   }

   void Level::TickLevel(const float deltaTime)
   {
      mScene->Tick_GameThread(deltaTime);
   }

   const std::vector<std::shared_ptr<Actor>>& Level::GetActors() const
   {
      return mScene->AllActors;
   }

   const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& Level::GetSceneProxies() const
   {
      return mScene->SceneProxies;
   }

   const std::vector<std::shared_ptr<LightSceneProxy>>& Level::GetLightProxies() const
   {
      return mScene->LightProxies;
   }

   const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& Level::GetShadowGroupPrimitives() const
   {
      return mScene->ShadowGroupPrimitives;
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


