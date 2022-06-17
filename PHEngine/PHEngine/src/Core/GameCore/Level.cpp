#include "Level.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeDataContainer.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/GameCore/HumanoidPlayerController.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

#include <glm/vec3.hpp>
#include <cereal/archives/xml.hpp>
#include <fstream>
#include <TinyLogger/LogInterface.h>

using namespace TinyLogger;
using namespace IO;

namespace EngineCore
{

   Level::Level(InterThreadCommunicationMgr &interThreadMgr)
       : mScene(std::make_shared<Scene>(interThreadMgr))
   {
   }

   Level::~Level()
   {
   }

   void Level::PreLevelInit()
   {
      Logger::Out("Level::PreLevelInit");
   }

   void Level::PostLevelInit()
   {
      Logger::Out("Level::PostLevelInit");
      mScene->PostLevelInit();

      ResourceMap::DeleteInstance();
      TextureAtlasFactory::GetInstance()->AllocateAtlasSpace();
   }

   void Level::PostPlayLevelFinished()
   {
      Logger::Out("Level::PostPlayLevelFinished");
      mScene->PostPlayLevelFinished();
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

      for (auto &actorController : mScene->GetActorControllers())
      {
         actorController->CollectDataForSerialization(container);
      }

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
      Logger::Out("CollectAllocatedResourcesForSerialization");

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
      Logger::Out("Level::InstantiateLevelFromSerializedContainer");

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

         Logger::Out("Level::InstantiateLevelFromSerializedContainer => Actor name: ", actor->GetName());

         for (const auto &componentData : actorData.ComponentsData)
         {
            const auto &component = SerializeHelper::CreateComponentFromSerializedData(mScene, componentData);

            Logger::Out("Level::InstantiateLevelFromSerializedContainer => Component name: ", component->GetGameObjectName());

            if (component)
            {
               actor->AddComponent(component);
            }
         }

         mScene->AddActor(actor);
      }

      for (const auto &actorControllerData : container.ActorControllerData)
      {
         const auto &bindedActor = mScene->GetActorByName(actorControllerData->BindedActorName);
         mScene->AddActorController(std::make_shared<HumanoidPlayerController>(mScene->GetMainCamera(), bindedActor));
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
