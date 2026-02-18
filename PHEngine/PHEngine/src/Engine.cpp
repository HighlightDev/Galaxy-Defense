#include "Engine.h"

#include "Core/AudioCore/SoundDevice.h"
#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/Event/BroadcastEvent.h"
#include "Core/GameCore/Event/CameraTransformChangedEvent.h"
#include "Core/GameCore/Event/GameThreadEventDispatcher.h"
#include "Core/GameCore/Event/GeneralSystemSettingsChangedEvent.h"
#include "Core/GameCore/Event/KeyboardInputEvent.h"
#include "Core/GameCore/Event/KinematicBodyMovedEvent.h"
#include "Core/GameCore/Event/LuaThreadEventDispatcher.h"
#include "Core/GameCore/Event/MouseButtonDownEvent.h"
#include "Core/GameCore/Event/MouseMovedEvent.h"
#include "Core/GameCore/Event/MouseScrollEvent.h"
#include "Core/GameCore/Event/PhysicsCollisionEvent.h"
#include "Core/GameCore/Event/PhysicsComponentUpdatedEvent.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/GameCore/Event/TextureAtlasGeneratedEvent.h"
#include "Core/GameCore/Event/WindowSizeChangedEvent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/ResourceManagerCore/Pool/FreeTypeFontMeshPool.h"
#include "Core/ResourceManagerCore/Pool/InstancedMeshPool.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/ParticlesPool.h"
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"
#include "Core/ResourceManagerCore/Pool/RuntimeGeneratedMeshPool.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/ResourceManagerCore/Pool/SoundBufferPool.h"
#include "Core/ResourceManagerCore/Pool/SoundStreamPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/ResourceManagerCore/Pool/UniformBufferPool.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#if DEBUG
#include "Core/CommonCore/ResourceUsageObserver.h"
#endif

using namespace TinyLogger;
using namespace IO;
using namespace EngineUtility;
using namespace Resources;
using namespace EngineCore::Scripts;

namespace EngineCore {
Engine::Engine()
    : m_interThreadMgr()
    , mInputManager(std::make_shared<InputManager>())
    , m_scene(std::make_shared<Scene>(m_interThreadMgr))
    , m_luaScriptProcessor(std::make_shared<LuaScriptProcessor>(m_interThreadMgr))
    , mActiveAudioOutputDevice(SoundDevice::GetInstance())
    , mRenderThreadDeltaTimeSeconds()
    , mGameThreadDeltaTimeSeconds()
{
    LogInfo("Engine::ctor");
    m_scene->Initialize();
}

Engine::~Engine()
{
    LogInfo("Engine::dctor");
    PauseGameThreadEvent::GetInstance()->RemoveListener(PauseGameThreadEvent::GetInstanceId());
    ExitGameThreadEvent::GetInstance()->RemoveListener(ExitGameThreadEvent::GetInstanceId());
    LoadLevelGameThreadEvent::GetInstance()->RemoveListener(LoadLevelGameThreadEvent::GetInstanceId());
    RestartLevelGameThreadEvent::GetInstance()->RemoveListener(RestartLevelGameThreadEvent::GetInstanceId());
}

void Engine::Initialize()
{
    const std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    LogInfo("OpenGL version: ", version);

    LogInfo("Engine::Initialize");
    GameThreadEventDispatcher::GetInstance()
        ->RegisterEventsByType<
            CameraTransformChangedGameThreadEvent,
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
            MouseButtonDownRootEvent,
            RestartLevelGameThreadEvent,
            GeneralSystemSettingsChangedGameThreadEvent>();

    LuaThreadEventDispatcher::GetInstance()
        ->RegisterEventsByType<
            KeyboardButtonDownLuaThreadEvent,
            MouseMovedLuaThreadEvent,
            MouseScrollLuaThreadEvent,
            MouseButtonDownLuaThreadEvent,
            WindowSizeChangedLuaThreadEvent,
            BroadcastLuaThreadEvent,
            GeneralSystemSettingsChangedLuaThreadEvent>();

    m_sceneRenderer = std::make_shared<SceneRenderer>(m_interThreadMgr);
    m_sceneRenderer->Initialize();
    m_interThreadMgr.SetSceneRendererWP(m_sceneRenderer);
    m_interThreadMgr.SetSceneWP(m_scene);
    m_interThreadMgr.SetLuaScriptProcessorWP(m_luaScriptProcessor);
    m_gameThread = std::thread(std::bind(&Engine::GameThreadPulse, this));
    m_luaThread = std::thread(std::bind(&Engine::LuaThreadPulse, this));

#if DEBUG
    m_resourceConsumptionLogTimer = std::make_shared<GameThreadTimer>();
    m_resourceConsumptionLogTimer->Initialize();
    m_resourceConsumptionLogTimer->SetIntervalMs(1000);
    m_resourceConsumptionLogTimer->SetIsRepeat(true);
    m_resourceConsumptionLogTimer->SetIsPausable(false);
    m_resourceConsumptionLogTimer->SetCallback([]() {
        const auto& resObs = ResourceUsageObserver::GetInstance();
        resObs->CollectResourceConsumptionInfo();
        LogInfo("Pid:", resObs->GetPid(), " mem mb:", resObs->GetLastMemoryUsageMegabytes());
    });
    m_resourceConsumptionLogTimer->StartTimer();
#endif

    const auto& thisSp = std::dynamic_pointer_cast<Engine>(shared_from_this());
    PauseGameThreadEvent::GetInstance()->AddListener(thisSp);
    ExitGameThreadEvent::GetInstance()->AddListener(thisSp);
    LoadLevelGameThreadEvent::GetInstance()->AddListener(thisSp);
    RestartLevelGameThreadEvent::GetInstance()->AddListener(thisSp);
}

void Engine::CleanUp()
{
    LogInfo("Engine::CleanUp");
    StopGameThreadExecution();
    StopLuaThreadExecution();
    m_gameThread.join();
    m_luaThread.join();

    CompositeShaderPool::GetInstance()->CleanUp();
    FreeTypeFontMeshPool::GetInstance()->CleanUp();
    InstancedMeshPool::GetInstance()->CleanUp();
    MeshPool::GetInstance()->CleanUp();
    TexturePool::GetInstance()->CleanUp();
    ParticlesPool::GetInstance()->CleanUp();
    RenderTargetPool::GetInstance()->CleanUp();
    ShaderPool::GetInstance()->CleanUp();
    SimplePrimitivePool::GetInstance()->CleanUp();
    SoundBufferPool::GetInstance()->CleanUp();
    SoundStreamPool::GetInstance()->CleanUp();
    RuntimeGeneratedMeshPool::GetInstance()->CleanUp();
    UniformBufferPool::GetInstance()->CleanUp();
    mActiveAudioOutputDevice->CleanUp();
}

std::shared_ptr<InputManager> Engine::GetInputManager() const
{
    return mInputManager;
}

InterThreadCommunicationMgr& Engine::GetThreadCommunicationManager()
{
    return m_interThreadMgr;
}

void Engine::SetLevelFactory(const std::shared_ptr<ILevelFactory>& lvlFactory)
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
    const auto& resObserver = ResourceUsageObserver::GetInstance();
    resObserver->CollectResourceConsumptionInfo();
    LogInfo("Engine::UnloadCurrentLevel: mem before lvl unload: ", resObserver->GetLastMemoryUsageMegabytes());
    // Clear jobs for game and lua threads
    m_interThreadMgr.SetIsAllowedPushGameThreadJobs(false);
    m_interThreadMgr.SetIsAllowedPushLuaThreadJobs(false);
    m_interThreadMgr.ClearGameThreadJobs();
    m_interThreadMgr.ClearLuaThreadJobs();

    // Wait until game and lua threads are idle
    mIsLevelUnloading = true;
    std::unique_lock<std::mutex> lk(mUnloadLevelMutex);
    mUnloadLevelCv.wait(lk, [this] { return mIsGameThreadIdle && mIsLuaThreadIdle; });
    mIsLevelUnloading = false;
    mIsGameThreadIdle = false;
    mIsLuaThreadIdle = false;
    m_level->UnloadLevel();
    m_scene->UnloadScene();
    m_luaScriptProcessor->CleanUp();
    m_sceneRenderer->CleanUp();
    ResourceMap::GetInstance()->CleanUp();
    UniformBufferPool::GetInstance()->CleanUp();
    resObserver->CollectResourceConsumptionInfo();
    LogInfo("Engine::UnloadCurrentLevel: mem after lvl unload: ", resObserver->GetLastMemoryUsageMegabytes());
    m_interThreadMgr.SetIsAllowedPushGameThreadJobs(true);
    m_interThreadMgr.SetIsAllowedPushLuaThreadJobs(true);
    m_sceneRenderer->Initialize();
}

void Engine::PlayLevel(const std::string& levelName)
{
    LogInfo("Engine::PlayLevel: ", levelName);
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"),
        "Engine::PlayLevel must be called from Render thread");
    ext_assert(m_levelFactory, "Level factory is not initialized");

    if (!m_level || (m_level->GetLevelName() != levelName)) {
        bLevelIsLoading.store(true, std::memory_order::seq_cst);
        bPauseGameThreadExecution.store(false, std::memory_order::seq_cst);
        if (m_level) {
            UnloadCurrentLevel();
        }

        const auto newLevel = m_levelFactory->CreateLevel(levelName);
        ext_assert(newLevel, "Failed to create level: " + levelName);
        m_level.reset();
        m_level = newLevel;
        m_level->SetScene(m_scene);
        PreLevelInit();
        OnLevelInit();
        PostPhysicsInitialize();
        PostLevelInit();
        ResourceMap::GetInstance()->WaitUntilResourcesLoad();
        PostPlayLevelFinished();
        bLevelIsLoading.store(false, std::memory_order::seq_cst);
    }
}

void Engine::RestartLevel()
{
    LogInfo("Engine::RestartLevel");
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"),
        "Engine::RestartLevel must be called from Render thread");
    ext_assert(m_levelFactory, "Level factory is not initialized");
    ext_assert(m_level, "Current level is null, cannot restart");

    bLevelIsLoading.store(true, std::memory_order::seq_cst);
    bPauseGameThreadExecution.store(false, std::memory_order::seq_cst);
    const auto currentLevelName = m_level->GetLevelName();
    UnloadCurrentLevel();

    const auto newLevel = m_levelFactory->CreateLevel(currentLevelName);
    ext_assert(newLevel, "Failed to create level on restart: " + currentLevelName);
    m_level.reset();
    m_level = newLevel;
    m_level->SetScene(m_scene);
    PreLevelInit();
    OnLevelInit();
    PostPhysicsInitialize();
    PostLevelInit();
    ResourceMap::GetInstance()->WaitUntilResourcesLoad();
    PostPlayLevelFinished();
    bLevelIsLoading.store(false, std::memory_order::seq_cst);
}

void Engine::PreLevelInit()
{
    LogInfo("Engine::PreLevelInit");
    m_level->PreLevelInit();
}

void Engine::OnLevelInit()
{
    LogInfo("Engine::OnLevelInit");
    m_scene->OnLevelInit();
    m_level->InitLevel();
}

void Engine::PostLevelInit()
{
    LogInfo("Engine::PostLevelInit");
    m_level->PostLevelInit();
    m_scene->PostLevelInit();
    TextureAtlasFactory::GetInstance()->AllocateAtlasSpace();
    m_sceneRenderer->PostLevelInit();
}

void Engine::PostPhysicsInitialize()
{
    LogInfo("Engine::PostPhysicsInitialize");
    m_level->PostPhysicsInitialize();
    m_scene->PostPhysicsInitialize();
}

void Engine::PostPlayLevelFinished()
{
    LogInfo("Engine::PostPlayLevelFinished");
    m_level->PostPlayLevelFinished();
    m_scene->PostPlayLevelFinished();
}

void Engine::ProcessEvent(const PauseGameThreadEvent* sender, const PauseGameThreadEvent::EventData_t& data)
{
    LogInfo("Engine::ProcessEvent::PauseGameThreadEvent: ", std::get<0>(data) ? "PAUSE" : "UNPAUSE");
    bPauseGameThreadExecution.store(std::get<0>(data));
}

void Engine::ProcessEvent(const ExitGameThreadEvent* sender, const ExitGameThreadEvent::EventData_t& data)
{
    LogInfo("Engine::ProcessEvent::ExitGameThreadEvent");
    bExitGame = true;
    StopGameThreadExecution();
    StopLuaThreadExecution();
}

void Engine::ProcessEvent(const LoadLevelGameThreadEvent* sender, const LoadLevelGameThreadEvent::EventData_t& data)
{
    const auto lvlName = std::get<0>(data);
    LogInfo("Engine::ProcessEvent::LoadLevelGameThreadEvent: ", lvlName);
    static constexpr auto functionId = Hash64_CT("Engine::ProcessEvent::LoadLevelGameThreadEvent");
    m_interThreadMgr.ExecuteOnRenderThread(
        Thread::eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
        0,
        functionId,
        [weak = weak_from_this(), lvlName](
            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
            std::weak_ptr<EngineCore::Scene> sceneWp,
            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
            if (const auto& strong = weak.lock()) {
                strong->PlayLevel(lvlName);
            }
        });
}

void Engine::ProcessEvent(const RestartLevelGameThreadEvent* sender, const RestartLevelGameThreadEvent::EventData_t& data)
{
    LogInfo("Engine::ProcessEvent::RestartLevelGameThreadEvent");
    static constexpr auto functionId = Hash64_CT("Engine::ProcessEvent::RestartLevelGameThreadEvent");
    m_interThreadMgr.ExecuteOnRenderThread(
        Thread::eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
        0,
        functionId,
        [weak = weak_from_this()](
            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
            std::weak_ptr<EngineCore::Scene> sceneWp,
            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
            if (const auto& strong = weak.lock()) {
                strong->RestartLevel();
            }
        });
}

void Engine::LuaThreadPulse()
{
    using namespace std::chrono_literals;
    ThreadHelper::GetInstance()->RegisterThread("Lua");

    while (bLuaThreadExecution.load(std::memory_order::seq_cst)) {
        const auto ltStartTimePoint = EngineTime::GetNowTime();
        if (!bLevelIsLoading.load()) {
            ProcessLuaThreadEvents(eExecutionOrder::PRE_EXECUTION);
            m_interThreadMgr.SpinLuaThreadJob();
            m_luaScriptProcessor->Tick(mLuaThreadDeltaTimeSeconds);

            ProcessLuaThreadEvents(eExecutionOrder::POST_EXECUTION);
        }
        std::this_thread::sleep_for(20ms);
        mLuaThreadDeltaTimeSeconds = (float)EngineTime::GetSecondsFromDuration(EngineTime::GetPassedDuration(ltStartTimePoint));

#ifdef DEBUG
        if (sumLtSeconds >= 1.0f) {
            const float fps = static_cast<float>(ltCounter) * (1.0f / sumLtSeconds);
            m_scene->SetLuaThreadFPSTextValue(fps);
            ltCounter = 0;
            sumLtSeconds = 0.0f;
        }
        sumLtSeconds += mLuaThreadDeltaTimeSeconds;
        ++ltCounter;
#endif
        if (mIsLevelUnloading) {
            mIsLuaThreadIdle = true;
            mUnloadLevelCv.notify_all(); // notify all waiting threads to resume
        }
    }
}

void Engine::GameThreadPulse()
{
    ThreadHelper::GetInstance()->RegisterThread("Game");

    while (bGameThreadExecution.load(std::memory_order::seq_cst)) {
        const auto gtStartTimePoint = EngineTime::GetNowTime();

        if (!bLevelIsLoading.load()) {
            /* Events: pre execution */
            ProcessGameThreadEvents(eExecutionOrder::PRE_EXECUTION);

            /* Work Jobs */
            m_interThreadMgr.SpinGameThreadJobs();

            if (!bPauseGameThreadExecution.load()) {
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
        mGameThreadDeltaTimeSeconds = (float)EngineTime::GetSecondsFromDuration(EngineTime::GetPassedDuration(gtStartTimePoint));

#ifdef DEBUG
        if (sumGtSeconds >= 1.0f) { // duration is >= than one second
            const float fps = static_cast<float>(gtCounter) * (1.0f / sumGtSeconds);
            m_scene->SetGameThreadFPSTextValue(fps);
            sumGtSeconds = 0.0f;
            gtCounter = 0;
        }
        sumGtSeconds += mGameThreadDeltaTimeSeconds;
        ++gtCounter;
#endif

        if (mIsLevelUnloading) {
            mIsGameThreadIdle = true;
            mUnloadLevelCv.notify_all(); // notify all waiting threads to resume
        }
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

    mRenderThreadDeltaTimeSeconds = (float)EngineTime::GetSecondsFromDuration(EngineTime::GetPassedDuration(rtStartTimePoint));

#ifdef DEBUG
    if (sumRtSeconds >= 1.0f) {
        const float fps = (static_cast<float>(rtCounter) * (1.0f / sumRtSeconds));
        m_scene->SetRenderThreadFPSTextValue(fps);
        sumRtSeconds = 0.0f;
        rtCounter = 0;
    }
    sumRtSeconds += mRenderThreadDeltaTimeSeconds;
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
    if (m_level) {
        m_level->RestartLuaScripts();
    }
}

#endif
} // namespace EngineCore