#pragma once

#include "Core/GameCore/Level.h"
#include "Implementation/LevelProgressSystem/LevelProgressController.h"

#include <memory>

using namespace EngineCore;

namespace Game
{

   class LevelBase : public Level
   {
   protected:
      std::unique_ptr<LevelProgressController> mLvlProgressController;

   public:
      explicit LevelBase(const std::string &levelName);

      ~LevelBase() = default;
   };

}
