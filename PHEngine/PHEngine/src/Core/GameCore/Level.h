#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/InterThreadCommunicationMgr.h"

using namespace Thread;

namespace Game
{

   class Level
   {

   protected:

      std::shared_ptr<Scene> mScene;

   public:

      Level(InterThreadCommunicationMgr& interThreadMgr);

      virtual ~Level();

      void PostPhysicsInitialize();

      void InitLevel();

      std::shared_ptr<ACamera> GetCamera() const;

      std::weak_ptr<Scene> GetSceneWP() const;

      // TODO: this is a temporary solution
      void CameraMove();

      void CameraRotate();

      virtual void PreLevelInit();

      virtual void PostLevelInit();

      virtual void TickLevel(const float deltaTime);

      virtual void LoadLevel();

      void SerializeLevel(const std::string& pathToFolder);

      void DeserializeLevel(const std::string& pathToFile);

   private:

      void InstantiateLevelFromSerializedContainer(struct SerializeDataContainer& container);
   };
}

