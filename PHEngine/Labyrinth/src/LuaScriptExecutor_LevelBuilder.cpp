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
      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, Component*(std::string, ComponentData*)> ::Rigister(mLuaInstance, "_CreateComponent");
      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, Actor*(std::string, glm::vec3, glm::vec3, glm::vec3)> ::Rigister(mLuaInstance, "_CreateActor");
      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, void(Actor*, Component*)> ::Rigister(mLuaInstance, "_AttachComponentToActor");

      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, ProjectedShadowInfo*(int32_t)> ::Rigister(mLuaInstance, "_CreateDirLightProjectedShadowInfo");
      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, ComponentData*(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo*)> ::Rigister(mLuaInstance, "_CreateDirLightComponentData");

      LuaRegisterCallback<LuaScriptExecutor_LevelBuilder, ITexture*(std::string)> ::Rigister(mLuaInstance, "_GetTexture");
   }

   void LuaScriptExecutor_LevelBuilder::RunScript()
   {
      const auto folderManager = IO::FolderManager::GetInstance();

      const bool bScriptExecuted = mLuaInstance.ExecuteScript(folderManager->GetScriptPath() + mScriptName);

      assert((bScriptExecuted, "Lua script execution failure"));

      LuaFunction<void(void*)>::Call(mLuaInstance, "Create", (void*)this);

      //IMaterial* materialInstance = MaterialParser::ParseMaterialDescriptor(folderManager->GetMaterialPath() + "Pbs.m");
   }

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

   void LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<Actor*, Component*>& dataToAttachActorToComponent)
   {
      Actor* actor = std::get<0>(dataToAttachActorToComponent);
      Component* component = std::get<1>(dataToAttachActorToComponent);

      assert((actor && component, "Actor or component is null"));

      assert((mActiveComponents.count(component->GetObjectId()), "Sought component doesn't exist"));

      actor->AddComponent(mActiveComponents[component->GetObjectId()]);
   }

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

   ComponentData* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo*>& dirLightComponentData)
   {
      return ComponentCreator::CreateDirLightComponentData(std::get<0>(dirLightComponentData), std::get<1>(dirLightComponentData), std::get<2>(dirLightComponentData),
         std::get<3>(dirLightComponentData), std::get<4>(dirLightComponentData), std::get<5>(dirLightComponentData));
   }

   ProjectedShadowInfo* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<int32_t>& dirLightProjectionData)
   {
      const int32_t shadowAtlasSize = std::get<0>(dirLightProjectionData);
      const float orthoHalfExtent = GlobalSettings::GetInstance()->GetShadowOrthoProjectionHalfExtent();

      auto directionalLightTextureAtlasRequest = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(shadowAtlasSize, shadowAtlasSize));
      ProjectedShadowInfo* dirShadowProjInfo = new ProjectedDirShadowInfo(directionalLightTextureAtlasRequest, orthoHalfExtent);

      return dirShadowProjInfo;
   }

   ITexture* LuaScriptExecutor_LevelBuilder::ExecuteLuaCallback(const std::tuple<std::string>& getTextureResource)
   {
      const std::string& filePath = IO::FolderManager::GetInstance()->GetDirectoryRelativePathByFileName(std::get<0>(getTextureResource));
      std::shared_ptr<ITexture> resultTexture = TexturePool::GetInstance()->GetOrAllocateResource(filePath);
      return resultTexture.get();
   }
}
