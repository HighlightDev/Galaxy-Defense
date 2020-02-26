#pragma once

#include <stdint.h>

namespace Game
{

	struct InputData
	{
		inline int32_t GetMouseDeltaX() const
		{
			return m_mouseX - m_prevMouseX;
		}

		inline int32_t GetMouseDeltaY() const
		{
			return m_mouseY - m_prevMouseY;
		}

		inline void SetMouseX(int32_t mouseX)
		{
			m_prevMouseX = m_mouseX;
			m_mouseX = mouseX;
		}

		inline void SetMouseY(int32_t mouseY)
		{
			m_prevMouseY = m_mouseY;
			m_mouseY = mouseY;
		}

		inline int32_t GetMouseX() const
		{
			return m_mouseX;
		}

		inline int32_t GetMouseY() const
		{
			return m_mouseY;
		}

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

	private:

		int32_t m_mouseX;
		int32_t m_mouseY;

		int32_t m_windowWidth;
		int32_t m_windowHeight;
		int32_t m_screenWidth;
		int32_t m_screenHeight;
		int32_t m_windowPosX;
		int32_t m_windowPosY;

		int32_t m_prevMouseX;
		int32_t m_prevMouseY;
	};

}
