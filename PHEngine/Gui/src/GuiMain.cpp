#include <iostream>
#include <stdint.h>
#include <GL/glew.h>
#include <glfw3.h>

#include "Engine.h"
#include "Core/ResourceManagerCore/Pool/PoolBase.h"
#include "Core/ResourceManagerCore/Policy/MeshAllocationPolicy.h"
#include "Core/GameCore/GlobalProperties.h"
#include "GuiMain.h"
#include "Core/GameCore/KeyboardInputManager.h"

#include "Core/GameCore/Level.h"

using namespace Game;

bool bMouseMove = false;
bool bKeyDown = false;
bool bPushFrame = false;
bool bShaderRecompile = false;

void get_window_pos(GLFWwindow* window)
{
	int32_t x, y;
	glfwGetWindowPos(window, &x, &y);

	GlobalProperties::GetInstance()->GetInputData().SetWindowPos(x, y);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	int32_t xPos = static_cast<int32_t>(xpos);
	int32_t yPos = static_cast<int32_t>(ypos);

	GlobalProperties::GetInstance()->GetInputData().SetMouseX(xPos);
	GlobalProperties::GetInstance()->GetInputData().SetMouseY(yPos);

	bMouseMove = true;
}

/*! @brief The function signature for keyboard key callbacks.
 *
 *  This is the function signature for keyboard key callback functions.
 *
 *  @param[in] window The window that received the event.
 *  @param[in] key The [keyboard key](@ref keys) that was pressed or released.
 *  @param[in] scancode The system-specific scancode of the key.
 *  @param[in] action `GLFW_PRESS`, `GLFW_RELEASE` or `GLFW_REPEAT`.
 *  @param[in] mods Bit field describing which [modifier keys](@ref mods) were
 *  held down.
 *
 *  @sa @ref input_key
 *  @sa glfwSetKeyCallback
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle, scancode and modifier mask parameters.
 *
 *  @ingroup input
 */

void key_pressed_callback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t actionType, int32_t modifierKey)
{
   if (actionType == GLFW_PRESS)
   {
      if (key == 'w' || key == 'W')
      {
         bKeyDown = true;
      }
      else if (key == 'P' || key == 'p')
      {
         bPushFrame = true;
      }
      else if (key == 'R' || key == 'p')
      {
         bShaderRecompile = true;
      }

      KeyboardInputManager::GetInstance()->PushKeyEvent((Keys)key, KEY_PRESSED);

   }
   else if (actionType == GLFW_RELEASE)
   {
      KeyboardInputManager::GetInstance()->PushKeyEvent((Keys)key, KEY_RELEASED);
      bKeyDown = false;
      bPushFrame = false;
   }
}

void get_screen_rezolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	GlobalProperties::GetInstance()->GetInputData().SetScreenWidth(mode->width);
	GlobalProperties::GetInstance()->GetInputData().SetScreenHeight(mode->height);
}

void get_window_size(GLFWwindow* window)
{
	int32_t width, height;
	glfwGetWindowSize(window, &width, &height);

	GlobalProperties::GetInstance()->GetInputData().SetWindowWidth(width);
	GlobalProperties::GetInstance()->GetInputData().SetWindowHeight(height);
}

int32_t main(int32_t argc, char** argv)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */

   get_screen_rezolution();

   auto width = GlobalProperties::GetInstance()->GetInputData().GetScreenWidth();
   auto height = GlobalProperties::GetInstance()->GetInputData().GetScreenHeight();

	// window = glfwCreateWindow(width, height, "PHEngine", NULL, NULL);
   window = glfwCreateWindow(1200, 900, "PHEngine", NULL, NULL);
   
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	get_window_size(window);
	get_window_pos(window);

	glfwSetCursorPosCallback(window, cursor_position_callback);
   glfwSetKeyCallback(window, key_pressed_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	GLenum initResult = glewInit();
	if (initResult != GLEW_OK)
	{
		std::cout << "Something is wrong" << std::endl;
		glfwTerminate();
	}

	Engine engine;
   engine.PlayLevel(std::make_shared<Level>(engine.GetThreadCommunicationManager()));

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		engine.TickWindow();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		if (bMouseMove)
		{
			engine.MouseMove();
			bMouseMove = false;
		}
      if (bKeyDown)
      {
         engine.KeyDown();
      }
      if (bPushFrame)
      {
#if DEBUG
         engine.PushFrame();
#endif
         bPushFrame = false;
      }
      if (bShaderRecompile)
      {
#ifdef DEBUG
         engine.RecompileAllShaders();
#endif
         bShaderRecompile = false;
      }
	}

	glfwTerminate();

	std::cin.get();

	return 0;
}