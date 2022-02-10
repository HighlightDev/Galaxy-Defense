#include "SimpleLevel.h"
#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaScriptExecutor_EngineObjectsCreator.h"
#include "Core/GameCore/ScriptingCore/EngineObjectCreator.h"

using namespace Graphics;
using namespace EnginePhysics;
using namespace IO;

namespace Labyrinth
{

   SimpleLevel::SimpleLevel(InterThreadCommunicationMgr &threadMgr)
       : Level(threadMgr)
   {
   }

   SimpleLevel::~SimpleLevel()
   {
   }

   void SimpleLevel::RunLuaBuildLevelScript()
   {
      static constexpr const char *lvlName
           = "createTestLevel.lua";
          //= "spaceLvl1.lua";
      LuaScriptExecutor_EngineObjectsCreator mLuaLevelBuilder = LuaScriptExecutor_EngineObjectsCreator(lvlName);
      mLuaLevelBuilder.PostInit(mScene);
      mLuaLevelBuilder.RegisterCallbacks();
      mLuaLevelBuilder.RunScript();
   }

   void SimpleLevel::PreLevelInit()
   {
      Base::PreLevelInit();
   }

   void SimpleLevel::PostLevelInit()
   {
      Base::PostLevelInit();

      /*const auto groundActor = mScene->GetActor("Ground");
      const auto pointLightComponents = mScene->GetActor("MainLightActor")->GetComponentsByType<PointLightComponent>();
      const auto plShadowTexAtlasRequest = pointLightComponents[0]->GetRenderData().ShadowInfo->GetTextureAtlasSpaceRequest();

      const CubemapComponentData cubemapComponentData("CubemapComponent", glm::vec3(10, 2, 10), glm::vec3(), glm::vec3(2),
                                                      FolderManager::GetInstance()->GetShadersPath() + "cubemapRendererVS.glsl", FolderManager::GetInstance()->GetShadersPath() + "cubemapRendererFS.glsl", plShadowTexAtlasRequest);
      const auto cubemapRendererComponent = mScene->CreateComponent_GameThread<CubemapComponent, Game::ComponentMetaType::Cubemap>(cubemapComponentData);
      groundActor->AddComponent(cubemapRendererComponent);*/
   }

   void SimpleLevel::LoadLevel()
   {
      // ResourceMap::GetInstance()->WaitUntilResourcesLoad();

#if 0
      DeserializeLevel("test_serialize.xml");
#else
      RunLuaBuildLevelScript();
#endif
   }
#undef GET_REL_PATH_TO_FILE
}
