#pragma once

#include "Core/GameCore/ITickable.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <functional>

namespace EngineCore
{
    class GameThreadTimer;

    class GameThreadTimersHolder
        : public ITickable
    {
    private:
        std::vector<GameThreadTimer *> mTimerInstances;

        GameThreadTimersHolder();

    public:
        static GameThreadTimersHolder *GetInstance();

        void RegisterTimerInstance(GameThreadTimer *instance);

        void UnregisterTimerInstance(GameThreadTimer *instance);

        void Tick(const float deltaSeconds) override;

        void UnpausableTick(const float deltaTime) override;
    };

    class GameThreadTimer
    {
        friend class GameThreadTimersHolder;

        static size_t s_instanceId;

        size_t m_instanceId;

        size_t m_intervalMs;

        float m_timerTimeMilliseconds;

        bool m_isRepeat;

        bool m_isRunning;

        bool m_isPausable;

        std::function<void(void)> mCallback;

    public:
        GameThreadTimer();

        virtual ~GameThreadTimer();

        GameThreadTimer(const GameThreadTimer &) = delete;

        GameThreadTimer &operator=(const GameThreadTimer &) = delete;

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

}