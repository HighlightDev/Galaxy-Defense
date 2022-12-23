#include "Timer.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

namespace EngineCore
{

    GameThreadTimersHolder::GameThreadTimersHolder()
        : mTimerInstances()
    {
    }

    GameThreadTimersHolder *GameThreadTimersHolder::GetInstance()
    {
        static GameThreadTimersHolder mInstance;
        return &mInstance;
    }

    void GameThreadTimersHolder::RegisterTimerInstance(GameThreadTimer *instance)
    {
        assert(mTimerInstances.end() == std::find_if(mTimerInstances.begin(),
                                                     mTimerInstances.end(), [=](const auto m_instance)
                                                     { return instance->GetInstanceId() == m_instance->GetInstanceId(); }));
        mTimerInstances.emplace_back(instance);
    }

    void GameThreadTimersHolder::UnregisterTimerInstance(GameThreadTimer *instance)
    {
        const auto removeIt = std::remove_if(mTimerInstances.begin(),
                                             mTimerInstances.end(), [=](const auto m_instance)
                                             { return instance->GetInstanceId() == m_instance->GetInstanceId(); });
        mTimerInstances.erase(removeIt);
    }

    void GameThreadTimersHolder::UpdateTimers()
    {
        for (const auto &timer : mTimerInstances)
        {
            timer->TimerPulse();
        }
    }

    size_t GameThreadTimer::s_instanceId = 0;

    GameThreadTimer::GameThreadTimer()
        : m_instanceId(s_instanceId++),
          m_intervalMs(0),
          m_startTimerTime(),
          m_isRepeat(false),
          m_isRunning(false)
    {
        GameThreadTimersHolder::GetInstance()->RegisterTimerInstance(this);
    }

    GameThreadTimer::~GameThreadTimer()
    {
        GameThreadTimersHolder::GetInstance()->UnregisterTimerInstance(this);
    }

    size_t GameThreadTimer::GetInstanceId() const
    {
        return m_instanceId;
    }

    void GameThreadTimer::TimerPulse()
    {
        if (m_isRunning)
        {
            if (EngineTime::GetMillisecondsFromDuration(EngineTime::GetPassedDuration(m_startTimerTime)) >= (double)m_intervalMs)
            {
                mCallback();
                if (m_isRepeat)
                {
                    RestartTimer();
                }
                else
                {
                    StopTimer();
                }
            }
        }
    }

    void GameThreadTimer::SetIntervalMs(const size_t intervalMs)
    {
        m_intervalMs = intervalMs;
    }

    void GameThreadTimer::SetIsRepeat(const bool isRepeat)
    {
        m_isRepeat = isRepeat;
    }

    void GameThreadTimer::StartTimer()
    {
        assert(mCallback);
        m_isRunning = true;
        m_startTimerTime = EngineTime::GetNowTime();
    }

    void GameThreadTimer::RestartTimer()
    {
        StartTimer();
    }

    void GameThreadTimer::StopTimer()
    {
        if (m_isRunning)
        {
            m_isRunning = false;
        }
    }

    void GameThreadTimer::SetCallback(std::function<void(void)> callback)
    {
        assert(!mCallback);
        mCallback = callback;
    }

    bool GameThreadTimer::IsRunning() const
    {
        return m_isRunning;
    }
}