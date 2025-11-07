#include "StreamingSoundComponent.h"

#include "Core/AudioCore/StreamingSoundSource.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

namespace EngineCore {
StreamingSoundComponent::StreamingSoundComponent(const std::shared_ptr<ComponentData>& data)
    : Component(data->EngineObjectName)
    , mStreamingSoundSource()
{
}

StreamingSoundComponent::~StreamingSoundComponent()
{
    Event::GeneralSystemSettingsChangedGameThreadEvent::GetInstance()->RemoveListener(
        Event::GeneralSystemSettingsChangedGameThreadEvent::GetInstanceId());
}

void StreamingSoundComponent::Initialize()
{
    Component::Initialize();
    
    Event::GeneralSystemSettingsChangedGameThreadEvent::GetInstance()->AddListener(
        std::dynamic_pointer_cast<Event::GeneralSystemSettingsChangedGameThreadEvent>(shared_from_this()));
}

void StreamingSoundComponent::CleanUp()
{
    if (mStreamingSoundSource) {
        mStreamingSoundSource->CleanUp();
        mStreamingSoundSource = nullptr;
    }
}

void StreamingSoundComponent::Tick(const float deltaTimeSec)
{
    mStreamingSoundSource->UpdateBufferStream();
}

eComponentType StreamingSoundComponent::GetComponentType() const
{
    return AUDIO_COMPONENT;
}

void StreamingSoundComponent::ProcessEvent(
    const Event::GeneralSystemSettingsChangedGameThreadEvent* senderPtr,
    const Event::GeneralSystemSettingsChangedGameThreadEvent::EventData_t& data)
{
    if (std::get<0>(data) == Event::eSystemSettingsEventType::MUSIC_SETTINGS_CHANGED) {
        const auto& jsonParameters = std::get<1>(data);
        const auto& jsonObj = nlohmann::json::parse(jsonParameters);

        if (jsonObj.contains("action")) {
            const auto& doneAction = jsonObj["action"].get<std::string>();
            if ("change_value" == doneAction) {
                if (jsonObj.contains("gain")) {
                    const float gain = nlohmann_utilities::GetFloatFromJson(jsonObj["gain"]);
                    assert(gain >= 0.0f && gain <= 1.0f);
                    SetGain(gain);
                }
            }
        }
    }
}

void StreamingSoundComponent::CreateStreamingSoundSource(const std::string& soundFileName)
{
    mStreamingSoundSource = std::make_shared<StreamingSoundSource>(soundFileName);
}

std::shared_ptr<StreamingSoundSource> StreamingSoundComponent::GetStreamingSoundSource() const
{
    return mStreamingSoundSource;
}

void StreamingSoundComponent::PlayStream()
{
    mStreamingSoundSource->Play();
}

void StreamingSoundComponent::SetIsLoopSound(const bool isLoopSound)
{
    mStreamingSoundSource->SetIsLoopSound(isLoopSound);
}

void StreamingSoundComponent::SetGain(const float gain)
{
    mStreamingSoundSource->SetGain(gain);
}
} // namespace EngineCore