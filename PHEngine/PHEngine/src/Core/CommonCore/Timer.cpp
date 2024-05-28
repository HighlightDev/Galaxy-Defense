#include "Timer.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/ThreadHelper.h"

namespace EngineCore
{

    GameThreadTimersHolder::GameThreadTimersHolder()
        : mTimerInstances()
    {
        mTimerInstances.reserve(1000);
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
        LogInfo("GameThreadTimersHolder::RegisterTimerInstance => Current Thread: ", ThreadHelper::GetInstance()->GetCurrentThreadNameFromRegisteredThreads());
        mTimerInstances.emplace_back(instance);
    }

    void GameThreadTimersHolder::UnregisterTimerInstance(GameThreadTimer *instance)
    {
        const auto removeIt = std::remove_if(mTimerInstances.begin(),
                                             mTimerInstances.end(), [=](const auto m_instance)
                                             { return instance->GetInstanceId() == m_instance->GetInstanceId(); });
        LogInfo("GameThreadTimersHolder::UnregisterTimerInstance => Current Thread: ", ThreadHelper::GetInstance()->GetCurrentThreadNameFromRegisteredThreads());
        mTimerInstances.erase(removeIt);
    }

    void GameThreadTimersHolder::Tick(const float deltaSeconds)
    {
        const float deltaMilliseconds = deltaSeconds * 1000.0f;
        for (const auto &timer : mTimerInstances)
        {
            if (timer->m_isPausable)
            {
                timer->TimerPulse(deltaMilliseconds);
            }
        }
    }

    void GameThreadTimersHolder::UnpausableTick(const float deltaSeconds)
    {
        const float deltaMilliseconds = deltaSeconds * 1000.0f;
        for (const auto &timer : mTimerInstances)
        {
            if (!timer->m_isPausable)
            {
                timer->TimerPulse(deltaMilliseconds);
            }
        }
    }

    size_t GameThreadTimer::s_instanceId = 0;

    GameThreadTimer::GameThreadTimer()
        : m_instanceId(s_instanceId++),
          m_intervalMs(0),
          m_timerTimeMilliseconds(0.0f),
          m_isRepeat(false),
          m_isRunning(false),
          m_isPausable(true)
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

    void GameThreadTimer::TimerPulse(const float deltaMilliseconds)
    {
        if (m_isRunning)
        {
            if (m_timerTimeMilliseconds >= (float)m_intervalMs)
            {
                if (mCallback)
                {
                    mCallback();
                }
                if (m_isRepeat)
                {
                    RestartTimer();
                }
                else
                {
                    StopTimer();
                }
            }
            m_timerTimeMilliseconds += deltaMilliseconds;
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

    void GameThreadTimer::SetIsPausable(const bool isPausable)
    {
        m_isPausable = isPausable;
    }

    void GameThreadTimer::StartTimer()
    {
        m_isRunning = true;
        m_timerTimeMilliseconds = 0.0f;
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