#pragma once

#include "Core/CommonCore/TimeHelper.h"
#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/Event/ExitGameEvent.h"
#include "Core/GameCore/Event/LoadLevelEvent.h"
#include "Core/GameCore/Event/PauseGameEvent.h"
#include "Core/GameCore/Event/RestartLevelEvent.h"
#include "Core/GameCore/ILevelFactory.h"
#include "Core/GameCore/Input/InputManager.h"
#include "Core/GameCore/Level.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

using namespace EngineCore;
using namespace Graphics::Renderer;
using namespace Event;

namespace EngineCore {
namespace Scripts {
class LuaScriptProcessor;
}
} // namespace EngineCore

namespace EngineCore {
class SoundDevice;

class Engine : public PauseGameThreadEvent,
               public ExitGameThreadEvent,
               public LoadLevelGameThreadEvent,
               public RestartLevelGameThreadEvent,
               public std::enable_shared_from_this<Engine> {
    InterThreadCommunicationMgr m_interThreadMgr;

    std::shared_ptr<InputManager> mInputManager;

    std::shared_ptr<ILevelFactory> m_levelFactory;

    std::shared_ptr<Level> m_level;

    std::shared_ptr<Scene> m_scene;
    std::shared_ptr<SceneRenderer> m_sceneRenderer;
    std::shared_ptr<::EngineCore::Scripts::LuaScriptProcessor> m_luaScriptProcessor;

    std::atomic_bool bGameThreadExecution{true};
    std::atomic_bool bLuaThreadExecution{true};

    std::shared_ptr<SoundDevice> mActiveAudioOutputDevice;

    std::thread m_gameThread;

    std::thread m_luaThread;

    float mRenderThreadDeltaTimeSeconds;

    float mGameThreadDeltaTimeSeconds;

    float mLuaThreadDeltaTimeSeconds;

    std::atomic_bool bPauseGameThreadExecution{false};

    std::atomic_bool bLevelIsLoading{true};

    std::atomic_bool bExitGame{false};

#if DEBUG
    std::shared_ptr<GameThreadTimer> m_resourceConsumptionLogTimer;

    size_t rtCounter{0};
    size_t gtCounter{0};
    size_t ltCounter{0};

    float sumRtSeconds{0.0f};
    float sumGtSeconds{0.0f};
    float sumLtSeconds{0.0f};
#endif

    std::condition_variable mUnloadLevelCv;
    std::mutex mUnloadLevelMutex;

    std::atomic_bool mIsLevelUnloading{false};
    std::atomic_bool mIsGameThreadIdle{false};
    std::atomic_bool mIsLuaThreadIdle{false};

public:
    Engine();

    ~Engine();

    void Initialize();

    void SetLevelFactory(const std::shared_ptr<ILevelFactory>& lvlFactory);

    void PreLevelInit();

    void OnLevelInit();

    void PlayLevel(const std::string& levelName);

    void RestartLevel();

    void PostLevelInit();

    void PostPhysicsInitialize();

    void PostPlayLevelFinished();

    void ProcessGameThreadEvents(const eExecutionOrder order);

    void ProcessLuaThreadEvents(const eExecutionOrder order);

    void ProcessEvent(const PauseGameThreadEvent* sender, const PauseGameThreadEvent::EventData_t& data) override;

    void ProcessEvent(const ExitGameThreadEvent* sender, const ExitGameThreadEvent::EventData_t& data) override;

    void ProcessEvent(const LoadLevelGameThreadEvent* sender, const LoadLevelGameThreadEvent::EventData_t& data) override;

    void ProcessEvent(const RestartLevelGameThreadEvent* sender, const RestartLevelGameThreadEvent::EventData_t& data) override;

    void GameThreadPulse();

    void RenderThreadPulse();

    void LuaThreadPulse();

    void TickWindow();

    std::shared_ptr<InputManager> GetInputManager() const;

    float GetRenderThreadDeltaTime() const;

    float GetGameThreadDeltaTime() const;

    float GetLuaThreadDeltaTime() const;

    InterThreadCommunicationMgr& GetThreadCommunicationManager();

    std::atomic_bool IsExitGameState() const;

    std::shared_ptr<Scene> GetSceneSp() const;

#if DEBUG

    void RecompileAllShaders();

    void RestartLuaScripts();

#endif
    void CleanUp();

private:
    void StopGameThreadExecution();

    void StopLuaThreadExecution();

    void UnloadCurrentLevel();
};
} // namespace EngineCore
