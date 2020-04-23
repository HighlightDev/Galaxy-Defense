#pragma once

#include <memory>
#include <string>

#include "Core/InterThreadCommunicationMgr.h"
#include "Engine.h"

namespace Game
{

   class Level;

   class LevelFactory
   {
   public:

      LevelFactory() = default;

      virtual std::shared_ptr<Level> CreateLevel(const std::string& levelName, Thread::InterThreadCommunicationMgr& threadMgr, Engine* engine) const = 0;
   };

}

