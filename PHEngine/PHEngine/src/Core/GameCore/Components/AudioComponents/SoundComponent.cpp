#include "SoundComponent.h"

#include "Core/AudioCore/SoundBuffer.h"
#include "Core/AudioCore/SoundSource.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/ResourceManagerCore/Pool/SoundBufferPool.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

using namespace Resources;

namespace EngineCore {
SoundComponent::SoundComponent(const std::shared_ptr<ComponentData>& data)
    : Component(data->EngineObjectName)
    , mSoundBuffersMap()
    , mSoundSource(std::make_shared<SoundSource>())
{
#ifdef DEBUG
    if (not EngineUtility::EngineConfigHolder::GetInstance()->GetEngineConfig().EnableSounds) {
        mSoundSource->SetGain(0.0f);
    }
#endif
}

SoundComponent::~SoundComponent()
{
    Event::GeneralSystemSettingsChangedGameThreadEvent::GetInstance()->RemoveListener(
        Event::GeneralSystemSettingsChangedGameThreadEvent::GetInstanceId());
}

void SoundComponent::OnRegistered()
{
    Component::OnRegistered();

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

void SoundComponent::Tick(const float deltaTimeSec, const float playSpeed)
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
                    ext_assert(gain >= 0.0f && gain <= 1.0f, "Gain value out of range in SoundComponent::ProcessEvent");
                    SetGain(gain);
                }
            }
        }
    }
}

void SoundComponent::CreateSoundBuffer(const std::string& soundFileName, const std::string& bufferName)
{
    ext_assert(!mSoundBuffersMap.count(bufferName), "Sound buffer already exists with name in SoundComponent::CreateSoundBuffer");
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
    ext_assert(mSoundBuffersMap.count(soundName), "Sound buffer does not exist with name in SoundComponent::PlayBuffer");
    mSoundSource->Play(mSoundBuffersMap.at(soundName));
}

void SoundComponent::SetGain(const float gain)
{
#ifdef DEBUG
    if (!EngineUtility::EngineConfigHolder::GetInstance()->GetEngineConfig().EnableSounds) {
        return;
    }
#endif
    ext_assert(gain >= 0.0f && gain <= 1.0f, "Gain value out of range in SoundComponent::SetGain");
    mSoundSource->SetGain(gain);
}
} // namespace EngineCore