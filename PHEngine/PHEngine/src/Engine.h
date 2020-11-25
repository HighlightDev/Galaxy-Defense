#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Level.h"
#include "Core/GameCore/LevelFactory.h"

#include <thread>
#include <chrono>

using namespace Game;
using namespace Graphics::Renderer;

class Engine
{

private:

   using Clock_t = std::chrono::high_resolution_clock;

private:

   static constexpr double InvLimitFPS = 1.0 / 60.0;

   InterThreadCommunicationMgr& m_interThreadMgr;

   std::shared_ptr<Level> m_level;

   std::shared_ptr<DeferredShadingSceneRenderer> m_sceneRenderer;

   bool bGameThreadExecution = true;

private:
   std::thread m_gameThread;

   typename Clock_t::time_point mLastRenderThreadPulseTime;
   double mRenderThreadDeltaTimeSeconds;

   typename Clock_t::time_point mLastGameThreadPulseTime;
   double mGameThreadDeltaTimeSeconds;
   double mGameThreadSumDeltaTimeSec;

public:

	Engine(InterThreadCommunicationMgr& interThreadMgr);

	~Engine();

   void StopExecution();

   void PlayLevel(std::shared_ptr<Level> level);

   void PreLevelInit();

   void PostLevelInit();

   void PostPhysicsInitialize();

   void ProcessEvents(Event::ExecutionOrder order);

   void GameThreadPulse();

   void RenderThreadPulse();

	void TickWindow();

	void MouseMove();

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

