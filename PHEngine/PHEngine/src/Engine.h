#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Level.h"
#include "Core/GameCore/LevelFactory.h"
#include "Core/GameCore/Input/InputManager.h"
#include "Core/CommonCore/TimeHelper.h"
#include "Core/GameCore/Event/PauseGameThreadEvent.h"
#include "Core/GameCore/Event/ExitGameThreadEvent.h"
#include "Core/CommonCore/Timer.h"

#include <thread>
#include <chrono>
#include <atomic>

using namespace EngineCore;
using namespace Graphics::Renderer;

namespace EngineCore
{
   class SoundDevice;

   class Engine
       : public Event::PauseGameThreadEvent,
         public Event::ExitGameThreadEvent
   {
      InterThreadCommunicationMgr &m_interThreadMgr;

      std::shared_ptr<InputManager> mInputManager;

      std::shared_ptr<Level> m_level;

      std::shared_ptr<DeferredShadingSceneRenderer> m_sceneRenderer;

      std::atomic_bool bGameThreadExecution{true};

      std::shared_ptr<SoundDevice> mActiveAudioOutputDevice;

      std::thread m_gameThread;

      float mRenderThreadDeltaTimeSeconds;

      float mGameThreadDeltaTimeSeconds;

      std::atomic_bool bPauseGameThreadExecution{false};

      bool bExitGame{false};

#if DEBUG
      GameThreadTimer m_echoTimer;
#endif

   public:
      Engine(InterThreadCommunicationMgr &interThreadMgr);

      ~Engine();

      void PlayLevel(std::shared_ptr<Level> level);

      void PreLevelInit();

      void PostLevelInit();

      void PostPhysicsInitialize();

      void PostPlayLevelFinished();

      void ProcessEvents(Event::eExecutionOrder order);

      void ProcessEvent(const PauseGameThreadEvent::EventData_t &data) override;

      void ProcessEvent(const ExitGameThreadEvent::EventData_t &data) override;

      void GameThreadPulse();

      void RenderThreadPulse();

      void TickWindow();

      std::shared_ptr<InputManager> GetInputManager() const;

      float GetRenderThreadDeltaTime() const;

      float GetGameThreadDeltaTime() const;

      InterThreadCommunicationMgr &GetThreadCommunicationManager();

      bool IsExitGameState() const;

#if DEBUG

      void RecompileAllShaders();

#endif

      void CleanUp();

   private:
      void StopGameThreadExecution();
   };
}
