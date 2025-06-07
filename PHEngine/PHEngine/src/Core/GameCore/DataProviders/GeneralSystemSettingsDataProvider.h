#pragma once

#include "Core/GameCore/Event/BroadcastEvent.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include <stdint.h>

#include <cstddef>
#include <thread>

using namespace Graphics;

namespace EngineCore::DataProviders {
class GeneralSystemSettingsDataProvider : public Event::BroadcastGameThreadEvent,
                                          public std::enable_shared_from_this<Event::BroadcastGameThreadEvent> {
    std::once_flag m_initFlag;

    // Display device settings
    int32_t m_windowWidth{0};
    int32_t m_windowHeight{0};
    int32_t m_screenWidth{0};
    int32_t m_screenHeight{0};
    int32_t m_windowPosX{0};
    int32_t m_windowPosY{0};

    // Audio settings
    float m_soundGain{1.0f}; // Default gain level (1.0 = 100%)
    float m_musicGain{1.0f}; // Default music gain level (1.0 = 100%)

public:
    GeneralSystemSettingsDataProvider();
    static const std::shared_ptr<GeneralSystemSettingsDataProvider>& GetInstance();

    void Initialize();

    ~GeneralSystemSettingsDataProvider();

    void ProcessEvent(
        const Event::BroadcastGameThreadEvent* sender,
        const typename Event::BroadcastGameThreadEvent::EventData_t& eventData) override;

    void SetWindowSize(const int32_t width, const int32_t height);

    void SetWindowWidth(const int32_t width);

    void SetWindowHeight(const int32_t height);

    inline int32_t GetWindowWidth() const;

    inline int32_t GetWindowHeight() const;

    inline int32_t GetScreenWidth() const;

    inline int32_t GetScreenHeight() const;

    inline void SetScreenWidth(int32_t width);

    inline void SetScreenHeight(int32_t height);

    inline void SetWindowPos(int32_t posX, int32_t posY);

    inline float GetHeightToWidthRatio() const;

    inline float GetWidthToHeightRatio() const;

    inline int32_t GetWindowPosX() const;

    inline int32_t GetWindowPosY() const;

    inline float GetSoundGain() const;

    void SetSoundGain(const float gain, const bool bSendToLua = true);

    inline float GetMusicGain() const;

    void SetMusicGain(const float gain, const bool bSendToLua = true);
};

inline int32_t GeneralSystemSettingsDataProvider::GetWindowWidth() const
{
    return m_windowWidth;
}

inline int32_t GeneralSystemSettingsDataProvider::GetWindowHeight() const
{
    return m_windowHeight;
}

inline int32_t GeneralSystemSettingsDataProvider::GetScreenWidth() const
{
    return m_screenWidth;
}

inline int32_t GeneralSystemSettingsDataProvider::GetScreenHeight() const
{
    return m_screenHeight;
}

inline void GeneralSystemSettingsDataProvider::SetScreenWidth(int32_t width)
{
    m_screenWidth = width;
}

inline void GeneralSystemSettingsDataProvider::SetScreenHeight(int32_t height)
{
    m_screenHeight = height;
}

inline void GeneralSystemSettingsDataProvider::SetWindowPos(int32_t posX, int32_t posY)
{
    m_windowPosX = posX;
    m_windowPosY = posY;
}

inline float GeneralSystemSettingsDataProvider::GetHeightToWidthRatio() const
{
    return (float)m_screenHeight / (float)m_screenWidth;
}

inline float GeneralSystemSettingsDataProvider::GetWidthToHeightRatio() const
{
    return (float)m_screenWidth / (float)m_screenHeight;
}

inline int32_t GeneralSystemSettingsDataProvider::GetWindowPosX() const
{
    return m_windowPosX;
}

inline int32_t GeneralSystemSettingsDataProvider::GetWindowPosY() const
{
    return m_windowPosY;
}

inline float GeneralSystemSettingsDataProvider::GetSoundGain() const
{
    return m_soundGain;
}

inline float GeneralSystemSettingsDataProvider::GetMusicGain() const
{
    return m_musicGain;
}

} // namespace EngineCore::DataProviders
