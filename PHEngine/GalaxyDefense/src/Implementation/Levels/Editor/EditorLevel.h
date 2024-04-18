#pragma once

#include "Implementation/Levels/LevelBase.h"
#include "Implementation/Levels/Editor/Controllers/LevelEditorController.h"
#include "Implementation/Levels/Editor/Controllers/LevelEditorUiController.h"

using namespace EngineCore;

namespace Game
{
   class EditorLevel : public LevelBase
   {
      using Base = LevelBase;

      std::unique_ptr<LevelEditorUiController> mUiController;

      std::shared_ptr<LevelEditorController> mLevelEditorController;

   public:
      EditorLevel();

      ~EditorLevel() override;

      void InitLevel() override;

      void PreLevelInit() override;

      void PostLevelInit() override;

      void PostPlayLevelFinished() override;

      void RunLuaBuildLevelScript();

      void UnloadLevel() override;

      void Tick(const float deltaTime) override;

      void UnpausableTick(const float deltaTime) override;

   private:
      void CreateScene();
   };

}
