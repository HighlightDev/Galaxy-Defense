#include "StreamingSoundComponent.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/AudioCore/StreamingSoundSource.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"

namespace EngineCore
{
    StreamingSoundComponent::StreamingSoundComponent(const ComponentData &data)
        : Component(data.GameObjectName),
          mStreamingSoundSource()
    {
    }

    StreamingSoundComponent::~StreamingSoundComponent()
    {
    }

    void StreamingSoundComponent::Tick(const float deltaTime)
    {
        mStreamingSoundSource->UpdateBufferStream();
    }

    void StreamingSoundComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
    {
    }

    eComponentType StreamingSoundComponent::GetComponentType() const
    {
        return AUDIO_COMPONENT;
    }

    void StreamingSoundComponent::CreateStreamingSoundSource(const std::string &soundFileName)
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
}