#pragma once

#include "Core/GameCore/ITickable.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace EngineCore {
class GameThreadTimer;

class GameThreadTimersHolder : public ITickable {
private:
    std::vector<std::weak_ptr<GameThreadTimer>> mTimerInstances;

    GameThreadTimersHolder();

public:
    static GameThreadTimersHolder* GetInstance();

    void RegisterTimerInstance(std::shared_ptr<GameThreadTimer> instance);

    void Tick(const float deltaSeconds) override;

    void UnpausableTick(const float deltaTime) override;
};

class GameThreadTimer : public std::enable_shared_from_this<GameThreadTimer> {
    friend class GameThreadTimersHolder;

    static size_t s_instanceId;

    size_t m_instanceId;

    size_t m_intervalMs;

    float m_timerTimeMilliseconds;

    bool m_isRepeat;

    bool m_isRunning;

    bool m_isPausable;

    std::function<void(void)> mCallback;

    bool m_isInitialized;

public:
    GameThreadTimer();

    virtual ~GameThreadTimer() = default;

    GameThreadTimer(const GameThreadTimer&) = delete;

    GameThreadTimer& operator=(const GameThreadTimer&) = delete;

    void Initialize();

    size_t GetInstanceId() const;

    void SetIntervalMs(const size_t intervalMs);

    void SetIsRepeat(const bool isRepeat);

    void SetIsPausable(const bool isPausable);

    void SetCallback(std::function<void(void)> callback);

    void StartTimer();

    void RestartTimer();

    void StopTimer();

    bool IsRunning() const;

protected:
    virtual void TimerPulse(const float deltaMilliseconds);
};

} // namespace EngineCore