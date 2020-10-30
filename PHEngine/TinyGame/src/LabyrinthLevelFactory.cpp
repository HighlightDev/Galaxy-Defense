#include "LabyrinthLevelFactory.h"

#include "Core/InterThreadCommunicationMgr.h"
#include "SimpleLevel.h"

namespace Labyrinth
{

   LabyrinthLevelFactory* LabyrinthLevelFactory::mLevelFactoryInstance = nullptr;

   std::shared_ptr<Level> LabyrinthLevelFactory::CreateLevel(const std::string& levelName, Thread::InterThreadCommunicationMgr& threadMgr) const
   {
      return std::make_shared<SimpleLevel>(threadMgr);
   }

   LabyrinthLevelFactory* LabyrinthLevelFactory::GetInstance() 
   {
      if (!mLevelFactoryInstance)
         mLevelFactoryInstance = new LabyrinthLevelFactory();

      return mLevelFactoryInstance;
   }

}