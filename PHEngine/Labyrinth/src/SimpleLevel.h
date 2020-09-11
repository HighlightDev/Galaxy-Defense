#pragma once

#include "Core/GameCore/Level.h"
#include "Core/GameCore/ScriptingCore/LuaCore.h"

#include <tuple>

using namespace Game;

namespace Labyrinth
{

   class ILuaLevelExecutor
   {
   public:
      virtual void operator()(const std::tuple<LuaTable<std::string, int, float, double>>& parameters) = 0;
   };

   class SimpleLevel :
      public Level, public ILuaLevelExecutor
   {
      using Base = Level;

   public:

      SimpleLevel(InterThreadCommunicationMgr& threadMgr);

      virtual ~SimpleLevel();

      virtual void LoadLevel() override;

      virtual void PreConstructorInitialize();

      virtual void PostConstructorInitialize();
        
      virtual void operator()(const std::tuple<LuaTable<std::string, int, float, double>>& parameters) override;

      void TestLua();
   };

}

