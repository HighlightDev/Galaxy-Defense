#pragma once

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Event/GeneralSystemSettingsChangedEvent.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <unordered_map>

namespace EngineCore {
struct ComponentData;
class SoundBuffer;
class SoundSource;

class SoundComponent : public Component, public Event::GeneralSystemSettingsChangedGameThreadEvent {
protected:
    std::unordered_map<std::string, std::shared_ptr<SoundBuffer>> mSoundBuffersMap;

    std::shared_ptr<SoundSource> mSoundSource;

public:
    SoundComponent(const std::shared_ptr<ComponentData>& data);

    ~SoundComponent() override;

    void Initialize() override;

    void CleanUp() override;

    void Tick(const float deltaTimeSec) override;

    eComponentType GetComponentType() const override;

    void ProcessEvent(
        const Event::GeneralSystemSettingsChangedGameThreadEvent* senderPtr,
        const Event::GeneralSystemSettingsChangedGameThreadEvent::EventData_t& data) override;

    void CreateSoundBuffer(const std::string& soundFileName, const std::string& bufferName);

    std::shared_ptr<SoundBuffer> GetSoundBufferByName(const std::string& soundName) const;

    std::shared_ptr<SoundSource> GetSoundSource() const;

    virtual void PlayBuffer(const std::string& soundName);

    void SetGain(const float gain);
};
} // namespace EngineCore