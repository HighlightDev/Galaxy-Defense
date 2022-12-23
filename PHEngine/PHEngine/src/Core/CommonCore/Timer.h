#pragma once

#include "TimeHelper.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <functional>

namespace EngineCore
{
    class GameThreadTimer;

    class GameThreadTimersHolder
    {
    private:
        std::vector<GameThreadTimer *> mTimerInstances;

        GameThreadTimersHolder();

    public:
        static GameThreadTimersHolder *GetInstance();

        void RegisterTimerInstance(GameThreadTimer *instance);

        void UnregisterTimerInstance(GameThreadTimer *instance);

        void UpdateTimers();
    };

    class GameThreadTimer
    {
        friend class GameThreadTimersHolder;

        static size_t s_instanceId;

        size_t m_instanceId;

        size_t m_intervalMs;

        Moment_t m_startTimerTime;

        bool m_isRepeat;

        bool m_isRunning;

        std::function<void(void)> mCallback;

    public:
        GameThreadTimer();

        virtual ~GameThreadTimer();

        GameThreadTimer(const GameThreadTimer &) = delete;

        GameThreadTimer &operator=(const GameThreadTimer &) = delete;

        size_t GetInstanceId() const;

        void SetIntervalMs(const size_t intervalMs);

        void SetIsRepeat(const bool isRepeat);

        void StartTimer();

        void RestartTimer();

        void StopTimer();

        void SetCallback(std::function<void(void)> callback);

        bool IsRunning() const;

    protected:
        virtual void TimerPulse();
    }; 

}