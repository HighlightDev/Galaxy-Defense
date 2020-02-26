#include "Engine.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"

Engine::Engine()
   : m_interThreadMgr()
   , mLastRenderThreadPulseTime(Clock_t::now())
   , mRenderThreadDeltaTimeSeconds()
   , mLastGameThreadPulseTime(Clock_t::now())
   , mGameThreadDeltaTimeSeconds()
   , mGameThreadSumDeltaTimeSec()
{
}

Engine::~Engine()
{
}

void Engine::PlayLevel(std::shared_ptr<Level> level)
{
   m_level = level;
   m_sceneRenderer = std::make_unique<DeferredShadingSceneRenderer>(m_interThreadMgr, m_level);

   m_level->InitLevel();

   PostConstructorInitialize();

   m_gameThread = std::thread(std::bind(&Engine::GameThreadPulse, this));
   m_gameThread.detach();
}

InterThreadCommunicationMgr& Engine::GetThreadCommunicationManager()
{
   return m_interThreadMgr;
}

void Engine::PostConstructorInitialize()
{
   m_level->PostConstructorInitialize();
   m_sceneRenderer->PostConstructorInitialize();
}

void Engine::GameThreadPulse()
{
   while (true)
   {
      /* GAME THREAD*/
      {
         mGameThreadDeltaTimeSeconds = GetGameThreadDeltaSeconds();
         mGameThreadSumDeltaTimeSec += mGameThreadDeltaTimeSeconds;

         SPIN_GAME_THREAD_JOBS(m_interThreadMgr);
         if (mGameThreadSumDeltaTimeSec >= InvLimitFPS) // 1 / 60 of a second
         {
            // This should be executed on game thread
            m_level->TickLevel(static_cast<float>(mGameThreadDeltaTimeSeconds));
            mLastGameThreadPulseTime = Clock_t::now();
            mGameThreadSumDeltaTimeSec = 0.0;
         }
      }
   }
}

void Engine::RenderThreadPulse()
{
   /* RENDER THREAD */
   {
      mRenderThreadDeltaTimeSeconds = GetRenderThreadDeltaSeconds();
      
      // This should be executed on render thread
      SPIN_RENDER_THREAD_JOBS(m_interThreadMgr);
      m_sceneRenderer->RenderScene_RenderThread();
      mLastRenderThreadPulseTime = Clock_t::now();
   }

}

void Engine::TickWindow()
{
   RenderThreadPulse();
}

void Engine::MouseMove()
{
   m_level->CameraRotate();
}

void Engine::KeyDown()
{
   m_level->CameraMove();
}

double Engine::GetRenderThreadDeltaSeconds() const
{
   Clock_t::duration deltaTime = Clock_t::now() - mLastRenderThreadPulseTime;
   const double invFromNanoToSec = 0.000000001;
   return static_cast<double>(deltaTime.count()) * invFromNanoToSec;
}

double Engine::GetGameThreadDeltaSeconds() const
{
   Clock_t::duration deltaTime = Clock_t::now() - mLastGameThreadPulseTime;
   const double invFromNanoToSec = 0.000000001;
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
}

#endif