#include "GameLevelFactory.h"

#include "IntroLevel.h"
#include "MainMenuLevel.h"

namespace Game
{
   std::shared_ptr<Level> GameLevelFactory::CreateLevel(const std::string &levelName) const
   {
      if ("MainMenuLevel" == levelName)
      {
         return std::make_shared<MainMenuLevel>();
      }
      else if ("FirstLevel" == levelName)
      {
         return std::make_shared<IntroLevel>();
      }
      return nullptr;
   }

}