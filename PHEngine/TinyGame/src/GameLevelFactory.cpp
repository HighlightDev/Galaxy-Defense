#include "GameLevelFactory.h"

#include "IntroLevel.h"

namespace Game
{
   std::shared_ptr<Level> GameLevelFactory::CreateLevel(const std::string& levelName) const
   {
      return std::make_shared<IntroLevel>();
   }

   GameLevelFactory* GameLevelFactory::GetInstance() 
   {
      static GameLevelFactory instance;

      return &instance;
   }

}