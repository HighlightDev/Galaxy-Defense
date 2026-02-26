#include "GeneralSystemSettingsDataProvider.h"

#include "Core/GameCore/Event/GeneralSystemSettingsChangedEvent.h"
#include "Core/GameCore/Event/WindowSizeChangedEvent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

namespace EngineCore::DataProviders {

GeneralSystemSettingsDataProvider::GeneralSystemSettingsDataProvider()
{
}

void GeneralSystemSettingsDataProvider::Initialize()
{
    Event::BroadcastGameThreadEvent::GetInstance()->AddListener(
        std::static_pointer_cast<Event::BroadcastGameThreadEvent>(shared_from_this()));
}

GeneralSystemSettingsDataProvider::~GeneralSystemSettingsDataProvider()
{
    Event::BroadcastGameThreadEvent::GetInstance()->RemoveListener(Event::BroadcastGameThreadEvent::GetInstanceId());
}

void GeneralSystemSettingsDataProvider::SetOpenGLMajorVersion(const float version)
{
    m_openglMajorVersion = version;
    LogInfo("GeneralSystemSettingsDataProvider::SetOpenGLMajorVersion: version: ", version);
}

void GeneralSystemSettingsDataProvider::SetOpenGLMinorVersion(const float version)
{
    m_openglMinorVersion = version;
    LogInfo("GeneralSystemSettingsDataProvider::SetOpenGLMinorVersion: version: ", version);
}

float GeneralSystemSettingsDataProvider::GetOpenGLMajorVersion() const
{
    return m_openglMajorVersion;
}

float GeneralSystemSettingsDataProvider::GetOpenGLMinorVersion() const
{
    return m_openglMinorVersion;
}

float GeneralSystemSettingsDataProvider::GetOpenGLVersion() const
{
    return m_openglMajorVersion + (m_openglMinorVersion * 0.1f);
}

const std::shared_ptr<GeneralSystemSettingsDataProvider>& GeneralSystemSettingsDataProvider::GetInstance()
{
    static const std::shared_ptr<GeneralSystemSettingsDataProvider>& settings
        = std::make_shared<GeneralSystemSettingsDataProvider>();
    std::call_once(settings->m_initFlag, [_settings = settings]() { settings->Initialize(); });
    return settings;
}

void GeneralSystemSettingsDataProvider::ProcessEvent(
    const BroadcastGameThreadEvent* sender, const typename Event::BroadcastGameThreadEvent::EventData_t& eventData)
{
    const auto& eventHeader = std::get<0>(eventData);
    const auto& jsonParametersStr = std::get<1>(eventData);
    if ("GeneralSystemSettingsEvents" == eventHeader) {
        const auto& jsonObj = nlohmann::json::parse(jsonParametersStr);
        if (jsonObj.contains("action")) {
            const auto& doneAction = jsonObj["action"].get<std::string>();
            if ("change_sound" == doneAction) {
                if (jsonObj.contains("gain")) {
                    SetSoundGain(nlohmann_utilities::GetFloatFromJson(jsonObj, "gain"), false);
                }
            } else if ("change_music" == doneAction) {
                if (jsonObj.contains("gain")) {
                    SetMusicGain(nlohmann_utilities::GetFloatFromJson(jsonObj, "gain"), false);
                }
            }
        }
    }
}

void GeneralSystemSettingsDataProvider::SetWindowSize(const int32_t width, const int32_t height)
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

void GeneralSystemSettingsDataProvider::SetWindowWidth(const int32_t width)
{
    if (m_windowWidth != width) {
        m_windowWidth = width;
        Event::WindowSizeChangedGameThreadEvent::GetInstance()->SendEvent(
            Event::eExecutionOrder::POST_EXECUTION, ViewPortInfo(0, 0, m_windowWidth, m_windowHeight));
        Event::WindowSizeChangedLuaThreadEvent::GetInstance()->SendEvent(
            Event::eExecutionOrder::POST_EXECUTION, ViewPortInfo(0, 0, m_windowWidth, m_windowHeight));
    }
}

void GeneralSystemSettingsDataProvider::SetWindowHeight(const int32_t height)
{
    if (m_windowHeight != height) {
        m_windowHeight = height;
        Event::WindowSizeChangedGameThreadEvent::GetInstance()->SendEvent(
            Event::eExecutionOrder::POST_EXECUTION, ViewPortInfo(0, 0, m_windowWidth, m_windowHeight));
        Event::WindowSizeChangedLuaThreadEvent::GetInstance()->SendEvent(
            Event::eExecutionOrder::POST_EXECUTION, ViewPortInfo(0, 0, m_windowWidth, m_windowHeight));
    }
}

void GeneralSystemSettingsDataProvider::SetSoundGain(const float gain, const bool bSendToLua)
{
    ext_assert(gain >= 0.0f && gain <= 1.0f, "gain must be between 0.0 and 1.0");
    if (!EngineMath::FloatsNearEqual(m_soundGain, gain)) {
        m_soundGain = gain;
        LogInfo("GeneralSystemSettingsDataProvider::SetSoundGain: gain: ", gain, " SendToLua: ", bSendToLua);
        nlohmann::json jsonObj;
        jsonObj["action"] = "change_value";
        jsonObj["gain"] = m_soundGain;
        Event::GeneralSystemSettingsChangedGameThreadEvent::GetInstance()->SendEvent(
            Event::eExecutionOrder::POST_EXECUTION, Event::eSystemSettingsEventType::SOUND_SETTINGS_CHANGED, jsonObj.dump());
        if (bSendToLua) {
            Event::GeneralSystemSettingsChangedLuaThreadEvent::GetInstance()->SendEvent(
                Event::eExecutionOrder::POST_EXECUTION, Event::eSystemSettingsEventType::SOUND_SETTINGS_CHANGED, jsonObj.dump());
        }
    }
}

void GeneralSystemSettingsDataProvider::SetMusicGain(const float gain, const bool bSendToLua)
{
    ext_assert(gain >= 0.0f && gain <= 1.0f, "gain must be between 0.0 and 1.0");
    if (!EngineMath::FloatsNearEqual(m_musicGain, gain)) {
        m_musicGain = gain;
        LogInfo("GeneralSystemSettingsDataProvider::SetMusicGain: gain: ", gain, " SendToLua: ", bSendToLua);
        nlohmann::json jsonObj;
        jsonObj["action"] = "change_value";
        jsonObj["gain"] = m_musicGain;
        Event::GeneralSystemSettingsChangedGameThreadEvent::GetInstance()->SendEvent(
            Event::eExecutionOrder::POST_EXECUTION, Event::eSystemSettingsEventType::MUSIC_SETTINGS_CHANGED, jsonObj.dump());
        if (bSendToLua) {
            Event::GeneralSystemSettingsChangedLuaThreadEvent::GetInstance()->SendEvent(
                Event::eExecutionOrder::POST_EXECUTION, Event::eSystemSettingsEventType::MUSIC_SETTINGS_CHANGED, jsonObj.dump());
        }
    }
}
} // namespace EngineCore::DataProviders
