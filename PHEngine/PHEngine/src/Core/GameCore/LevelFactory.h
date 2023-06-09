#pragma once

#include <memory>
#include <string>

namespace EngineCore
{

   class Level;

   class LevelFactory
   {
   public:

      LevelFactory() = default;

      virtual std::shared_ptr<Level> CreateLevel(const std::string& levelName) const = 0;
   };

}

