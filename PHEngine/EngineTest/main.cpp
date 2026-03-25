#include "Core/CommonCore/EngineConstants.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/Event/ExitGameEvent.h"
#include "Core/GameCore/Input/InputManager.h"
#include "Core/GameCore/Input/MouseEventEnums.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Engine.h"
#include "src/Implementation/Levels/TestFeaturesLevelFactory.h"

#include <TinyLogger/LogInterface.h>
#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <stdint.h>

#ifdef USE_LIBUNWIND
#include "TinyUnwinder.h"

#include <stdlib.h>

#include <csignal>
#endif

using namespace EngineCore;
using namespace EngineUtility;
using namespace IO;
using namespace TinyLogger;
using namespace TestFeatures;
using namespace EngineCore::DataProviders;

bool bShaderRecompile = false;
bool bLuaScriptsRestart = false;

static std::shared_ptr<InputManager> engineInputManager = nullptr;

void get_window_pos(GLFWwindow* window)
{
    int32_t x, y;
    glfwGetWindowPos(window, &x, &y);

    GeneralSystemSettingsDataProvider::GetInstance()->SetWindowPos(x, y);
}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (std::abs(yoffset) > 0.0000001) {
        const auto direction = yoffset < 0.0 ? eMouseScrollDirection::ZoomOut : eMouseScrollDirection::ZoomIn;
        engineInputManager->TriggeOnMouseScroll(direction, static_cast<float>(yoffset));
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    /*  @param[in] button The [mouse button](@ref buttons) that was pressed or
     *  released.
     *  @param[in] action One of `GLFW_PRESS` or `GLFW_RELEASE`.  Future releases
     *  may add more actions.
     * */

    if (GLFW_PRESS == action && button == GLFW_MOUSE_BUTTON_RIGHT) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else if (GLFW_RELEASE == action && button == GLFW_MOUSE_BUTTON_RIGHT) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (GLFW_PRESS == action) {
        engineInputManager->TriggerOnMouseButtonKeyDown((eMouseKeys)button);
    } else if (GLFW_RELEASE == action) {
        engineInputManager->TriggerOnMouseButtonKeyUp((eMouseKeys)button);
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
        Event::ExitGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION);
    }

    if (actionType == GLFW_PRESS) {
        if (key == 'R' || key == 'r') {
            bShaderRecompile = true;
        }

        if (key == 'L' || key == 'l') {
            bLuaScriptsRestart = true;
        }

        const eKeyboardKeys resultKey = s_modifierKeysMap.count(key) ? s_modifierKeysMap.at(key) : (eKeyboardKeys)key;
        engineInputManager->TriggerOnKeyboardKeyDown(resultKey);
    } else if (actionType == GLFW_RELEASE) {
        const eKeyboardKeys resultKey = s_modifierKeysMap.count(key) ? s_modifierKeysMap.at(key) : (eKeyboardKeys)key;
        engineInputManager->TriggerOnKeyboardKeyUp(resultKey);
    }
}

void get_screen_rezolution(GLFWmonitor* activeMonitor)
{
    const GLFWvidmode* mode = glfwGetVideoMode(activeMonitor);

    GeneralSystemSettingsDataProvider::GetInstance()->SetScreenWidth(mode->width);
    GeneralSystemSettingsDataProvider::GetInstance()->SetScreenHeight(mode->height);
}

void get_window_size(GLFWwindow* window)
{
    int32_t width, height;

    glfwGetWindowSize(window, &width, &height);

    GeneralSystemSettingsDataProvider::GetInstance()->SetWindowWidth(width);
    GeneralSystemSettingsDataProvider::GetInstance()->SetWindowHeight(height);
}

void window_size_changed_callback(GLFWwindow* window, int width, int height)
{
    GeneralSystemSettingsDataProvider::GetInstance()->SetWindowSize(width, height);
}

void window_position_changed_callback(GLFWwindow* window, int xpos, int ypos)
{
    GeneralSystemSettingsDataProvider::GetInstance()->SetWindowPos(xpos, ypos);
}

#ifdef USE_LIBUNWIND

void handler(int sig)
{
    Tools::Unwind::TinyUnwinder unwinder;
    LogInfo("Callstack BackTrace: \n", unwinder.GetStackBacktraceStr());
    Logger::StopLogThread(); // join logger thread
    exit(1);
}
#endif

int32_t main(int32_t argc, char** argv)
{
#ifdef USE_LIBUNWIND
    std::signal(SIGSEGV, handler); // install our handler
#endif

    ThreadHelper::GetInstance()->RegisterThread(EngineConstants::c_renderThreadName);

    // Logger::InitLog(std::make_shared<LoggerClientConsole>());
    Logger::InitLog(std::make_shared<LoggerClientFile>());
    Logger::StartLogThread();

    FolderManager::GetInstance()->BuildSystemPathToFolders();
    EngineConfigHolder::GetInstance()->LoadSettings(FolderManager::GetInstance()->GetConfigPath() + "engineConfig.cfg");

    GLFWwindow* window;
    // Initialize the library
    if (!glfwInit())
        return -1;

    auto width = 1200;
    auto height = 900;
    GLFWmonitor* activeMonitor = nullptr;
    int monitorsCount = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorsCount);
    const auto& windowMode = EngineConfigHolder::GetInstance()->GetEngineConfig().WindowMode;
    const auto& preferableActiveMonitor = EngineConfigHolder::GetInstance()->GetEngineConfig().ActiveMonitor;
    if ("fullscreen" == windowMode) {
        if ("additional" == preferableActiveMonitor && monitorsCount > 1) {
            activeMonitor = monitors[1];
        } else {
            activeMonitor = glfwGetPrimaryMonitor();
        }
    } else {
        activeMonitor = glfwGetPrimaryMonitor();
    }

    get_screen_rezolution(activeMonitor);

    if ("fullscreen" == windowMode) {
        width = GeneralSystemSettingsDataProvider::GetInstance()->GetScreenWidth();
        height = GeneralSystemSettingsDataProvider::GetInstance()->GetScreenHeight();
    } else if ("windowed" == windowMode) {
        GeneralSystemSettingsDataProvider::GetInstance()->SetScreenWidth(width);
        GeneralSystemSettingsDataProvider::GetInstance()->SetScreenHeight(height);
    }

    window = glfwCreateWindow(width, height, "PHEngine", "windowed" == windowMode ? nullptr : activeMonitor, NULL);
    LogInfo("main : glfwWindow create with size: width = ", width, " height = ", height);

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
    glfwSetWindowSizeCallback(window, window_size_changed_callback);
    glfwSetWindowPosCallback(window, window_position_changed_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    GLenum initResult = glewInit();

    if (initResult != GLEW_OK) {
        LogInfo("main: GLEW is NOK");
        glfwTerminate();
    }

    {
        const auto engine = std::make_shared<Engine>();
        engine->Initialize();
        engineInputManager = engine->GetInputManager();
        engine->SetLevelFactory(std::make_shared<TestFeaturesLevelFactory>());
        engine->PlayLevel("TestFeaturesLevel");
        // Loop until the user closes the window
        while (!glfwWindowShouldClose(window) && !engine->IsExitGameState()) {
            engine->TickWindow();
            // Swap front and back buffers
            glfwSwapBuffers(window);
            // Poll for and process events
            glfwPollEvents();
#ifdef DEBUG

            if (bShaderRecompile) {
                engine->RecompileAllShaders();
                bShaderRecompile = false;
            }

            if (bLuaScriptsRestart) {
                engine->RestartLuaScripts();
                bLuaScriptsRestart = false;
            }

#endif
        }

        engine->CleanUp();
    }

    glfwTerminate();

    Logger::StopLogThread(); // join logger thread

    return 0;
}