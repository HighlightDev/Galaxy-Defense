#include "GameLevelFactory.h"

#include "Implementation/Levels/CombatLevel/CombatLevel.h"
#include "Implementation/Levels/MainMenu/MainMenuLevel.h"
#include "Implementation/Levels/Editor/EditorLevel.h"

namespace Game
{
   std::shared_ptr<Level> GameLevelFactory::CreateLevel(const std::string &levelName) const
   {
      if ("MainMenuLevel" == levelName)
      {
         return std::make_shared<MainMenuLevel>();
      }
      else if ("CombatLevel" == levelName)
      {
         return std::make_shared<CombatLevel>();
      }
      else if ("EditorLevel" == levelName)
      {
         return std::make_shared<EditorLevel>();
      }
      return nullptr;
   }

}