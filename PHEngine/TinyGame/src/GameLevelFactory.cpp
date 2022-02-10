#include "GameLevelFactory.h"

#include "Core/InterThreadCommunicationMgr.h"
#include "IntroLevel.h"

namespace Labyrinth
{

   GameLevelFactory* GameLevelFactory::mLevelFactoryInstance = nullptr;

   std::shared_ptr<Level> GameLevelFactory::CreateLevel(const std::string& levelName, Thread::InterThreadCommunicationMgr& threadMgr) const
   {
      return std::make_shared<IntroLevel>(threadMgr);
   }

   GameLevelFactory* GameLevelFactory::GetInstance() 
   {
      if (!mLevelFactoryInstance)
         mLevelFactoryInstance = new GameLevelFactory();

      return mLevelFactoryInstance;
   }

}