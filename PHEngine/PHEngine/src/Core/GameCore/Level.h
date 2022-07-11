#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/InterThreadCommunicationMgr.h"

using namespace Thread;

namespace EngineCore
{

   class Level
   {
   protected:

      std::shared_ptr<Scene> mScene;

   public:

      Level(InterThreadCommunicationMgr& interThreadMgr);

      virtual ~Level();

      void PostPhysicsInitialize();


      std::weak_ptr<Scene> GetSceneWP() const;

      virtual void PreLevelInit();
      
      virtual void InitLevel();

      virtual void PostLevelInit();

      virtual void PostPlayLevelFinished();

      virtual void TickLevel(const float deltaTime);

      void SerializeLevel(const std::string& pathToFolder);

      void DeserializeLevel(const std::string& pathToFile);

   private:

      void InstantiateLevelFromSerializedContainer(struct SerializeDataContainer& container);

      void CollectAllocatedResourcesForSerialization(struct SerializeDataContainer& container);
   };
}

