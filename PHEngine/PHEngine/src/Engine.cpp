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
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GameCore/Event/EventDispatcher.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/AudioCore/SoundDevice.h"
#include "Core/GameCore/LoggerExtension.h"

#include <TinyLogger/LogInterface.h>

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
   }

   Engine::~Engine()
   {
   }

   void Engine::CleanUp()
   {
      StopGameThreadExecution();
      m_gameThread.join();

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

   void Engine::PlayLevel(std::shared_ptr<Level> level)
   {
      m_level = level;

      PreLevelInit();
      m_sceneRenderer = std::make_shared<DeferredShadingSceneRenderer>(m_interThreadMgr);
      m_interThreadMgr.SetSceneRendererWP(m_sceneRenderer);

      m_level->InitLevel();
      m_interThreadMgr.SetSceneWP(m_level->GetSceneWP());

      PostPhysicsInitialize();
      PostLevelInit();
      PostPlayLevelFinished();

      m_gameThread = std::thread(std::bind(&Engine::GameThreadPulse, this));
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
                                 PhysicsCollisionOccuredEvent,
                                 TextRegisterEvent,
                                 TextDataChangedEvent>();

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

            // This should be executed on game thread
            m_level->TickLevel(mGameThreadDeltaTimeSeconds);

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

            /* Events: post execution */
            ProcessEvents(Event::eExecutionOrder::POST_EXECUTION);

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
}