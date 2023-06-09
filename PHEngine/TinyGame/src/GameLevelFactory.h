#pragma once

#include "Core/GameCore/LevelFactory.h"

using namespace EngineCore;

namespace Game
{

   class GameLevelFactory
      : public LevelFactory
   {
   public:
      GameLevelFactory() = default;

      std::shared_ptr<Level> CreateLevel(const std::string& levelName) const override;

      static GameLevelFactory* GetInstance();
   };

}

