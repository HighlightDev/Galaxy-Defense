#pragma once

#include "Implementation/Levels/LevelBase.h"
#include "Implementation/Controllers/GameFlowController.h"
#include "Implementation/Levels/CombatLevel/Controllers/UiController.h"
#include "Implementation/Levels/LevelData.h"

using namespace EngineCore;

namespace Game
{
   class CombatController;

   class CombatLevel : public LevelBase
   {
      using Base = LevelBase;

      std::shared_ptr<CombatController> mCombatController;

      std::shared_ptr<GameFlowController> mGameFlowController;

      std::unique_ptr<UiController> mUiController;

   public:
      CombatLevel();

      ~CombatLevel() override;

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

      LevelData LoadLevelDataFromFile(const std::string& levelName) const;
   };

}
