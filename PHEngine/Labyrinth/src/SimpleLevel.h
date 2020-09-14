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
      //virtual void operator()(const std::tuple<float>& parameters) = 0;
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
        
      class SceneComponent* operator()(const std::tuple<>& parameters);

      void operator()(const std::tuple<SceneComponent*>& parameters);

      void TestLua();
   };

}

