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
#include "Core/IoCore/FolderManager.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"
#include "Core/AudioCore/SoundDevice.h"
#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GameCore/Event/GameThreadEventDispatcher.h"
#include "Core/GameCore/Event/LuaThreadEventDispatcher.h"
#include "Core/GameCore/Event/WindowSizeChangedEvent.h"
#include "Core/GameCore/Event/BroadcastEvent.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/GameCore/Event/CameraTransformChangedEvent.h"
#include "Core/GameCore/Event/PhysicsComponentUpdatedEvent.h"
#include "Core/GameCore/Event/KeyboardInputEvent.h"
#include "Core/GameCore/Event/KinematicBodyMovedEvent.h"
#include "Core/GameCore/Event/TextureAtlasGeneratedEvent.h"
#include "Core/GameCore/Event/MouseMovedEvent.h"
#include "Core/GameCore/Event/MouseScrollEvent.h"
#include "Core/GameCore/Event/MouseButtonDownEvent.h"
#include "Core/GameCore/Event/PhysicsCollisionEvent.h"

using namespace TinyLogger;
using namespace IO;
using namespace EngineUtility;
using namespace Resources;
using namespace EngineCore::Scripts;

namespace EngineCore
{
   Engine::Engine()
       : m_interThreadMgr(),
         mInputManager(std::make_shared<InputManager>()),
         m_scene(std::make_shared<Scene>(m_interThreadMgr)),
         m_luaScriptProcessor(std::make_shared<LuaScriptProcessor>(m_interThreadMgr)),
         mActiveAudioOutputDevice(SoundDevice::GetInstance()),
         mRenderThreadDeltaTimeSeconds(),
         mGameThreadDeltaTimeSeconds()
   {
      m_scene->Initialize();
   }

   Engine::~Engine()
   {
      PauseGameThreadEvent::GetInstance()->RemoveListener(PauseGameThreadEvent::GetInstanceId());
      ExitGameThreadEvent::GetInstance()->RemoveListener(ExitGameThreadEvent::GetInstanceId());
      LoadLevelGameThreadEvent::GetInstance()->RemoveListener(LoadLevelGameThreadEvent::GetInstanceId());
   }

   void Engine::Initialize()
   {
      GameThreadEventDispatcher::GetInstance()
          ->RegisterEventsByType<CameraTransformChangedGameThreadEvent,
                                 PlayerMovedGameThreadEvent,
                                 PhysicsComponentUpdatedGameThreadEvent,
                                 KeyboardButtonDownGameThreadEvent,
                                 KinematicBodyMovedGameThreadEvent,
                                 TextureAtlasGeneratedGameThreadEvent,
                                 MouseMovedGameThreadEvent,
                                 MouseScrollGameThreadEvent,
                                 MouseButtonDownGameThreadEvent,
                                 PhysicsCollisionGameThreadEvent,
                                 PauseGameThreadEvent,
                                 ExitGameThreadEvent,
                                 LoadLevelGameThreadEvent,
                                 WindowSizeChangedGameThreadEvent,
                                 BroadcastGameThreadEvent,
                                 MouseButtonDownRootEvent>();

      LuaThreadEventDispatcher::GetInstance()
          ->RegisterEventsByType<KeyboardButtonDownLuaThreadEvent,
                                 MouseMovedLuaThreadEvent,
                                 MouseScrollLuaThreadEvent,
                                 MouseButtonDownLuaThreadEvent,
                                 WindowSizeChangedLuaThreadEvent,
                                 BroadcastLuaThreadEvent>();

      m_sceneRenderer = std::make_shared<SceneRenderer>(m_interThreadMgr);
      m_interThreadMgr.SetSceneRendererWP(m_sceneRenderer);
      m_interThreadMgr.SetSceneWP(m_scene);
      m_interThreadMgr.SetLuaScriptProcessorWP(m_luaScriptProcessor);
      m_gameThread = std::thread(std::bind(&Engine::GameThreadPulse, this));
      m_luaThread = std::thread(std::bind(&Engine::LuaThreadPulse, this));

#if DEBUG
      m_resourceConsumptionLogTimer.SetIntervalMs(3000);
      m_resourceConsumptionLogTimer.SetIsRepeat(true);
      m_resourceConsumptionLogTimer.SetIsPausable(false);
      m_resourceConsumptionLogTimer.SetCallback([resObs = &mResourceUsageObserver]() { 
         resObs->CollectResourceConsumptionInfo();
         LogInfo("Pid:", resObs->GetPid(), " mem mb:", resObs->GetLastMemoryUsageMegabytes());
      });
      m_resourceConsumptionLogTimer.StartTimer();
#endif

      const auto &thisSp = std::dynamic_pointer_cast<Engine>(shared_from_this());
      PauseGameThreadEvent::GetInstance()->AddListener(thisSp);
      ExitGameThreadEvent::GetInstance()->AddListener(thisSp);
      LoadLevelGameThreadEvent::GetInstance()->AddListener(thisSp);
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

   InterThreadCommunicationMgr &Engine::GetThreadCommunicationManager()
   {
      return m_interThreadMgr;
   }

   void Engine::SetLevelFactory(const std::shared_ptr<ILevelFactory> &lvlFactory)
   {
      m_levelFactory = lvlFactory;
   }

   void Engine::StopGameThreadExecution()
   {
      bGameThreadExecution.store(false);
   }

   void Engine::StopLuaThreadExecution()
   {
      bLuaThreadExecution.store(false);
   }

   void Engine::UnloadCurrentLevel()
   {
      // Clear jobs for game and lua threads
      m_interThreadMgr.SetIsAllowedPushGameThreadJobs(false);
      m_interThreadMgr.SetIsAllowedPushLuaThreadJobs(false);
      m_interThreadMgr.ClearGameThreadJobs();
      m_interThreadMgr.ClearLuaThreadJobs();
      std::this_thread::sleep_for(1000ms); // wait until the lua thread or game thread can still run
      m_level->UnloadLevel();
      m_scene->UnloadScene();
      m_luaScriptProcessor->CleanUp();
      m_sceneRenderer->CleanUp();
      ResourceMap::GetInstance()->CleanUp();
      m_interThreadMgr.SetIsAllowedPushGameThreadJobs(true);
      m_interThreadMgr.SetIsAllowedPushLuaThreadJobs(true);
   }

   void Engine::PlayLevel(const std::string &levelName)
   {
      assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"));
      assert(m_levelFactory);

      if (!m_level || (m_level->GetLevelName() != levelName))
      {
         bLevelIsLoading.store(true, std::memory_order::memory_order_seq_cst);
         bPauseGameThreadExecution.store(false, std::memory_order::memory_order_seq_cst);
         if (m_level)
         {
            UnloadCurrentLevel();
         }

         const auto newLevel = m_levelFactory->CreateLevel(levelName);
         assert(newLevel);
         m_level.reset();
         m_level = newLevel;
         m_level->SetScene(m_scene);
         PreLevelInit();
         OnLevelInit();
         PostPhysicsInitialize();
         PostLevelInit();
         ResourceMap::GetInstance()->WaitUntilResourcesLoad();
         PostPlayLevelFinished();
         bLevelIsLoading.store(false, std::memory_order::memory_order_seq_cst);
      }
   }

   void Engine::PreLevelInit()
   {
      m_level->PreLevelInit();
   }

   void Engine::OnLevelInit()
   {
      m_scene->OnLevelInit();
      m_level->InitLevel();
   }

   void Engine::PostLevelInit()
   {
      m_level->PostLevelInit();
      m_scene->PostLevelInit();
      TextureAtlasFactory::GetInstance()->AllocateAtlasSpace();
      m_sceneRenderer->PostLevelInit();
   }

   void Engine::PostPhysicsInitialize()
   {
      m_level->PostPhysicsInitialize();
      m_scene->PostPhysicsInitialize();
   }

   void Engine::PostPlayLevelFinished()
   {
      m_level->PostPlayLevelFinished();
      m_scene->PostPlayLevelFinished();
   }

   size_t rtCounter = 0;
   size_t gtCounter = 0;
   size_t luaThreadCounter = 0;

   float sumRtFramesTime = 0.0f;
   float sumGtFramesTime = 0.0f;
   float sumLuaThreadFramesTime = 0.0f;

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

   void Engine::ProcessEvent(const LoadLevelGameThreadEvent::EventData_t &data)
   {
      const auto lvlName = std::get<0>(data);
      static constexpr auto functionId = Hash64_CT("Engine::ProcessEvent::LoadLevelGameThreadEvent");
      m_interThreadMgr.ExecuteOnRenderThread(Thread::eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, 0, functionId, [this, lvlName]()
                                             { PlayLevel(lvlName); });
   }

   void Engine::LuaThreadPulse()
   {
      using namespace std::chrono_literals;
      ThreadHelper::GetInstance()->RegisterThread("Lua");

      while (bLuaThreadExecution.load(std::memory_order::memory_order_seq_cst))
      {
         const auto ltStartTimePoint = EngineTime::GetNowTime();
         if (!bLevelIsLoading.load())
         {
            ProcessLuaThreadEvents(eExecutionOrder::PRE_EXECUTION);
            m_interThreadMgr.SpinLuaThreadJob();
            m_luaScriptProcessor->Tick(mLuaThreadDeltaTimeSeconds);

            ProcessLuaThreadEvents(eExecutionOrder::POST_EXECUTION);
         }
         std::this_thread::sleep_for(20ms);
         mLuaThreadDeltaTimeSeconds = (float)EngineTime::GetSecondsFromDuration(
             EngineTime::GetPassedDuration(ltStartTimePoint));

#ifdef DEBUG
         if (luaThreadCounter == 100)
         {
            luaThreadCounter = 0;
            const float fps = 100.0f / (float)sumLuaThreadFramesTime;
            m_scene->SetLuaThreadFPSTextValue(fps);
            sumLuaThreadFramesTime = 0.0f;
         }
         sumLuaThreadFramesTime += mLuaThreadDeltaTimeSeconds;
         ++luaThreadCounter;
#endif
      }
   }

   void Engine::GameThreadPulse()
   {
      ThreadHelper::GetInstance()->RegisterThread("Game");

      while (bGameThreadExecution.load(std::memory_order::memory_order_seq_cst))
      {
         const auto gtStartTimePoint = EngineTime::GetNowTime();

         if (!bLevelIsLoading.load())
         {
            /* Events: pre execution */
            ProcessGameThreadEvents(eExecutionOrder::PRE_EXECUTION);

            /* Work Jobs */
            m_interThreadMgr.SpinGameThreadJobs();

            if (!bPauseGameThreadExecution.load())
            {
               GameThreadTimersHolder::GetInstance()->Tick(mGameThreadDeltaTimeSeconds);
               m_scene->Tick(mGameThreadDeltaTimeSeconds);
               m_level->Tick(mGameThreadDeltaTimeSeconds);
            }

            GameThreadTimersHolder::GetInstance()->UnpausableTick(mGameThreadDeltaTimeSeconds);
            m_scene->UnpausableTick(mGameThreadDeltaTimeSeconds);
            m_level->UnpausableTick(mGameThreadDeltaTimeSeconds);

            /* Events: post execution */
            ProcessGameThreadEvents(eExecutionOrder::POST_EXECUTION);
         }
         mGameThreadDeltaTimeSeconds = (float)EngineTime::GetSecondsFromDuration(
             EngineTime::GetPassedDuration(gtStartTimePoint));

#ifdef DEBUG
         if (gtCounter == 1000)
         {
            gtCounter = 0;
            const float fps = 1000.0f / (float)sumGtFramesTime;
            m_scene->SetGameThreadFPSTextValue(fps);
            sumGtFramesTime = 0.0f;
         }
         sumGtFramesTime += mGameThreadDeltaTimeSeconds;
         ++gtCounter;
#endif
      }
   }

   void Engine::ProcessGameThreadEvents(const eExecutionOrder order)
   {
      GameThreadEventDispatcher::GetInstance()->ProcessEvents(order);
   }

   void Engine::ProcessLuaThreadEvents(const eExecutionOrder order)
   {
      LuaThreadEventDispatcher::GetInstance()->ProcessEvents(order);
   }

   void Engine::RenderThreadPulse()
   {
      const auto rtStartTimePoint = EngineTime::GetNowTime();
      m_interThreadMgr.SpinRenderThreadJobs();
      m_sceneRenderer->RenderScene_RenderThread();

      mRenderThreadDeltaTimeSeconds =
          (float)EngineTime::GetSecondsFromDuration(EngineTime::GetPassedDuration(rtStartTimePoint));

#ifdef DEBUG
      if (rtCounter == 10)
      {
         rtCounter = 0;
         const float fps = 10.0f / sumRtFramesTime;
         m_scene->SetRenderThreadFPSTextValue(fps);
         sumRtFramesTime = 0.0f;
      }
      sumRtFramesTime += mRenderThreadDeltaTimeSeconds;
      ++rtCounter;
#endif
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

   float Engine::GetLuaThreadDeltaTime() const
   {
      return mLuaThreadDeltaTimeSeconds;
   }

   bool Engine::IsExitGameState() const
   {
      return bExitGame;
   }

   std::shared_ptr<Scene> Engine::GetSceneSp() const
   {
      return m_scene;
   }

#if DEBUG

   void Engine::RecompileAllShaders()
   {
      LogInfo("Engine::RecompileAllShaders");
      Resources::ShaderPool::GetInstance()->RecompileShaders();
      Resources::CompositeShaderPool::GetInstance()->RecompileShaders();
   }

   void Engine::RestartLuaScripts()
   {
      LogInfo("Engine::RestartLuaScripts");
      if (m_level)
      {
         m_level->RestartLuaScripts();
      }
   }

#endif
}