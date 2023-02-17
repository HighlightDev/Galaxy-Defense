#include "Engine.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/ResourceManagerCore/Pool/FontMeshPool.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/ParticlesPool.h"
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/ResourceManagerCore/Pool/SoundBufferPool.h"
#include "Core/ResourceManagerCore/Pool/SoundMemoryChunkPool.h"
#include "Core/ResourceManagerCore/Pool/RuntimeGeneratedMeshPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GameCore/Event/EventDispatcher.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/AudioCore/SoundDevice.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/Event/PauseGameThreadEvent.h"
#include "Core/GameCore/Event/ExitGameThreadEvent.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

using namespace TinyLogger;
using namespace IO;
using namespace EngineUtility;
using namespace Resources;

namespace EngineCore
{
   Engine::Engine(InterThreadCommunicationMgr &interThreadMgr)
       : m_interThreadMgr(interThreadMgr),
         mInputManager(std::make_shared<InputManager>()),
         mActiveAudioOutputDevice(SoundDevice::GetInstance()),
         mRenderThreadDeltaTimeSeconds(),
         mGameThreadDeltaTimeSeconds()
   {
      PauseGameThreadEvent::GetInstance()->AddListener(this);
      ExitGameThreadEvent::GetInstance()->AddListener(this);

#if DEBUG
      m_echoTimer.SetIntervalMs(2000);
      m_echoTimer.SetIsRepeat(true);
      m_echoTimer.SetIsPausable(false);
      m_echoTimer.SetCallback([]()
                              { LogInfo("EchoTimer::Timeout => Time passed: 2 seconds"); });
      m_echoTimer.StartTimer();
#endif
   }

   Engine::~Engine()
   {
      PauseGameThreadEvent::GetInstance()->RemoveListener(this);
      ExitGameThreadEvent::GetInstance()->RemoveListener(this);
   }

   void Engine::CleanUp()
   {
      StopGameThreadExecution();
      StopLuaThreadExecution();
      m_gameThread.join();
      m_luaThread.join();

      CompositeShaderPool::GetInstance()->CleanUp();
      FontMeshPool::GetInstance()->CleanUp();
      MeshPool::GetInstance()->CleanUp();
      TexturePool::GetInstance()->CleanUp();
      ParticlesPool::GetInstance()->CleanUp();
      RenderTargetPool::GetInstance()->CleanUp();
      ShaderPool::GetInstance()->CleanUp();
      SimplePrimitivePool::GetInstance()->CleanUp();
      SoundBufferPool::GetInstance()->CleanUp();
      SoundMemoryChunkPool::GetInstance()->CleanUp();
      RuntimeGeneratedMeshPool::GetInstance()->CleanUp();

      mActiveAudioOutputDevice->CleanUp();
   }

   std::shared_ptr<InputManager> Engine::GetInputManager() const
   {
      return mInputManager;
   }

   void Engine::StopGameThreadExecution()
   {
      bGameThreadExecution.store(false);
   }

   void Engine::StopLuaThreadExecution()
   {
      bLuaThreadExecution.store(false);
   }

   void Engine::PlayLevel(std::shared_ptr<Level> level)
   {
      m_level = level;

      PreLevelInit();
      m_sceneRenderer = std::make_shared<DeferredShadingSceneRenderer>(m_interThreadMgr);
      m_interThreadMgr.SetSceneRendererWP(m_sceneRenderer);
      m_interThreadMgr.SetSceneWP(m_level->GetSceneWP());

      m_level->InitLevel();

      PostPhysicsInitialize();
      PostLevelInit();
      PostPlayLevelFinished();

      m_gameThread = std::thread(std::bind(&Engine::GameThreadPulse, this));
      m_luaThread = std::thread(std::bind(&Engine::LuaThreadPulse, this));
   }

   InterThreadCommunicationMgr &Engine::GetThreadCommunicationManager()
   {
      return m_interThreadMgr;
   }

   void Engine::PreLevelInit()
   {
      EventDispatcher::GetInstance()
          ->RegisterEventsByType<CameraTransformChangedEvent,
                                 PlayerMovedEvent,
                                 PhysicsComponentUpdatedEvent,
                                 KeyboardButtonDownEvent,
                                 KinematicBodyMovedEvent,
                                 TextureAtlasGeneratedEvent,
                                 MouseMovedEvent,
                                 MouseScrollEvent,
                                 MouseButtonDownEvent,
                                 PhysicsCollisionEvent,
                                 TextRegisterEvent,
                                 TextDataChangedEvent,
                                 PauseGameThreadEvent,
                                 ExitGameThreadEvent>();

      EngineConfigHolder::GetInstance()->LoadSettings(FolderManager::GetInstance()->GetConfigPath() + "engineConfig.cfg");

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

   void Engine::PostPlayLevelFinished()
   {
      m_level->PostPlayLevelFinished();
   }

   size_t rtCounter = 0;
   size_t gtCounter = 0;

   float sumRtFramesTime = 0.0f;
   float sumGtFramesTime = 0.0f;

   void Engine::ProcessEvent(const PauseGameThreadEvent::EventData_t &data)
   {
      bPauseGameThreadExecution.store(std::get<0>(data));
   }

   void Engine::ProcessEvent(const ExitGameThreadEvent::EventData_t &data)
   {
      bExitGame = true;
      StopGameThreadExecution();
      StopLuaThreadExecution();
   }

   void Engine::LuaThreadPulse()
   {
      using namespace std::chrono_literals;
      ThreadHelper::GetInstance()->RegisterThread("Lua");

      while (bLuaThreadExecution.load(std::memory_order::memory_order_seq_cst))
      {
         m_level->TickLua(0.0f);
         std::this_thread::sleep_for(1000ms);
      }
   }

   void Engine::GameThreadPulse()
   {
      ThreadHelper::GetInstance()->RegisterThread("Game");

      while (bGameThreadExecution.load(std::memory_order::memory_order_seq_cst))
      {
         /* GAME THREAD*/
         {
            const auto gtStartTimePoint = EngineTime::GetNowTime();

            /* Events: pre execution */
            ProcessEvents(Event::eExecutionOrder::PRE_EXECUTION);

            /* Work Jobs */
            m_interThreadMgr.SpinGameThreadJobs();

            if (!bPauseGameThreadExecution.load())
            {
               GameThreadTimersHolder::GetInstance()->Tick(mGameThreadDeltaTimeSeconds);
               m_level->Tick(mGameThreadDeltaTimeSeconds);
            }

            m_level->UnpausableTick(mGameThreadDeltaTimeSeconds);

            /* Events: post execution */
            ProcessEvents(Event::eExecutionOrder::POST_EXECUTION);
#ifdef DEBUG
            if (gtCounter == 1000)
            {
               gtCounter = 0;
               const float fps = 1000.0f / (float)sumGtFramesTime;
               m_level->SetGameThreadFPSTextValue(fps);
               sumGtFramesTime = 0.0f;
            }
            sumGtFramesTime += mGameThreadDeltaTimeSeconds;
            ++gtCounter;
#endif

            mGameThreadDeltaTimeSeconds = (float)EngineTime::GetSecondsFromDuration(
                EngineTime::GetPassedDuration(gtStartTimePoint));
         }
      }
   }

   void Engine::ProcessEvents(Event::eExecutionOrder order)
   {
      Event::EventDispatcher::GetInstance()->ProcessEvents(order);
   }

   void Engine::RenderThreadPulse()
   {
      /* RENDER THREAD */
      {
         const auto rtStartTimePoint = EngineTime::GetNowTime();
#ifdef DEBUG
         if (rtCounter == 100)
         {
            rtCounter = 0;
            const float fps = 100.0f / sumRtFramesTime;
            m_level->SetRenderThreadFPSTextValue(fps);
            sumRtFramesTime = 0.0f;
         }
         sumRtFramesTime += mRenderThreadDeltaTimeSeconds;
         ++rtCounter;
#endif
         m_interThreadMgr.SpinRenderThreadJobs();
         m_sceneRenderer->RenderScene_RenderThread();

         mRenderThreadDeltaTimeSeconds =
             (float)EngineTime::GetSecondsFromDuration(EngineTime::GetPassedDuration(rtStartTimePoint));
      }
   }

   void Engine::TickWindow()
   {
      RenderThreadPulse();
   }

   float Engine::GetRenderThreadDeltaTime() const
   {
      return mRenderThreadDeltaTimeSeconds;
   }

   float Engine::GetGameThreadDeltaTime() const
   {
      return mGameThreadDeltaTimeSeconds;
   }

   bool Engine::IsExitGameState() const
   {
      return bExitGame;
   }

#if DEBUG

   void Engine::RecompileAllShaders()
   {
      Resources::ShaderPool::GetInstance()->RecompileShaders();
      Resources::CompositeShaderPool::GetInstance()->RecompileShaders();
   }

#endif
}