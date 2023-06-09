#include "Level.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeDataContainer.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/GameCore/HumanoidPlayerController.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Components/UiComponents/UiComponent.h"
#include "Core/GameCore/Components/ComponentCreators/UiComponentCreator.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"
#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "Core/CommonCore/Assertion.h"

#include <glm/vec3.hpp>
#include <cereal/archives/xml.hpp>
#include <fstream>

using namespace IO;
using namespace EngineUtility;

namespace EngineCore
{

   Level::Level()
       : mDebugDummyActor(),
         mRtTextField(),
         mGtTextField()
   {
   }

   Level::~Level()
   {
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

#ifdef DEBUG
      if (const auto& sceneSp = mSceneWp.lock())
      {
         mDebugDummyActor = std::make_shared<Actor>("Level Debug Dummy Actor",
                                                    std::make_shared<SceneComponent>("c_DebugDummyActor_rootComponent",
                                                                                     glm::vec3(),
                                                                                     glm::vec3(),
                                                                                     glm::vec3(1.0f)));
         const auto &uiComponentCreator = std::make_shared<UiComponentCreator<UiComponent>>();
         const auto &c_uiComponent = std::static_pointer_cast<UiComponent>(sceneSp->CreateComponent_GameThread(uiComponentCreator,
                                                                                                               ComponentData("c_uiComponent_DebugDummyActor")));
         mDebugDummyActor->AddComponent(c_uiComponent);
         sceneSp->AddActor(mDebugDummyActor);

         const size_t rtFpsTextId = c_uiComponent->CreateEmptyTextField("nimbus_mono", 10, glm::vec3(0.8, 0.0, 0.0), false, 0.3f, 1, eTextHorizontalAlignmentType::LEFT);
         const size_t gtFpsTextId = c_uiComponent->CreateEmptyTextField("nimbus_mono", 10, glm::vec3(0.0, 0.8, 0.0), false, 0.3f, 1, eTextHorizontalAlignmentType::LEFT);
         mRtTextField = c_uiComponent->GetTextFieldById(rtFpsTextId);
         mGtTextField = c_uiComponent->GetTextFieldById(gtFpsTextId);

         if (const auto &rtTextSp = mRtTextField.lock())
         {
            rtTextSp->SetPosition(glm::vec2(0.0f, 0.00f));
            rtTextSp->SetVisibility(true);
         }

         if (const auto &gtTextSp = mGtTextField.lock())
         {
            gtTextSp->SetPosition(glm::vec2(0.0f, 0.05f));
            gtTextSp->SetVisibility(true);
         }
      }
#endif
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
      LogInfo("Level::InstantiateLevelFromSerializedContainer");

      const std::vector<std::string> &resourceNames = SerializeHelper::GetSerializedAllocatedResources(container.Resources);
      for (const auto &resName : resourceNames)
      {
         ResourceMap::GetInstance()->AllocateAsync(resName);
      }

      ResourceMap::GetInstance()->WaitUntilResourcesLoad();

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
                  BindingAttachmentBuilder::SetAttachment(gameObject, binding.get(), bindingData.EngineObjectPropertyName);
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

#ifdef DEBUG

   void Level::SetRenderThreadFPSTextValue(const float fps)
   {
      if (const auto &rtTextSp = mRtTextField.lock())
      {
         const auto value = std::to_string(fps);
         rtTextSp->SetText("RT: " + value.substr(0, IndexOf(value, ".") + 2));
      }
   }

   void Level::SetGameThreadFPSTextValue(const float fps)
   {
      if (const auto &gtTextSp = mGtTextField.lock())
      {
         const auto value = std::to_string(fps);
         gtTextSp->SetText("GT: " + value.substr(0, IndexOf(value, ".") + 2));
      }
   }

#endif
}
