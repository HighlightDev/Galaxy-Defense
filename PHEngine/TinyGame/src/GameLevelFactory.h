#pragma once

#include "Core/GameCore/LevelFactory.h"

using namespace EngineCore;

namespace Game
{

   class GameLevelFactory
      : public LevelFactory
   {

      static GameLevelFactory* mLevelFactoryInstance;

   public:
      GameLevelFactory() = default;

      virtual std::shared_ptr<Level> CreateLevel(const std::string& levelName, Thread::InterThreadCommunicationMgr& threadMgr) const override;

      static GameLevelFactory* GetInstance();
   };

}

