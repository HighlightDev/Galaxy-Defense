#include "LuaScriptExecutor_EngineObjectsCreator.h"
#include "LuaToCPPAdapter.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirShadowInfo.h"
#include "Core/GameCore/GlobalSettings.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GameCore/ThirdPersonCamera.h"

using namespace Graphics;

namespace Game
{

   LuaScriptExecutor_EngineObjectsCreator::LuaScriptExecutor_EngineObjectsCreator(std::weak_ptr<Scene> scene, const std::string& scriptName)
      : mActiveComponents()
      , mSceneWP(scene)
      , mScriptName(scriptName)
   {
   }

   LuaScriptExecutor_EngineObjectsCreator::~LuaScriptExecutor_EngineObjectsCreator()
   {
   }

   void LuaScriptExecutor_EngineObjectsCreator::RegisterCallbacks()
   {
      LuaRegisterCallback<LuaScriptExecutor_EngineObjectsCreator, Component*(std::string, ComponentData*)>::Register(mLuaInstance, "_CreateComponent");
      LuaRegisterCallback<LuaScriptExecutor_EngineObjectsCreator, Actor*(std::string, std::string, glm::vec3, glm::vec3, glm::vec3)>::Register(mLuaInstance, "_CreateActor");
      LuaRegisterCallback<LuaScriptExecutor_EngineObjectsCreator, void(Actor*, Component*)>::Register(mLuaInstance, "_AttachComponentToActor");

      LuaRegisterCallback<LuaScriptExecutor_EngineObjectsCreator, ProjectedShadowInfo*(int32_t)>::Register(mLuaInstance, "_CreateDirLightProjectedShadowInfo");
      LuaRegisterCallback<LuaScriptExecutor_EngineObjectsCreator, ComponentData*(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo*)>::Register(mLuaInstance, "_CreateDirLightComponentData");

      LuaRegisterCallback<LuaScriptExecutor_EngineObjectsCreator, ComponentData*(std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial*)>::Register(mLuaInstance, "_CreateMeshComponentData");
      LuaRegisterCallback<LuaScriptExecutor_EngineObjectsCreator, ComponentData*(PhysicsDescriptor*)>::Register(mLuaInstance, "_CreatePhysicsComponentData");

      LuaRegisterCallback<LuaScriptExecutor_EngineObjectsCreator, IMaterial*(std::string, LuaArgDummyPlaceholder)>::Register(mLuaInstance, "_CreateMaterial");
      LuaRegisterCallback<LuaScriptExecutor_EngineObjectsCreator, void(IMaterial*, std::string, std::string) >::Register(mLuaInstance, "_SetTextureToMaterial");
      LuaRegisterCallback<LuaScriptExecutor_EngineObjectsCreator, void(IMaterial*, float, std::string)>::Register(mLuaInstance, "_SetFloatToMaterial");

      LuaRegisterCallback<LuaScriptExecutor_EngineObjectsCreator, PhyShapeBase*(glm::vec3)>::Register(mLuaInstance, "_CreatePhysicsBoxShape");

      LuaRegisterCallback<LuaScriptExecutor_EngineObjectsCreator, PhysicsDescriptor*(PhyShapeBase*, float)>::Register(mLuaInstance, "_CreateRigidBodyController");
      LuaRegisterCallback<LuaScriptExecutor_EngineObjectsCreator, PhysicsDescriptor*(float, float, float, float)>::Register(mLuaInstance, "_CreateDynamicCharacterController");
   }

   void LuaScriptExecutor_EngineObjectsCreator::RunScript()
   {
      const auto folderManager = IO::FolderManager::GetInstance();

      const bool bScriptExecuted = mLuaInstance.ExecuteScript(folderManager->GetScriptPath() + mScriptName);

      assert((bScriptExecuted, "Lua script execution failure"));

      LuaFunction<void(void*)>::Call(mLuaInstance, "CreateTestLevel", (void*)this);
   }

   /* -------------------  Create Actor ----------------------------*/
   Actor* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, std::string, glm::vec3, glm::vec3, glm::vec3>& actorData)
   {
      Actor* createdActor = nullptr;

      if (auto scene = mSceneWP.lock())
      {
       
         auto actorSP = LuaToCPPAdapter::CreateActorByString(std::get<0>(actorData), std::get<1>(actorData), 
            std::make_shared<Game::SceneComponent>(std::get<2>(actorData), std::get<3>(actorData), std::get<4>(actorData)));
         scene->AllActors.push_back(actorSP);
         createdActor = actorSP.get();
      }

      return createdActor;
   }

   /* -------------------  Attach component to Actor ----------------------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<Actor*, Component*>& dataToAttachActorToComponent)
   {
      Actor* actor = std::get<0>(dataToAttachActorToComponent);
      Component* component = std::get<1>(dataToAttachActorToComponent);

      assert((actor && component, "Actor or component is null"));

      const bool componentExists = mActiveComponents.count(component->GetObjectId());
      assert((componentExists, "Sought component doesn't exist"));

      actor->AddComponent(mActiveComponents[component->GetObjectId()]);
   }

   /* ------------------- Attach actor to player controller ----------------------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<Actor*>& actorData)
   {
      Actor* actor = std::get<0>(actorData);
      assert((actor, "Actor is null"));

      if (auto scene = mSceneWP.lock())
      {
         ICamera* camera = scene->GetCamera();
         assert(ICamera::CameraType::THIRD_PERSON == camera->GetCameraType());

         auto actorIt = std::find_if(scene->AllActors.begin(), scene->AllActors.end(), [&](const std::shared_ptr<Actor>& sceneActor) { return sceneActor->GetObjectId() == actor->GetObjectId(); });
         assert(actorIt != scene->AllActors.end());

         scene->m_playerController.SetPlayerActor(*actorIt);
         static_cast<ThirdPersonCamera*>(camera)->SetThirdPersonTarget(*actorIt);
      }
   }

   /* -------------------  Create component ----------------------------*/
   Component* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, ComponentData*>& componentData)
   {
      Component* createdComponent = nullptr;

      if (auto scene = mSceneWP.lock())
      {
         std::shared_ptr<Component> component = LuaToCPPAdapter::CreateComponentByString(std::get<0>(componentData), std::get<1>(componentData), scene.get());

         mActiveComponents[component->GetObjectId()] = component;
         createdComponent = component.get();
      }

      return createdComponent;
   }

   /* -------------------  Create mesh component data ----------------------------*/
   ComponentData* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial*>& meshComponentData)
   {
      return LuaToCPPAdapter::CreateMeshComponentData(IO::FolderManager::GetInstance()->GetDirectoryRelativePathByFileName(std::get<0>(meshComponentData)),
         std::get<1>(meshComponentData), std::get<2>(meshComponentData),
         std::get<3>(meshComponentData), std::get<4>(meshComponentData), std::get<5>(meshComponentData));
   }

   /* -------------------  Create dir light component data ----------------------------*/
   ComponentData* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo*>& dirLightComponentData)
   {
      return LuaToCPPAdapter::CreateDirLightComponentData(std::get<0>(dirLightComponentData), std::get<1>(dirLightComponentData), std::get<2>(dirLightComponentData),
         std::get<3>(dirLightComponentData), std::get<4>(dirLightComponentData), std::get<5>(dirLightComponentData));
   }

   /* -------------------  Create physics component data ----------------------------*/
   ComponentData* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<PhysicsDescriptor*>& phyComponentData)
   {
      return LuaToCPPAdapter::CreatePhysicsComponentData(std::get<0>(phyComponentData));
   }

   /* -------------------  Create input component data ----------------------------*/
   ComponentData* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<>& inputComponentData)
   {
      return LuaToCPPAdapter::CreateInputComponentData();
   }

   /* -------------------  Create movement component data ----------------------------*/
   ComponentData* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<glm::vec3, std::string>& movementComponentData)
   {
      return LuaToCPPAdapter::CreateMovementComponentData(std::get<0>(movementComponentData), std::get<1>(movementComponentData));
   }

   /* -------------------  Create dir light projection shadow info --------------------*/
   ProjectedShadowInfo* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<int32_t>& dirLightProjectionData)
   {
      const int32_t shadowAtlasSize = std::get<0>(dirLightProjectionData);
      const float orthoHalfExtent = GlobalSettings::GetInstance()->GetShadowOrthoProjectionHalfExtent();

      auto directionalLightTextureAtlasRequest = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(shadowAtlasSize, shadowAtlasSize));
      ProjectedShadowInfo* dirShadowProjInfo = new ProjectedDirShadowInfo(directionalLightTextureAtlasRequest, orthoHalfExtent);

      return dirShadowProjInfo;
   }

   /* -------------------  Create material --------------------*/
   IMaterial* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, LuaArgDummyPlaceholder>& buildMaterial)
   {
      return MaterialParser::ParseMaterialDescriptor(IO::FolderManager::GetInstance()->GetDirectoryRelativePathByFileName(std::get<0>(buildMaterial)));
   }

   /* -------------------  Set texture --------------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple <IMaterial*, std::string, std::string > & setTextureToMaterial)
   {
      IMaterial* material = std::get<0>(setTextureToMaterial);

      const std::string& filePath = IO::FolderManager::GetInstance()->GetDirectoryRelativePathByFileName(std::get<1>(setTextureToMaterial));
      std::shared_ptr<ITexture> texture = TexturePool::GetInstance()->GetOrAllocateResource(filePath);

      const std::string& propertyName = std::get<2>(setTextureToMaterial);
      MaterialPropertySetter::SetMaterialPropertyValue(material, propertyName, texture);
   }

   /* -------------------  Set float --------------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<IMaterial*, float, std::string>& setFloatValueToMaterial)
   {
      IMaterial* material = std::get<0>(setFloatValueToMaterial);
      float value = std::get<1>(setFloatValueToMaterial);
      const std::string& propertyName = std::get<2>(setFloatValueToMaterial);
      MaterialPropertySetter::SetMaterialPropertyValue(material, propertyName, value);
   }

   /*-------------------- Create physics collision sphere shape --------------*/
   PhyShapeBase* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<float>& value)
   {
      return LuaToCPPAdapter::CreatePhysicsSphereShape(std::get<0>(value));
   }

   /*-------------------- Create physics collision box shape --------------*/
   PhyShapeBase* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<glm::vec3>& halfExtent)
   {
      return LuaToCPPAdapter::CreatePhysicsBoxShape(std::get<0>(halfExtent));
   }

   /*-------------------- Create physics collision capsule shape --------------*/
   PhyShapeBase* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<float, float>& capsuleData)
   {
      return LuaToCPPAdapter::CreatePhysicsCapsuleShape(std::get<0>(capsuleData), std::get<1>(capsuleData));
   }

   /*-------------------- Create physics collision plane shape --------------*/
   PhyShapeBase* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<glm::vec3, float> planeData)
   {
      return LuaToCPPAdapter::CreatePhysicsPlaneShape(std::get<0>(planeData), std::get<1>(planeData));
   }

   /*-------------------- Create rigid body controller--------------*/
   PhysicsDescriptor* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<PhyShapeBase*, float> descData)
   {
      PhysicsDescriptor* descriptor = nullptr;
      
      if (auto scene = mSceneWP.lock())
      {
         descriptor = LuaToCPPAdapter::CreateRigidBodyController(scene->mPhysicsWorld, std::get<0>(descData), std::get<1>(descData));
         scene->mPhysicsWorld->AddPhysDescriptor(descriptor);
      }

      return descriptor;
   }

   /*-------------------- Create dynamic character controller--------------*/
   PhysicsDescriptor* LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<float, float, float, float> descData)
   {
      PhysicsDescriptor* descriptor = nullptr;

      if (auto scene = mSceneWP.lock())
      {
         descriptor = LuaToCPPAdapter::CreateDynamicCharacterController(scene->mPhysicsWorld, std::get<0>(descData), std::get<1>(descData),
            std::get<2>(descData), std::get<3>(descData));
         scene->mPhysicsWorld->AddPhysDescriptor(descriptor);
      }

      return descriptor;
   }
}
