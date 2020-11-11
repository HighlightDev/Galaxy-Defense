#include "Level.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeDataContainer.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/GameCore/StateMachine/BindingAttachmentBuilder.h"

#include <glm/vec3.hpp>
#include <cereal/archives/xml.hpp>
#include <fstream>
#include <TinyLogger/LogInterface.h>

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

      container.PlayerControllerData = std::make_unique<SerializeDataPlayerController>(mScene->GetPlayerController()->GetBindedActor()->GameObjectName);

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
      //std::string t = TinyLogger::LogHelp::ToString<int>::Value(5);
      //TinyLogger::LogProxy::LogMessages(std::string("InstantiateLevelFromSerializedContainer"));

      for (const auto& actorData : container.Actors)
      {
         std::shared_ptr<Actor> actor = SerializeHelper::CreateActorFromSerializedData(actorData);

         //TinyLogger::LogProxy::LogMessages(std::string("Actor name: "), actor->GetName());

         for (const auto& componentData : actorData.ComponentsData)
         {
            auto component = SerializeHelper::CreateComponentFromSerializedData(mScene.get(), componentData);

            //TinyLogger::LogProxy::LogMessages(std::string("Component name: "), component->GameObjectName);

            if (component)
               actor->AddComponent(component);

            if (actor->GetName() == "SkeletBuddy")
            {
               auto meshComp = actor->GetComponent<SkeletalMeshComponent>();
               if (meshComp)
               {
                  actor->GetRootComponent()->AddOffsetUp(3);
               }
            }
         }

         mScene->AddActor(actor);

         if (actor->GameObjectName == container.PlayerControllerData->BindedActorName)
         {
            mScene->SetPlayerController(std::make_shared<PlayerController>(actor));
            if (auto thirdPersonCamera = static_cast<ThirdPersonCamera*>(mScene->GetCamera()))
            {
               thirdPersonCamera->SetThirdPersonTarget(actor);
            }
         }
      }

      // deserialize fsm
      for (const auto& actorData : container.Actors)
      {
         if (actorData.StateMachineData)
         {
            auto fsmSerializeData = actorData.StateMachineData;

            std::shared_ptr<StateMachine> actorFSM = SerializeHelper::CreateFsmFromSerializedData(fsmSerializeData);

            for (const auto& bindingData : fsmSerializeData->Bindings)
            {
               auto gameObject = mScene->GetGameObjectByName(bindingData.GameObjectName);
               const auto& binding = actorFSM->GetPropertyBindingByName(bindingData.BindingName);
               BindingAttachmentBuilder::SetAttachment(gameObject, binding.get(), bindingData.GameObjectPropertyName);
            }

            auto actorIt = std::find_if(mScene->GetActors().begin(), mScene->GetActors().end(), [&](const std::shared_ptr<Actor> actor)
            {
               return actor->GameObjectName == actorData.ActorName;
            });

            assert(actorIt != mScene->GetActors().end());

            (*actorIt)->AttachStateMachine(actorFSM);
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


