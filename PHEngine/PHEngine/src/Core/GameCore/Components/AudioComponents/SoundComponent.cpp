#include "SoundComponent.h"

#include "Core/AudioCore/SoundBuffer.h"
#include "Core/AudioCore/SoundSource.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/ResourceManagerCore/Pool/SoundBufferPool.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

using namespace Resources;

namespace EngineCore {
SoundComponent::SoundComponent(const std::shared_ptr<ComponentData>& data)
    : Component(data->EngineObjectName)
    , mSoundBuffersMap()
    , mSoundSource(std::make_shared<SoundSource>())
{
}

SoundComponent::~SoundComponent()
{
    Component::Initialize();

    Event::GeneralSystemSettingsChangedGameThreadEvent::GetInstance()->RemoveListener(
        Event::GeneralSystemSettingsChangedGameThreadEvent::GetInstanceId());
}

void SoundComponent::Initialize()
{
    Component::Initialize();

    Event::GeneralSystemSettingsChangedGameThreadEvent::GetInstance()->AddListener(
        std::dynamic_pointer_cast<Event::GeneralSystemSettingsChangedGameThreadEvent>(shared_from_this()));
}

void SoundComponent::CleanUp()
{
    for (const auto& [key, bufferValue] : mSoundBuffersMap) {
        SoundBufferPool::GetInstance()->TryToFreeMemory(bufferValue);
    }
    mSoundBuffersMap.clear();
    mSoundSource->CleanUp();
    mSoundSource = nullptr;
}

void SoundComponent::Tick(const float deltaTimeSec)
{
}

eComponentType SoundComponent::GetComponentType() const
{
    return AUDIO_COMPONENT;
}

void SoundComponent::ProcessEvent(
    const Event::GeneralSystemSettingsChangedGameThreadEvent* senderPtr,
    const Event::GeneralSystemSettingsChangedGameThreadEvent::EventData_t& data)
{
    if (std::get<0>(data) == Event::eSystemSettingsEventType::SOUND_SETTINGS_CHANGED) {
        const auto& jsonParameters = std::get<1>(data);
        const auto& jsonObj = nlohmann::json::parse(jsonParameters);

        if (jsonObj.contains("action")) {
            const auto& doneAction = jsonObj["action"].get<std::string>();
            if ("change_value" == doneAction) {
                if (jsonObj.contains("gain")) {
                    const float gain = nlohmann_utilities::GetFloatFromJson(jsonObj, "gain");
                    assert(gain >= 0.0f && gain <= 1.0f);
                    SetGain(gain);
                }
            }
        }
    }
}

void SoundComponent::CreateSoundBuffer(const std::string& soundFileName, const std::string& bufferName)
{
    assert(!mSoundBuffersMap.count(bufferName));
    const auto& buffer = SoundBufferPool::GetInstance()->GetOrAllocateResource(soundFileName);
    mSoundBuffersMap.emplace(bufferName, buffer);
}

std::shared_ptr<SoundBuffer> SoundComponent::GetSoundBufferByName(const std::string& soundName) const
{
    return mSoundBuffersMap.count(soundName) ? mSoundBuffersMap.at(soundName) : nullptr;
}

std::shared_ptr<SoundSource> SoundComponent::GetSoundSource() const
{
    return mSoundSource;
}

void SoundComponent::PlayBuffer(const std::string& soundName)
{
    assert(mSoundBuffersMap.count(soundName));
    mSoundSource->Play(mSoundBuffersMap.at(soundName));
}

void SoundComponent::SetGain(const float gain)
{
    assert(gain >= 0.0f && gain <= 1.0f);
    mSoundSource->SetGain(gain);
}
} // namespace EngineCore