#include "Level.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeDataContainer.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/GameCore/HumanoidPlayerController.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/MeshPoolParameters.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/Assertion.h"

#include <glm/vec3.hpp>
#include <cereal/archives/xml.hpp>
#include <fstream>
#include <algorithm>

using namespace IO;
using namespace EngineUtility;
using namespace Resources;

namespace EngineCore
{

   Level::Level(const std::string &levelName)
       : mLevelName(levelName)
   {
   }

   Level::~Level()
   {
   }

   std::string Level::GetLevelName() const
   {
      return mLevelName;
   }

   void Level::SetScene(const std::shared_ptr<Scene> &scene)
   {
      assert(scene);
      mSceneWp = scene;
   }

   void Level::PreLevelInit()
   {
      LogInfo("Level::PreLevelInit");
   }

   void Level::PostLevelInit()
   {
      LogInfo("Level::PostLevelInit");
   }

   void Level::PostPlayLevelFinished()
   {
      LogInfo("Level::PostPlayLevelFinished");
   }

   void Level::PostPhysicsInitialize()
   {
      LogInfo("Level::PostPhysicsInitialize");
   }

   void Level::InitLevel()
   {
      LogInfo("Level::InitLevel");
   }

   std::weak_ptr<Scene> Level::GetSceneWP() const
   {
      return mSceneWp;
   }

   void Level::SerializeLevel(const std::string &pathToFolder)
   {
      std::ofstream os(pathToFolder);
      cereal::XMLOutputArchive oarchive(os);

      SerializeDataContainer container;

      if (const auto &sceneSp = mSceneWp.lock())
      {
         CollectAllocatedResourcesForSerialization(container);

         for (auto &actor : sceneSp->GetActors())
         {
            actor->CollectDataForSerialization(container);
         }

         for (auto &camera : sceneSp->GetActiveCameras())
         {
            camera->CollectDataForSerialization(container);
         }

         for (auto &actorController : sceneSp->GetActorControllers())
         {
            actorController->CollectDataForSerialization(container);
         }
      }
      else
      {
         LogInfo("Level::SerializeLevel => Error. Scene was destroyed");
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
      LogInfo("CollectAllocatedResourcesForSerialization");

      std::vector<std::string> loadedTextureNames = TexturePool::GetInstance()->GetAllKeys();
      const auto &meshParamsList = MeshPool::GetInstance()->GetAllKeys();
      std::vector<std::string> loadedModelNames;
      loadedModelNames.reserve(meshParamsList.size());
      std::transform(meshParamsList.cbegin(), meshParamsList.cend(), std::back_inserter(loadedModelNames), [](const MeshPoolParameters &meshParam)
                     { return meshParam.mModelPath; });

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
      LogInfo("Level::InstantiateLevelFromSerializedContainer");

      const std::vector<std::string> &resourceNames = SerializeHelper::GetSerializedAllocatedResources(container.Resources);
      for (const auto &resName : resourceNames)
      {
         ResourceMap::GetInstance()->AllocateAsync(resName);
      }

      if (const auto &sceneSp = mSceneWp.lock())
      {
         for (const auto &cameraData : container.Cameras)
         {
            bool outIsMainSceneCamera = false;
            auto camera = SerializeHelper::CreateCameraFromSerializedData(sceneSp, cameraData, outIsMainSceneCamera);

            if (outIsMainSceneCamera)
            {
               sceneSp->RegisterMainCamera(camera);
            }
            else
            {
               sceneSp->RegisterCamera(camera);
            }

            // Deserialize planar reflection component
            if (camera && cameraData->mPlanarReflectionComponentData)
            {
               SerializeHelper::CreateComponentFromSerializedData(sceneSp, cameraData->mPlanarReflectionComponentData);
            }
         }

         for (const auto &actorData : container.Actors)
         {
            std::shared_ptr<Actor> actor = SerializeHelper::CreateActorFromSerializedData(actorData);

            LogInfo("Level::InstantiateLevelFromSerializedContainer => Actor name: ", actor->GetName());

            for (const auto &componentData : actorData.ComponentsData)
            {
               const auto &component = SerializeHelper::CreateComponentFromSerializedData(sceneSp, componentData);

               LogInfo("Level::InstantiateLevelFromSerializedContainer => Component name: ", component->GetEngineObjectName());

               if (component)
               {
                  actor->AddComponent(component);
               }
            }

            sceneSp->AddActor(actor);
         }

         for (const auto &actorControllerData : container.ActorControllerData)
         {
            const auto &bindedActor = sceneSp->GetActorByName(actorControllerData->BindedActorName);
            sceneSp->AddActorController(std::make_shared<HumanoidPlayerController>(sceneSp->GetMainCamera(), bindedActor));
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
                  auto gameObject = sceneSp->GetEngineObjectByName(bindingData.EngineObjectName);
                  const auto &binding = actorTweener->GetPropertyBindingByName(bindingData.BindingName);
                  BindingAttachmentBuilder::SetAttachment(gameObject, binding, bindingData.EngineObjectPropertyName);
               }

               auto actorIt = std::find_if(sceneSp->GetActors().begin(), sceneSp->GetActors().end(), [&](const std::shared_ptr<Actor> actor)
                                           { return actor->GetEngineObjectName() == actorData.ActorName; });

               assert(actorIt != sceneSp->GetActors().end());

               (*actorIt)->AttachTweener(actorTweener);
            }
         }
      }
      else
      {
         LogInfo("Level::InstantiateLevelFromSerializedContainer => Error. Scene was destroyed");
      }

      TextureAtlasFactory::GetInstance()->AllocateAtlasSpace();
   }

   void Level::Tick(const float deltaTime)
   {
   }

   void Level::UnpausableTick(const float deltaTime)
   {
   }

   void Level::RestartLuaScripts()
   {
   }
}
