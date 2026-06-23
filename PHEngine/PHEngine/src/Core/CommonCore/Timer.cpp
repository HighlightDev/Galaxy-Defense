#include "Timer.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/LoggerExtension.h"

namespace EngineCore {

GameThreadTimersHolder::GameThreadTimersHolder()
    : mTimerInstances()
{
    mLastExpiredTimersCheckTime = EngineTime::GetCurrentTime();
    mTimerInstances.reserve(1000);
}

GameThreadTimersHolder* GameThreadTimersHolder::GetInstance()
{
    static GameThreadTimersHolder mInstance;
    return &mInstance;
}

void GameThreadTimersHolder::RegisterTimerInstance(std::shared_ptr<GameThreadTimer> instance)
{
    LogInfo(
        "GameThreadTimersHolder::RegisterTimerInstance: instanceID: ",
        instance->GetInstanceId(),
        ", threadName: ",
        ThreadHelper::GetInstance()->GetCurrentThreadNameFromRegisteredThreads());
    mTimerInstances.emplace_back(instance);
}

void GameThreadTimersHolder::checkTimersForExpiration()
{
    mCheckForExpiredTimersInProgress.store(true);
    const auto currentTime = EngineTime::GetCurrentTime();
    const auto expiredTimersCheckDuration = EngineTime::GetPassedDuration(mLastExpiredTimersCheckTime);
    if (EngineTime::GetTimeDifferenceInMilliseconds(expiredTimersCheckDuration) > 2000.0) {
        mTimerInstances.erase(
            std::remove_if(
                mTimerInstances.begin(),
                mTimerInstances.end(),
                [](const std::weak_ptr<GameThreadTimer>& timerWp) { return timerWp.expired(); }),
            mTimerInstances.end());
        mLastExpiredTimersCheckTime = currentTime;
    }
    mCheckForExpiredTimersInProgress.store(false);
}

void GameThreadTimersHolder::Tick(const float deltaSeconds, const float playSpeed)
{
    checkTimersForExpiration();

    if (mCheckForExpiredTimersInProgress.load()) {
        return;
    }
    // Pausable timers are gameplay timers (spawn cadence, cooldowns, ...), so they advance at game speed.
    const float deltaMilliseconds = deltaSeconds * playSpeed * 1000.0f;
    for (const auto& timerWp : mTimerInstances) {
        if (const auto& timerSp = timerWp.lock()) {
            if (timerSp->m_isPausable) {
                timerSp->TimerPulse(deltaMilliseconds);
            }
        }
    }
}

void GameThreadTimersHolder::UnpausableTick(const float deltaSeconds, const float playSpeed)
{
    checkTimersForExpiration();

    if (mCheckForExpiredTimersInProgress.load()) {
        return;
    }
    const float deltaMilliseconds = deltaSeconds * 1000.0f;
    for (const auto& timerWp : mTimerInstances) {
        if (const auto timerSp = timerWp.lock()) {
            if (!timerSp->m_isPausable) {
                timerSp->TimerPulse(deltaMilliseconds);
            }
        }
    }
}

size_t GameThreadTimer::s_instanceId = 0;

GameThreadTimer::GameThreadTimer()
    : m_instanceId(s_instanceId++)
    , m_intervalMs(0)
    , m_timerTimeMilliseconds(0.0f)
    , m_isRepeat(false)
    , m_isRunning(false)
    , m_isPausable(true)
    , m_isInitialized(false)
{
}

GameThreadTimer::~GameThreadTimer()
{
    LogInfo(
        "GameThreadTimer::~GameThreadTimer: instanceID: ",
        m_instanceId,
        ", threadName: ",
        ThreadHelper::GetInstance()->GetCurrentThreadNameFromRegisteredThreads());
}

void GameThreadTimer::Initialize()
{
    if (!m_isInitialized) {
        GameThreadTimersHolder::GetInstance()->RegisterTimerInstance(shared_from_this());
        m_isInitialized = true;
    }
}

size_t GameThreadTimer::GetInstanceId() const
{
    return m_instanceId;
}

void GameThreadTimer::TimerPulse(const float deltaMilliseconds)
{
    if (m_isRunning) {
        if (m_timerTimeMilliseconds >= (float)m_intervalMs) {
            if (mCallback) {
                mCallback();
            }
            if (m_isRepeat) {
                RestartTimer();
            } else {
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
    ext_assert(m_isInitialized, "Timer wasn't initialized!");
    m_isRunning = true;
    m_timerTimeMilliseconds = 0.0f;
}

void GameThreadTimer::RestartTimer()
{
    StartTimer();
}

void GameThreadTimer::StopTimer()
{
    if (m_isRunning) {
        m_isRunning = false;
    }
}

void GameThreadTimer::SetCallback(std::function<void(void)> callback)
{
    ext_assert(!mCallback, "Callback is already set for this timer!");
    mCallback = callback;
}

bool GameThreadTimer::IsRunning() const
{
    return m_isRunning;
}
} // namespace EngineCore