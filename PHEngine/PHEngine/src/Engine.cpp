#include "Engine.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/GameCore/Event/EventDispatcher.h"

#include <TinyLogger/LogInterface.h>

Engine::Engine(InterThreadCommunicationMgr& interThreadMgr)
   : m_interThreadMgr(interThreadMgr)
   , mLastRenderThreadPulseTime(Clock_t::now())
   , mRenderThreadDeltaTimeSeconds()
   , mLastGameThreadPulseTime(Clock_t::now())
   , mGameThreadDeltaTimeSeconds()
   , mGameThreadSumDeltaTimeSec()
   , mInputManager(std::make_shared<InputManager>())
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

InterThreadCommunicationMgr& Engine::GetThreadCommunicationManager()
{
   return m_interThreadMgr;
}

void Engine::PreLevelInit()
{
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
            mLastGameThreadPulseTime = Clock_t::now();
            mGameThreadSumDeltaTimeSec = 0.0;
         }

         /* Events: post execution */
         ProcessEvents(Event::ExecutionOrder::POST_EXECUTION);

         const uint64_t memoryAfterExe = memoryBeforeExe - getProcessMemorySize();

         if (memoryAfterExe > 0)
         {
            TinyLogger::LogProxy::LogMessages("Engine::GameThread execution. Memory consumption : ", (uint64_t)memoryAfterExe);
         }
      }
   }
}

void Engine::ProcessEvents(Event::ExecutionOrder order)
{
   Event::EngineEventDispatcher::ProcessEvents(order);
}

#include <iostream>

void Engine::RenderThreadPulse()
{
   /* RENDER THREAD */
   {
      mRenderThreadDeltaTimeSeconds = GetRenderThreadDeltaSeconds();
      //std::cout << "spin RT spent sec="<<  mRenderThreadDeltaTimeSeconds << std::endl;
      
      // This should be executed on render thread
      m_interThreadMgr.SpinRenderThreadJobs();
      mLastRenderThreadPulseTime = Clock_t::now();

      m_sceneRenderer->RenderScene_RenderThread();
   }
}

void Engine::TickWindow()
{
   RenderThreadPulse();
}

double Engine::GetRenderThreadDeltaSeconds() const
{
   Clock_t::duration deltaTime = Clock_t::now() - mLastRenderThreadPulseTime;
   static constexpr double invFromNanoToSec = 0.000000001;
   return static_cast<double>(deltaTime.count()) * invFromNanoToSec;
}

double Engine::GetGameThreadDeltaSeconds() const
{
   Clock_t::duration deltaTime = Clock_t::now() - mLastGameThreadPulseTime;
   static constexpr double invFromNanoToSec = 0.000000001;
   return static_cast<double>(deltaTime.count()) * invFromNanoToSec;
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