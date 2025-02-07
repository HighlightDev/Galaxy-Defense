#pragma once

#include "Core/GameCore/Event/WindowSizeChangedEvent.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include <stdint.h>

#include <cstddef>

using namespace Graphics;

namespace IO {
class DisplayDeviceDataProvider {
    int32_t m_windowWidth = 0;
    int32_t m_windowHeight = 0;
    int32_t m_screenWidth = 0;
    int32_t m_screenHeight = 0;
    int32_t m_windowPosX = 0;
    int32_t m_windowPosY = 0;

    DisplayDeviceDataProvider();

public:
    static DisplayDeviceDataProvider* GetInstance();

    inline int32_t GetWindowWidth() const
    {
        return m_windowWidth;
    }

    inline int32_t GetWindowHeight() const
    {
        return m_windowHeight;
    }

    inline int32_t GetScreenWidth() const
    {
        return m_screenWidth;
    }

    inline int32_t GetScreenHeight() const
    {
        return m_screenHeight;
    }

    inline void SetWindowSize(const int32_t width, const int32_t height)
    {
        bool bWindowSizeDirty = false;
        if (m_windowWidth != width) {
            m_windowWidth = width;
            bWindowSizeDirty = true;
        }

        if (m_windowHeight != height) {
            m_windowHeight = height;
            bWindowSizeDirty = true;
        }

        if (bWindowSizeDirty) {
            Event::WindowSizeChangedGameThreadEvent::GetInstance()->SendEvent(
                Event::eExecutionOrder::POST_EXECUTION, ViewPortInfo(0, 0, m_windowWidth, m_windowHeight));
            Event::WindowSizeChangedLuaThreadEvent::GetInstance()->SendEvent(
                Event::eExecutionOrder::POST_EXECUTION, ViewPortInfo(0, 0, m_windowWidth, m_windowHeight));
        }
    }

    inline void SetWindowWidth(int32_t width)
    {
        if (m_windowWidth != width) {
            m_windowWidth = width;
            Event::WindowSizeChangedGameThreadEvent::GetInstance()->SendEvent(
                Event::eExecutionOrder::POST_EXECUTION, ViewPortInfo(0, 0, m_windowWidth, m_windowHeight));
            Event::WindowSizeChangedLuaThreadEvent::GetInstance()->SendEvent(
                Event::eExecutionOrder::POST_EXECUTION, ViewPortInfo(0, 0, m_windowWidth, m_windowHeight));
        }
    }

    inline void SetWindowHeight(int32_t height)
    {
        if (m_windowHeight != height) {
            m_windowHeight = height;
            Event::WindowSizeChangedGameThreadEvent::GetInstance()->SendEvent(
                Event::eExecutionOrder::POST_EXECUTION, ViewPortInfo(0, 0, m_windowWidth, m_windowHeight));
            Event::WindowSizeChangedLuaThreadEvent::GetInstance()->SendEvent(
                Event::eExecutionOrder::POST_EXECUTION, ViewPortInfo(0, 0, m_windowWidth, m_windowHeight));
        }
    }

    inline void SetScreenWidth(int32_t width)
    {
        m_screenWidth = width;
    }

    inline void SetScreenHeight(int32_t height)
    {
        m_screenHeight = height;
    }

    inline void SetWindowPos(int32_t posX, int32_t posY)
    {
        m_windowPosX = posX;
        m_windowPosY = posY;
    }

    inline float GetHeightToWidthRatio() const
    {
        return (float)m_screenHeight / (float)m_screenWidth;
    }

    inline float GetWidthToHeightRatio() const
    {
        return (float)m_screenWidth / (float)m_screenHeight;
    }

    inline int32_t GetWindowPosX() const
    {
        return m_windowPosX;
    }

    inline int32_t GetWindowPosY() const
    {
        return m_windowPosY;
    }
};

} // namespace IO
