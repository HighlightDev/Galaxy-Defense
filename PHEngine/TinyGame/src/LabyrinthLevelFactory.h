#pragma once

#include "Core/GameCore/LevelFactory.h"

using namespace Game;

namespace Labyrinth
{

   class LabyrinthLevelFactory
      : public LevelFactory
   {

      static LabyrinthLevelFactory* mLevelFactoryInstance;

   public:
      LabyrinthLevelFactory() = default;

      virtual std::shared_ptr<Level> CreateLevel(const std::string& levelName, Thread::InterThreadCommunicationMgr& threadMgr) const override;

      static LabyrinthLevelFactory* GetInstance();
   };

}

