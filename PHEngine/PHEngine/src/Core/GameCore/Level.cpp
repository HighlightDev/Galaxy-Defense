#include "Level.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeDataContainer.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"

#include <glm/vec3.hpp>
#include <cereal/archives/xml.hpp>
#include <fstream>

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

   void Level::SerializeLevel(const std::string& pathToFolder) {

      std::ofstream os(pathToFolder);
      cereal::XMLOutputArchive oarchive(os);

      SerializeDataContainer container;

      for (auto& actor : mScene->GetActors())
      {
         actor->CollectDataForSerialization(container);
      }

      oarchive(container);
   }

   void Level::DeserializeLevel(const std::string& pathToFile)
   {
      std::ifstream is(pathToFile);
      cereal::XMLInputArchive iarchive(is);
      SerializeDataContainer container;
      iarchive(container);

      InstantiateLevelFromSerializedContainer(container);
   }

   void Level::InstantiateLevelFromSerializedContainer(SerializeDataContainer& container) 
   {
      for (const auto& actorData : container.Actors)
      {
         std::shared_ptr<Actor> actor = SerializeHelper::CreateActorFromSerializedData(actorData);

         for (const auto& componentData : actorData.ComponentsData)
         {
            auto component = SerializeHelper::CreateComponentFromSerializedData(mScene.get(), componentData);
            if (component)
               actor->AddComponent(component);
         }

         if (actorData.StateMachineData)
         {
            std::shared_ptr<StateMachine> actorFSM = SerializeHelper::CreateFsmFromSerializedData(actorData.StateMachineData);

         }
      }

      TextureAtlasFactory::GetInstance()->AllocateAtlasSpace();
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


