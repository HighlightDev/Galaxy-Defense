#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ITickable.h"

#include <string>

using namespace Thread;

namespace EngineCore
{
   class Level
   : public ITickable 
   {
   protected:
      std::weak_ptr<Scene> mSceneWp;

      std::string mLevelName;

   public:
      Level(const std::string& levelName);

      virtual ~Level();

      std::string GetLevelName() const;

      void SetScene(const std::shared_ptr<Scene>& scene);

      void PostPhysicsInitialize();

      std::weak_ptr<Scene> GetSceneWP() const;

      virtual void PreLevelInit();

      virtual void InitLevel();

      virtual void PostLevelInit();

      virtual void PostPlayLevelFinished();

      void SerializeLevel(const std::string &pathToFolder);

      void DeserializeLevel(const std::string &pathToFile);

      virtual void UnloadLevel() = 0;

      void Tick(const float deltaTime) override;

      void UnpausableTick(const float deltaTime) override;

   private:
      void InstantiateLevelFromSerializedContainer(struct SerializeDataContainer &container);

      void CollectAllocatedResourcesForSerialization(struct SerializeDataContainer &container);
   };
}
