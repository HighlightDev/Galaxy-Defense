#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Components/UiComponents/UiComponent.h"
#include "Core/InterThreadCommunicationMgr.h"
#include "Core/GameCore/GUI/HudText/HudTextField.h"

using namespace Thread;

namespace EngineCore
{
   class Level
       : public ITickable
   {
#ifdef DEBUG
      std::shared_ptr<Actor> mDebugDummyActor;
      std::weak_ptr<HudTextField> mRtTextField;
      std::weak_ptr<HudTextField> mGtTextField;
#endif

   protected:
      std::shared_ptr<Scene> mScene;

   public:
      Level(InterThreadCommunicationMgr &interThreadMgr);

      virtual ~Level();

      void PostPhysicsInitialize();

      std::weak_ptr<Scene> GetSceneWP() const;

      virtual void PreLevelInit();

      virtual void InitLevel();

      virtual void PostLevelInit();

      virtual void PostPlayLevelFinished();

      void Tick(const float deltaTime) override final;

      void UnpausableTick(const float deltaTime) override final;

      void TickLua(const float deltaTime);

      void SerializeLevel(const std::string &pathToFolder);

      void DeserializeLevel(const std::string &pathToFile);

#ifdef DEBUG

      void SetRenderThreadFPSTextValue(const float fps);

      void SetGameThreadFPSTextValue(const float fps);

#endif

   private:
      void InstantiateLevelFromSerializedContainer(struct SerializeDataContainer &container);

      void CollectAllocatedResourcesForSerialization(struct SerializeDataContainer &container);
   };
}
