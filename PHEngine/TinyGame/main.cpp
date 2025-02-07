#include "Core/GameCore/Input/InputManager.h"
#include "Core/GameCore/Input/MouseEventEnums.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/ResourceManagerCore/Policy/MeshAllocationPolicy.h"
#include "Core/ResourceManagerCore/Pool/PoolBase.h"
#include "Engine.h"
#include "src/Implementation/Levels/GameLevelFactory.h"

#include <TinyLogger/LogInterface.h>
#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <stdint.h>

using namespace EngineCore;
using namespace Game;
using namespace IO;
using namespace TinyLogger;

bool bShaderRecompile = false;

bool bShowCursor = true;
bool bMouseButtonPressed = false;

static std::shared_ptr<InputManager> engineInputManager = nullptr;

void get_window_pos(GLFWwindow* window)
{
    int32_t x, y;
    glfwGetWindowPos(window, &x, &y);

    DisplayDeviceDataProvider::GetInstance()->SetWindowPos(x, y);
}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (std::abs(yoffset) > 0.0000001) {
        engineInputManager->TriggeOnMouseScroll(
            yoffset < -0.00001      ? eMouseScrollDirection::ZoomOut
                : yoffset > 0.00001 ? eMouseScrollDirection::ZoomIn
                                    : eMouseScrollDirection::Undefined);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    /*  @param[in] button The [mouse button](@ref buttons) that was pressed or
     *  released.
     *  @param[in] action One of `GLFW_PRESS` or `GLFW_RELEASE`.  Future releases
     *  may add more actions.
     * */

    if (GLFW_PRESS == action) {
        engineInputManager->TriggerOnMouseButtonKeyDown((eMouseKeys)button);
    } else if (GLFW_RELEASE == action) {
        engineInputManager->TriggerOnMouseButtonKeyUp((eMouseKeys)button);
    }

    if (GLFW_MOUSE_BUTTON_2 == button) {
        if (GLFW_PRESS == action) {
            bMouseButtonPressed = true;
            bShowCursor = !bShowCursor;
        } else if (GLFW_RELEASE == action) {
            bMouseButtonPressed = false;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    const int32_t xPos = static_cast<int32_t>(xpos);
    const int32_t yPos = static_cast<int32_t>(ypos);
    engineInputManager->TriggerOnMouseMove(xPos, yPos);
}

#define ESCAPE_KEY 256

void key_pressed_callback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t actionType, int32_t modifierKey)
{
    static const std::unordered_map<int32_t, eKeyboardKeys> s_modifierKeysMap
        = {{GLFW_KEY_LEFT_SHIFT, eKeyboardKeys::Shift},
           {GLFW_KEY_RIGHT_SHIFT, eKeyboardKeys::Shift},
           {GLFW_KEY_LEFT_CONTROL, eKeyboardKeys::Control},
           {GLFW_KEY_RIGHT_CONTROL, eKeyboardKeys::Control},
           {GLFW_KEY_LEFT_ALT, eKeyboardKeys::Alt},
           {GLFW_KEY_RIGHT_ALT, eKeyboardKeys::Alt}};

    if (key == ESCAPE_KEY) {
        key = static_cast<int32_t>(eKeyboardKeys::Escape);
    }

    if (actionType == GLFW_PRESS) {
        if (key == 'R' || key == 'p') {
            bShaderRecompile = true;
        }

        const eKeyboardKeys resultKey = s_modifierKeysMap.count(key) ? s_modifierKeysMap.at(key) : (eKeyboardKeys)key;
        engineInputManager->TriggerOnKeyboardKeyDown(resultKey);
    } else if (actionType == GLFW_RELEASE) {
        const eKeyboardKeys resultKey = s_modifierKeysMap.count(key) ? s_modifierKeysMap.at(key) : (eKeyboardKeys)key;
        engineInputManager->TriggerOnKeyboardKeyUp(resultKey);
    }
}

void get_screen_rezolution()
{
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    DisplayDeviceDataProvider::GetInstance()->SetScreenWidth(mode->width);
    DisplayDeviceDataProvider::GetInstance()->SetScreenHeight(mode->height);
}

void get_window_size(GLFWwindow* window)
{
    int32_t width, height;

    glfwGetWindowSize(window, &width, &height);

    DisplayDeviceDataProvider::GetInstance()->SetWindowWidth(width);
    DisplayDeviceDataProvider::GetInstance()->SetWindowHeight(height);
}

int32_t main(int32_t argc, char** argv)
{
    ThreadHelper::GetInstance()->RegisterThread("Render");
    FolderManager::GetInstance()->BuildSystemPathToFolders();

    // Logger::InitLog(std::make_shared<LoggerClientConsole>());
    Logger::InitLog(std::make_shared<LoggerClientFile>());
    Logger::StartLogThread();

    GLFWwindow* window;
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
    LogInfo("main => glfwWindow create with size: width = ", width, " height = ", height);

    if (!window) {
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

    if (initResult != GLEW_OK) {
        LogInfo("main => GLEW is NOK");
        glfwTerminate();
    }

    {
        const auto engine = std::make_shared<Engine>();
        engine->Initialize();
        engineInputManager = engine->GetInputManager();
        engine->SetLevelFactory(std::make_shared<GameLevelFactory>());
        engine->PlayLevel("MainMenuLevel");
        // Loop until the user closes the window
        while (!glfwWindowShouldClose(window) && !engine->IsExitGameState()) {
            engine->TickWindow();
            // Swap front and back buffers
            glfwSwapBuffers(window);
            // Poll for and process events
            glfwPollEvents();
#ifdef DEBUG

            if (bMouseButtonPressed) {
                glfwSetInputMode(window, GLFW_CURSOR, bShowCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            }

            if (bShaderRecompile) {
                engine->RecompileAllShaders();
                bShaderRecompile = false;
            }

#endif
        }

        engine->CleanUp();
    }

    glfwTerminate();

    Logger::StopLogThread(); // join logger thread

    return 0;
}