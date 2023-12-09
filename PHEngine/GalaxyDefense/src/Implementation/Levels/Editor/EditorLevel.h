#pragma once

#include "Implementation/Levels/LevelBase.h"
#include "Implementation/LevelEditor/LevelEditorController.h"

using namespace EngineCore;

namespace Game
{
   class CombatController;

   class EditorLevel : public LevelBase
   {
      using Base = LevelBase;

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
