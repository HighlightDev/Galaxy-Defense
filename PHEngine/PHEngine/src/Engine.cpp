#include "Engine.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/GameCore/Event/EventDispatcher.h"

#include <TinyLogger/LogInterface.h>

Engine::Engine(InterThreadCommunicationMgr &interThreadMgr)
    : m_interThreadMgr(interThreadMgr), mInputManager(std::make_shared<InputManager>()), mLastRenderThreadPulseTime(EngineTime::GetCurrentTime()), mRenderThreadDeltaTimeSeconds(), mLastGameThreadPulseTime(EngineTime::GetCurrentTime()), mGameThreadDeltaTimeSeconds(), mGameThreadSumDeltaTimeSec()
{
}

Engine::~Engine()
{
   m_gameThread.join();
}

std::shared_ptr<InputManager> Engine::GetInputManager() const
{
   return mInputManager;
}

void Engine::StopGameThreadExecution()
{
   bGameThreadExecution.store(false);
}

void Engine::PlayLevel(std::shared_ptr<Level> level)
{
   m_level = level;
   m_sceneRenderer = std::make_shared<DeferredShadingSceneRenderer>(m_interThreadMgr);
   m_interThreadMgr.SetSceneRendererWP(m_sceneRenderer);

   PreLevelInit();

   m_level->InitLevel();
   m_interThreadMgr.SetSceneWP(m_level->GetSceneWP());

   PostLevelInit();

   PostPhysicsInitialize();

   m_gameThread = std::thread(std::bind(&Engine::GameThreadPulse, this));
}

InterThreadCommunicationMgr &Engine::GetThreadCommunicationManager()
{
   return m_interThreadMgr;
}

void Engine::PreLevelInit()
{
   EventDispatcher::GetInstance()->RegisterEventsByType<CameraTransformChangedEvent, PlayerMovedEvent, PhysicsSimulationUpdatedEvent, KeyboardButtonDownEvent, KinematicBodyMovedEvent, TextureAtlasGeneratedEvent, MouseMovedEvent, MouseScrollEvent>();

   m_level->PreLevelInit();
}

void Engine::PostLevelInit()
{
   m_level->PostLevelInit();
   m_sceneRenderer->PostLevelInit();
}

void Engine::PostPhysicsInitialize()
{
   m_level->PostPhysicsInitialize();
}

void Engine::GameThreadPulse()
{
   while (bGameThreadExecution.load(std::memory_order::memory_order_seq_cst))
   {
      const uint64_t memoryBeforeExe = getProcessMemorySize();

      /* GAME THREAD*/
      {
         mGameThreadDeltaTimeSeconds = GetGameThreadDeltaSeconds();
         mGameThreadSumDeltaTimeSec += mGameThreadDeltaTimeSeconds;

         /* Events: pre execution */
         ProcessEvents(Event::ExecutionOrder::PRE_EXECUTION);

         /* Work Jobs */
         m_interThreadMgr.SpinGameThreadJobs();

         if (mGameThreadSumDeltaTimeSec >= InvLimitFPS) // 1 / 60 of a second
         {
            // This should be executed on game thread
            m_level->TickLevel(static_cast<float>(mGameThreadDeltaTimeSeconds));
            mLastGameThreadPulseTime = EngineTime::GetCurrentTime();
            mGameThreadSumDeltaTimeSec = 0.0;
         }

         /* Events: post execution */
         ProcessEvents(Event::ExecutionOrder::POST_EXECUTION);

         /*const uint64_t memoryAfterExe = memoryBeforeExe - getProcessMemorySize();

         if (memoryAfterExe > 0)
         {
            TinyLogger::LogProxy::LogMessages("Engine::GameThread execution. Memory consumption : ", (uint64_t)memoryAfterExe);
         }*/
      }
   }
}

void Engine::ProcessEvents(Event::ExecutionOrder order)
{
   Event::EventDispatcher::GetInstance()->ProcessEvents(order);
}

void Engine::RenderThreadPulse()
{
   /* RENDER THREAD */
   {
      // mRenderThreadDeltaTimeSeconds = GetRenderThreadDeltaSeconds();
      m_interThreadMgr.SpinRenderThreadJobs();
      m_sceneRenderer->RenderScene_RenderThread();
      // mLastRenderThreadPulseTime = EngineTime::GetCurrentTime();
   }
}

void Engine::TickWindow()
{
   RenderThreadPulse();
}

double Engine::GetRenderThreadDeltaSeconds() const
{
   return EngineTime::GetSecondsFromDuration(EngineTime::GetPassedDuration(mLastRenderThreadPulseTime));
}

double Engine::GetGameThreadDeltaSeconds() const
{
   return EngineTime::GetSecondsFromDuration(EngineTime::GetPassedDuration(mLastGameThreadPulseTime));
}

double Engine::GetRenderThreadDeltaTime() const
{
   return mRenderThreadDeltaTimeSeconds;
}

double Engine::GetGameThreadDeltaTime() const
{
   return mGameThreadDeltaTimeSeconds;
}

#if DEBUG

void Engine::PushFrame()
{
   m_sceneRenderer->PushRenderTargetToTextureRenderer();
}

void Engine::RecompileAllShaders()
{
   Resources::ShaderPool::GetInstance()->RecompileShaders();
   Resources::CompositeShaderPool::GetInstance()->RecompileShaders();
}

#endif