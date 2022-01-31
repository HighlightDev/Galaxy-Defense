#include "Level.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeDataContainer.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/GameCore/GlobalSettings.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"

#include <glm/vec3.hpp>
#include <cereal/archives/xml.hpp>
#include <fstream>
#include <TinyLogger/LogInterface.h>

namespace Game
{

   Level::Level(InterThreadCommunicationMgr &interThreadMgr)
       : mScene(std::make_shared<Scene>(interThreadMgr))
   {
      auto sharedFromMePtr = mScene->GetSharedFromMe();
      mScene->SetMeSharedPtr(sharedFromMePtr);
   }

   Level::~Level()
   {
   }

   void Level::PreLevelInit()
   {
      TinyLogger::LogProxy::LogMessages("Level::PreLevelInit");
   }

   void Level::PostLevelInit()
   {
      TinyLogger::LogProxy::LogMessages("Level::PostLevelInit");
      mScene->PostLevelInit();

      ResourceMap::DeleteInstance();
      TextureAtlasFactory::GetInstance()->AllocateAtlasSpace();
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

   void Level::SerializeLevel(const std::string &pathToFolder)
   {

      std::ofstream os(pathToFolder);
      cereal::XMLOutputArchive oarchive(os);

      SerializeDataContainer container;

      CollectAllocatedResourcesForSerialization(container);

      for (auto &actor : mScene->GetActors())
      {
         actor->CollectDataForSerialization(container);
      }

      for (auto &camera : mScene->GetActiveCameras())
      {
         camera->CollectDataForSerialization(container);
      }

      container.PlayerControllerData =
          std::make_unique<SerializeDataPlayerController>(mScene->GetPlayerController()->GetBindedActor()->GetGameObjectName());

      oarchive(container);
   }

   void Level::DeserializeLevel(const std::string &pathToFile)
   {
      std::ifstream is(pathToFile);
      cereal::XMLInputArchive iarchive(is);
      SerializeDataContainer container;
      iarchive(container);

      InstantiateLevelFromSerializedContainer(container);
   }

   void Level::CollectAllocatedResourcesForSerialization(SerializeDataContainer &container)
   {
      TinyLogger::LogProxy::LogMessages("CollectAllocatedResourcesForSerialization");

      std::vector<std::string> loadedTextureNames = TexturePool::GetInstance()->GetAllKeys();
      std::vector<std::string> loadedModelNames = MeshPool::GetInstance()->GetAllKeys();
      std::vector<std::string> concatNamesVec;
      concatNamesVec.reserve(loadedTextureNames.size() + loadedModelNames.size());
      concatNamesVec.insert(concatNamesVec.end(),
                            std::make_move_iterator(loadedTextureNames.begin()),
                            std::make_move_iterator(loadedTextureNames.end()));

      concatNamesVec.insert(concatNamesVec.end(),
                            std::make_move_iterator(loadedModelNames.begin()),
                            std::make_move_iterator(loadedModelNames.end()));

      container.Resources.ResourceNames = std::move(concatNamesVec);
   }

   void Level::InstantiateLevelFromSerializedContainer(SerializeDataContainer &container)
   {
      TinyLogger::LogProxy::LogMessages("InstantiateLevelFromSerializedContainer");

      const std::vector<std::string> &resourceNames = SerializeHelper::GetSerializedAllocatedResources(container.Resources);
      for (const auto &resName : resourceNames)
      {
         ResourceMap::GetInstance()->AllocateAsync(resName);
      }

      ResourceMap::GetInstance()->WaitUntilResourcesLoad();

      for (const auto &cameraData : container.Cameras)
      {
         bool outIsMainSceneCamera = false;
         auto camera = SerializeHelper::CreateCameraFromSerializedData(mScene, cameraData, outIsMainSceneCamera);

         if (outIsMainSceneCamera)
         {
            mScene->RegisterMainCamera(camera);
         }
         else
         {
            mScene->RegisterCamera(camera);
         }

         // Deserialize planar reflection component
         if (camera && cameraData->mPlanarReflectionComponentData)
         {
            SerializeHelper::CreateComponentFromSerializedData(mScene, cameraData->mPlanarReflectionComponentData);
         }
      }

      for (const auto &actorData : container.Actors)
      {
         std::shared_ptr<Actor> actor = SerializeHelper::CreateActorFromSerializedData(actorData);

         TinyLogger::LogProxy::LogMessages("Actor name: ", actor->GetName());

         for (const auto &componentData : actorData.ComponentsData)
         {
            const auto &component = SerializeHelper::CreateComponentFromSerializedData(mScene, componentData);

            TinyLogger::LogProxy::LogMessages("Component name: ", component->GetGameObjectName());

            if (component)
               actor->AddComponent(component);
         }

         mScene->AddActor(actor);

         if (actor->GetGameObjectName() == container.PlayerControllerData->BindedActorName)
         {
            mScene->SetPlayerController(std::make_shared<PlayerController>(mScene->GetMainCamera(), actor));
         }
      }

      // deserialize tweener
      for (const auto &actorData : container.Actors)
      {
         if (actorData.TweenerData)
         {
            auto data = actorData.TweenerData;

            std::shared_ptr<Tweener> actorTweener = SerializeHelper::CreateTweenerFromSerializedData(data);

            for (const auto &bindingData : data->Bindings)
            {
               auto gameObject = mScene->GetGameObjectByName(bindingData.GameObjectName);
               const auto &binding = actorTweener->GetPropertyBindingByName(bindingData.BindingName);
               BindingAttachmentBuilder::SetAttachment(gameObject, binding.get(), bindingData.GameObjectPropertyName);
            }

            auto actorIt = std::find_if(mScene->GetActors().begin(), mScene->GetActors().end(), [&](const std::shared_ptr<Actor> actor)
                                        { return actor->GetGameObjectName() == actorData.ActorName; });

            assert(actorIt != mScene->GetActors().end());

            (*actorIt)->AttachTweener(actorTweener);
         }
      }

      TextureAtlasFactory::GetInstance()->AllocateAtlasSpace();
   }

   void Level::TickLevel(const float deltaTime)
   {
      mScene->Tick_GameThread(deltaTime);
   }
}
