#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <stdint.h>
#include <TinyLogger/LogInterface.h>

#include "src/GameLevelFactory.h"
#include "Core/GameCore/Input/InputManager.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/ResourceManagerCore/Policy/MeshAllocationPolicy.h"
#include "Core/ResourceManagerCore/Pool/PoolBase.h"
#include "Core/GameCore/Input/MouseEventEnums.h"
#include "Engine.h"

using namespace EngineCore;
using namespace Game;
using namespace IO;
using namespace TinyLogger;

bool bPushFrame = false;
bool bShaderRecompile = false;
bool bSerializeLevel = false;
bool bDeserializeLevel = false;

bool bPollEvents = true;
bool bShowCursor = true;
bool bMouseButtonPressed = false;

static std::shared_ptr<InputManager> engineInputManager = nullptr;

void get_window_pos(GLFWwindow *window)
{
  int32_t x, y;
  glfwGetWindowPos(window, &x, &y);

  DisplayDeviceDataProvider::GetInstance()->SetWindowPos(x, y);
}

void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  if (std::abs(yoffset) > 0.0000001)
  {
    engineInputManager->TriggeOnMouseScroll(yoffset < -0.00001 ? eMouseScrollDirection::ZoomOut : yoffset > 0.00001 ? eMouseScrollDirection::ZoomIn
                                                                                                                    : eMouseScrollDirection::Undefined);
  }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
  /*  @param[in] button The [mouse button](@ref buttons) that was pressed or
   *  released.
   *  @param[in] action One of `GLFW_PRESS` or `GLFW_RELEASE`.  Future releases
   *  may add more actions.
   * */

  if (GLFW_MOUSE_BUTTON_1 == button)
  {
    if (GLFW_PRESS == action)
    {
    }
    else if (GLFW_RELEASE == action)
    {
    }
  }
  else if (GLFW_MOUSE_BUTTON_2 == button)
  {
    if (GLFW_PRESS == action)
    {
      bMouseButtonPressed = true;
      bShowCursor = !bShowCursor;
    }
    else if (GLFW_RELEASE == action)
    {
      bMouseButtonPressed = false;
    }
  }
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
  const int32_t xPos = static_cast<int32_t>(xpos);
  const int32_t yPos = static_cast<int32_t>(ypos);
  engineInputManager->TriggerOnMouseMove(xPos, yPos);
}

#define ESCAPE_KEY 256

void key_pressed_callback(GLFWwindow *window, int32_t key, int32_t scancode,
                          int32_t actionType, int32_t modifierKey)
{
  if (actionType == GLFW_PRESS)
  {
    if (key == 'P' || key == 'p')
    {
      bPushFrame = true;
    }
    else if (key == 'R' || key == 'p')
    {
      bShaderRecompile = true;
    }

    else if (key == ESCAPE_KEY)
    {
      bPollEvents = false;
    }

    else if (key == 'M' || key == 'm')
    {
      bSerializeLevel = true;
    }

    else if (key == 'N' || key == 'n')
    {
      bDeserializeLevel = true;
    }

    engineInputManager->TriggerOnKeyDown((Keys)key);
  }

  else if (actionType == GLFW_RELEASE)
  {
    engineInputManager->TriggerOnKeyUp((Keys)key);
    bPushFrame = false;
  }
}

void get_screen_rezolution()
{
  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

  DisplayDeviceDataProvider::GetInstance()->SetScreenWidth(mode->width);
  DisplayDeviceDataProvider::GetInstance()->SetScreenHeight(mode->height);
}

void get_window_size(GLFWwindow *window)
{
  int32_t width, height;

  glfwGetWindowSize(window, &width, &height);

  DisplayDeviceDataProvider::GetInstance()->SetWindowWidth(width);
  DisplayDeviceDataProvider::GetInstance()->SetWindowHeight(height);
}

int32_t main(int32_t argc, char **argv)
{
  FolderManager::GetInstance()->BuildSystemPathToFolders();

  Logger::InitLog(new LoggerClientConsole());
  Logger::StartLogThread();

  GLFWwindow *window;
  // Initialize the library
  if (!glfwInit())
    return -1;

  // Create a windowed mode window and its OpenGL context
  get_screen_rezolution();
  // auto width = DisplayDeviceDataProvider::GetInstance()->GetScreenWidth();
  // auto height = DisplayDeviceDataProvider::GetInstance()->GetScreenHeight();

  auto width = 1200;
  auto height = 900;
  window = glfwCreateWindow(width, height, "PHEngine", NULL, NULL);
  Logger::Out("main => glfwWindow create with size: width = ", width, " height = ", height);

  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  // Make the window's context current
  glfwMakeContextCurrent(window);
  get_window_size(window);
  get_window_pos(window);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetScrollCallback(window, mouse_scroll_callback);
  glfwSetKeyCallback(window, key_pressed_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  GLenum initResult = glewInit();

  if (initResult != GLEW_OK)
  {
    Logger::Out("main => GLEW is NOK");
    glfwTerminate();
  }

  {
    InterThreadCommunicationMgr threadManager;
    Engine engine(threadManager);
    engineInputManager = engine.GetInputManager();
    const auto &level = GameLevelFactory::GetInstance()->CreateLevel(
        "test level", threadManager);

    engine.PlayLevel(level);
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window) && bPollEvents)
    {
      engine.TickWindow();
      // Swap front and back buffers
      glfwSwapBuffers(window);
      // Poll for and process events
      glfwPollEvents();
#ifdef DEBUG

      if (bMouseButtonPressed)
      {
        glfwSetInputMode(window, GLFW_CURSOR, bShowCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
      }

      if (bPushFrame)
      {
        engine.PushFrame();
        bPushFrame = false;
      }

      if (bShaderRecompile)
      {
        engine.RecompileAllShaders();
        bShaderRecompile = false;
      }

#endif
      if (bSerializeLevel)
      {
        level->SerializeLevel("test_serialize.xml");
        bSerializeLevel = false;
      }

      if (bDeserializeLevel)
      {
        level->DeserializeLevel("test_serialize.xml");
        bDeserializeLevel = false;
      }
    }

    engine.CleanUp();
  }

  glfwTerminate();

  Logger::StopLogThread(); // join logger thread

  return 0;
}