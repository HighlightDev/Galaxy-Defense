#pragma once

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Event/GeneralSystemSettingsChangedEvent.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>

namespace EngineCore {
struct ComponentData;
class StreamingSoundSource;

class StreamingSoundComponent : public Component, public Event::GeneralSystemSettingsChangedGameThreadEvent {
protected:
    std::shared_ptr<StreamingSoundSource> mStreamingSoundSource;

public:
    StreamingSoundComponent(const std::shared_ptr<ComponentData>& data);

    ~StreamingSoundComponent() override;

    void OnRegistered() override;

    void CleanUp() override;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    eComponentType GetComponentType() const override;

    void ProcessEvent(
        const Event::GeneralSystemSettingsChangedGameThreadEvent* senderPtr,
        const Event::GeneralSystemSettingsChangedGameThreadEvent::EventData_t& data) override;

    void CreateStreamingSoundSource(const std::string& soundFileName);

    std::shared_ptr<StreamingSoundSource> GetStreamingSoundSource() const;

    virtual void PlayStream();

    void SetIsLoopSound(const bool isLoopSound);

    void SetGain(const float gain);
};
} // namespace EngineCore