#include "LuaScriptExecutor_LevelBuilder.h"
#include "ComponentCreator.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirShadowInfo.h"
#include "Core/GameCore/GlobalSettings.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"

using namespace Graphics;

namespace Labyrinth
{

   LuaScriptExecutor_LevelBuilder::LuaScriptExecutor_LevelBuilder(std::weak_ptr<Scene> scene, const std::string& scriptName)
      : mActiveComponents()
      , mSceneWP(scene)
      , mScriptName(scriptName)
   {
   }

   LuaScriptExecutor_LevelBuilder::~LuaScriptExecutor_LevelBuilder()
   {
   }

   void LuaScriptExecutor_LevelBuilder::RegisterCallbacks()
   {
      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, Component*(std::string, ComponentData*)>::Register(mLuaInstance, "_CreateComponent");
      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, Actor*(std::string, glm::vec3, glm::vec3, glm::vec3)>::Register(mLuaInstance, "_CreateActor");
      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, void(Actor*, Component*)>::Register(mLuaInstance, "_AttachComponentToActor");

      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, ProjectedShadowInfo*(int32_t)>::Register(mLuaInstance, "_CreateDirLightProjectedShadowInfo");
      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, ComponentData*(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo*)>::Register(mLuaInstance, "_CreateDirLightComponentData");

      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, ComponentData*(std::string, glm::vec3, glm::vec3, glm::vec3, IMaterial*)>::Register(mLuaInstance, "_CreateMeshComponentData");
      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, ComponentData*(PhysicsDescriptor*)>::Register(mLuaInstance, "_CreatePhysicsComponentData");

      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, IMaterial*(std::string, LuaArgDummyPlaceholder)>::Register(mLuaInstance, "_CreateMaterial");
      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, void(IMaterial*, std::string, std::string) >::Register(mLuaInstance, "_SetTextureToMaterial");
      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, void(IMaterial*, float, std::string)>::Register(mLuaInstance, "_SetFloatToMaterial");

      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, PhyShapeBase*(glm::vec3)>::Register(mLuaInstance, "_CreatePhysicsBoxShape");

      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, PhysicsDescriptor*(PhyShapeBase*, float)>::Register(mLuaInstance, "_CreateRigidBodyController");
      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, PhysicsDescriptor*(float, float, float, float)>::Register(mLuaInstance, "_CreateDynamicCharacterController");
   }

   void LuaScriptExecutor_LevelBuilder::RunScript()
   {
      const auto folderManager = IO::FolderManager::GetInstance();

      const bool bScriptExecuted = mLuaInstance.ExecuteScript(folderManager->GetScriptPath() + mScriptName);

      assert((bScriptExecuted, "Lua script execution failure"));

      LuaFunction<void(void*)>::Call(mLuaInstance, "CreateTestLevel", (void*)this);
   }

   /* -------------------  Create Actor ----------------------------*/
   Actor* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3>& actorData)
   {
      Actor* createdActor = nullptr;

      if (auto scene = mSceneWP.lock())
      {
         std::shared_ptr<Actor> actor = std::make_shared<Actor>(std::get<0>(actorData),
            std::make_shared<SceneComponent>(std::get<1>(actorData), std::get<2>(actorData), std::get<3>(actorData)));

         scene->AllActors.push_back(actor);
         createdActor = actor.get();
      }

      return createdActor;
   }

   /* -------------------  Attach component to Actor ----------------------------*/
   void LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<Actor*, Component*>& dataToAttachActorToComponent)
   {
      Actor* actor = std::get<0>(dataToAttachActorToComponent);
      Component* component = std::get<1>(dataToAttachActorToComponent);

      assert((actor && component, "Actor or component is null"));

      assert((mActiveComponents.count(component->GetObjectId()), "Sought component doesn't exist"));

      actor->AddComponent(mActiveComponents[component->GetObjectId()]);
   }

   /* -------------------  Create component ----------------------------*/
   Component* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<std::string, ComponentData*>& componentData)
   {
      Component* createdComponent = nullptr;

      if (auto scene = mSceneWP.lock())
      {
         std::shared_ptr<Component> component = ComponentCreator::CreateComponentByString(std::get<0>(componentData), std::get<1>(componentData), scene.get());

         mActiveComponents[component->GetObjectId()] = component;
         createdComponent = component.get();
      }

      return createdComponent;
   }

   /* -------------------  Create mesh component data ----------------------------*/
   ComponentData* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, IMaterial*>& meshComponentData)
   {
      return ComponentCreator::CreateMeshComponentData(IO::FolderManager::GetInstance()->GetDirectoryRelativePathByFileName(std::get<0>(meshComponentData)),
         std::get<1>(meshComponentData), std::get<2>(meshComponentData),
         std::get<3>(meshComponentData), std::get<4>(meshComponentData));
   }

   /* -------------------  Create dir light component data ----------------------------*/
   ComponentData* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo*>& dirLightComponentData)
   {
      return ComponentCreator::CreateDirLightComponentData(std::get<0>(dirLightComponentData), std::get<1>(dirLightComponentData), std::get<2>(dirLightComponentData),
         std::get<3>(dirLightComponentData), std::get<4>(dirLightComponentData), std::get<5>(dirLightComponentData));
   }

   /* -------------------  Create physics component data ----------------------------*/
   ComponentData* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<PhysicsDescriptor*>& phyComponentData)
   {
      return ComponentCreator::CreatePhysicsComponentData(std::get<0>(phyComponentData));
   }

   /* -------------------  Create dir light projection shadow info --------------------*/
   ProjectedShadowInfo* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<int32_t>& dirLightProjectionData)
   {
      const int32_t shadowAtlasSize = std::get<0>(dirLightProjectionData);
      const float orthoHalfExtent = GlobalSettings::GetInstance()->GetShadowOrthoProjectionHalfExtent();

      auto directionalLightTextureAtlasRequest = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(shadowAtlasSize, shadowAtlasSize));
      ProjectedShadowInfo* dirShadowProjInfo = new ProjectedDirShadowInfo(directionalLightTextureAtlasRequest, orthoHalfExtent);

      return dirShadowProjInfo;
   }

   /* -------------------  Create material --------------------*/
   IMaterial* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<std::string, LuaArgDummyPlaceholder>& buildMaterial)
   {
      return MaterialParser::ParseMaterialDescriptor(IO::FolderManager::GetInstance()->GetDirectoryRelativePathByFileName(std::get<0>(buildMaterial)));
   }

   /* -------------------  Set texture --------------------*/
   void LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple <IMaterial*, std::string, std::string > & setTextureToMaterial)
   {
      IMaterial* material = std::get<0>(setTextureToMaterial);

      const std::string& filePath = IO::FolderManager::GetInstance()->GetDirectoryRelativePathByFileName(std::get<1>(setTextureToMaterial));
      std::shared_ptr<ITexture> texture = TexturePool::GetInstance()->GetOrAllocateResource(filePath);

      const std::string& propertyName = std::get<2>(setTextureToMaterial);
      MaterialPropertySetter::SetMaterialPropertyValue(material, propertyName, texture);
   }

   /* -------------------  Set float --------------------*/
   void LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<IMaterial*, float, std::string>& setFloatValueToMaterial)
   {
      IMaterial* material = std::get<0>(setFloatValueToMaterial);
      float value = std::get<1>(setFloatValueToMaterial);
      const std::string& propertyName = std::get<2>(setFloatValueToMaterial);
      MaterialPropertySetter::SetMaterialPropertyValue(material, propertyName, value);
   }

   /*-------------------- Create physics collision sphere shape --------------*/
   PhyShapeBase* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<float>& value)
   {
      return ComponentCreator::CreatePhysicsSphereShape(std::get<0>(value));
   }

   /*-------------------- Create physics collision box shape --------------*/
   PhyShapeBase* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<glm::vec3>& halfExtent)
   {
      return ComponentCreator::CreatePhysicsBoxShape(std::get<0>(halfExtent));
   }

   /*-------------------- Create physics collision capsule shape --------------*/
   PhyShapeBase* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<float, float>& capsuleData)
   {
      return ComponentCreator::CreatePhysicsCapsuleShape(std::get<0>(capsuleData), std::get<1>(capsuleData));
   }

   /*-------------------- Create physics collision plane shape --------------*/
   PhyShapeBase* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<glm::vec3, float> planeData)
   {
      return ComponentCreator::CreatePhysicsPlaneShape(std::get<0>(planeData), std::get<1>(planeData));
   }

   /*-------------------- Create rigid body controller--------------*/
   PhysicsDescriptor* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<PhyShapeBase*, float> descData)
   {
      PhysicsDescriptor* descriptor = nullptr;
      
      if (auto scene = mSceneWP.lock())
      {
         descriptor = ComponentCreator::CreateRigidBodyController(scene->mPhysicsWorld, std::get<0>(descData), std::get<1>(descData));
         scene->mPhysicsWorld->AddPhysDescriptor(descriptor);
      }

      return descriptor;
   }

   /*-------------------- Create dynamic character controller--------------*/
   PhysicsDescriptor* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<float, float, float, float> descData)
   {
      PhysicsDescriptor* descriptor = nullptr;

      if (auto scene = mSceneWP.lock())
      {
         descriptor = ComponentCreator::CreateDynamicCharacterController(scene->mPhysicsWorld, std::get<0>(descData), std::get<1>(descData),
            std::get<2>(descData), std::get<3>(descData));
         scene->mPhysicsWorld->AddPhysDescriptor(descriptor);
      }

      return descriptor;
   }
}
