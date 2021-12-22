#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <iostream>
#include <stdint.h>

#include "Engine.h"
#include "Core/GameCore/Input/InputManager.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/ResourceManagerCore/Policy/MeshAllocationPolicy.h"
#include "Core/ResourceManagerCore/Pool/PoolBase.h"
#include "src/LabyrinthLevelFactory.h"

#include <TinyLogger/LogInterface.h>

using namespace Game;
using namespace Labyrinth;
using namespace IO;

bool bPushFrame = false;
bool bShaderRecompile = false;
bool bSerializeLevel = false;
bool bDeserializeLevel = false;

bool bPollEvents = true;

static std::shared_ptr<InputManager> engineInputManager = nullptr;

void get_window_pos(GLFWwindow *window) {
  int32_t x, y;
  glfwGetWindowPos(window, &x, &y);

  DisplayDeviceDataProvider::GetInstance()->SetWindowPos(x, y);
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

    else if (key == 'M' || key == 'm') {
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
  // TinyLogger::LogProxy::InitLog(new TinyLogger::LoggerClientConsole());
  TinyLogger::LogProxy::InitLog(new TinyLogger::LoggerClientFile());
  TinyLogger::LogProxy::StartLogThread();

  GLFWwindow *window;
  /* Initialize the library */
  if (!glfwInit())
    return -1;

  /* Create a windowed mode window and its OpenGL context */
  get_screen_rezolution();
  auto width = DisplayDeviceDataProvider::GetInstance()->GetScreenWidth();
  auto height = DisplayDeviceDataProvider::GetInstance()->GetScreenHeight();

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

  {
    InterThreadCommunicationMgr threadManager;
    Engine engine(threadManager);
    engineInputManager = engine.GetInputManager();
    const auto &level = LabyrinthLevelFactory::GetInstance()->CreateLevel(
        "test level", threadManager);

    engine.PlayLevel(level);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window) && bPollEvents)
    {
      engine.TickWindow();
      /* Swap front and back buffers */
      glfwSwapBuffers(window);
      /* Poll for and process events */
      glfwPollEvents();
#ifdef DEBUG
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

    engine.StopGameThreadExecution();
  }

  glfwTerminate();

  return 0;
}