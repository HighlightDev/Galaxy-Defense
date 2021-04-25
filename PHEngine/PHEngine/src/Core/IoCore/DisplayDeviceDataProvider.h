#pragma once

#include <stdint.h>

namespace IO
{
	class DisplayDeviceDataProvider
	{
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

      inline void SetWindowWidth(int32_t width)
      {
         m_windowWidth = width;
      }

      inline void SetWindowHeight(int32_t height)
      {
         m_windowHeight = height;
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

      inline int32_t GetWindowPosX() const {

         return m_windowPosX;
      }

      inline int32_t GetWindowPosY() const {

         return m_windowPosY;
      }
	};

}
