#pragma once

#include "Core/GameCore/Level.h"

using namespace Game;

namespace Labyrinth
{

   class ILuaLevelExecutor
   {
   public:
      virtual void operator()(double value) = 0;
      virtual void operator()(double, double) = 0;
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
        
      virtual void operator()(double value) override;

      virtual void operator()(double, double) override;

      void TestLua();
   };

}

