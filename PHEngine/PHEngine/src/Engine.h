#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Level.h"
#include "Core/GameCore/LevelFactory.h"
#include "Core/GameCore/Input/InputManager.h"
#include "Core/CommonCore/TimeHelper.h"

#include <thread>
#include <chrono>
#include <atomic>

using namespace EngineCore;
using namespace Graphics::Renderer;

class Engine
{

   static constexpr double InvLimitFPS = 1.0 / 60.0;

   InterThreadCommunicationMgr& m_interThreadMgr;

   std::shared_ptr<InputManager> mInputManager;

   std::shared_ptr<Level> m_level;

   std::shared_ptr<DeferredShadingSceneRenderer> m_sceneRenderer;

   std::atomic_bool bGameThreadExecution = true;

private:
   std::thread m_gameThread;

   Moment_t mLastRenderThreadPulseTime;
   double mRenderThreadDeltaTimeSeconds;

   Moment_t mLastGameThreadPulseTime;
   double mGameThreadDeltaTimeSeconds;
   double mGameThreadSumDeltaTimeSec;

public:

	Engine(InterThreadCommunicationMgr& interThreadMgr);

	~Engine();

   void StopGameThreadExecution();

   void PlayLevel(std::shared_ptr<Level> level);

   void PreLevelInit();

   void PostLevelInit();

   void PostPhysicsInitialize();

   void PostPlayLevelFinished();

   void ProcessEvents(Event::eExecutionOrder order);

   void GameThreadPulse();

   void RenderThreadPulse();

	void TickWindow();

   std::shared_ptr<InputManager> GetInputManager() const;

   double GetRenderThreadDeltaTime() const;

   double GetGameThreadDeltaTime() const;

   InterThreadCommunicationMgr& GetThreadCommunicationManager();

#if DEBUG

   void PushFrame();

   void RecompileAllShaders();

#endif

private:

   double GetRenderThreadDeltaSeconds() const;

   double GetGameThreadDeltaSeconds() const;

};

