#pragma once

#include "Core/GameCore/Level.h"

#include <tuple>

using namespace Game;

namespace Labyrinth
{

   class ILuaLevelExecutor
   {
   public:
      virtual void operator()(const std::tuple<double>& parameters) = 0;
      virtual void operator()(const std::tuple<double, double>& parameters) = 0;
      virtual void operator()(const std::tuple<float>& parameters) = 0;
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
        
      virtual void operator()(const std::tuple<double>& parameters) override;

      virtual void operator()(const std::tuple<double, double>& parameters) override;
      virtual void operator()(const std::tuple<float>& parameters) override;

      void TestLua();
   };

}

