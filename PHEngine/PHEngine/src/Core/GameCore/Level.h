#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/InterThreadCommunicationMgr.h"

using namespace Thread;

namespace Game
{

   class Level
   {

   protected:

      Scene* mScene;

   public:

      Level(InterThreadCommunicationMgr& interThreadMgr);

      virtual ~Level();

      void PostConstructorInitialize();

      void PostPhysicsInitialize();

      void InitLevel();

      virtual void TickLevel(const float deltaTime);

      virtual void LoadLevel();

      const std::vector<std::shared_ptr<Actor>>& GetActors() const;

      const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& GetSceneProxies() const;

      const std::vector<std::shared_ptr<LightSceneProxy>>& GetLightProxies() const;

      const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& GetShadowGroupPrimitives() const;

      ICamera* GetCamera() const;

      bool ReadAreProxiesUpdated(bool newValue);

      // TODO: this is a temporary solution
      void CameraMove();

      void CameraRotate();
   };
}

