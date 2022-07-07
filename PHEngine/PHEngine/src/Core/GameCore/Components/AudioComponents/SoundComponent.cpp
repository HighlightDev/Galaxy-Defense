#include "SoundComponent.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/AudioCore/SoundBuffer.h"
#include "Core/AudioCore/SoundSource.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/ResourceManagerCore/Pool/SoundBufferPool.h"

using namespace Resources;

namespace EngineCore
{
    SoundComponent::SoundComponent(const ComponentData &data)
        : Component(data.GameObjectName),
          mSoundBuffersMap(),
          mSoundSource(std::make_shared<SoundSource>())
    {
    }

    SoundComponent::~SoundComponent()
    {
    }

    void SoundComponent::Tick(const float deltaTime)
    {
    }

    void SoundComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
    {
    }

    eComponentType SoundComponent::GetComponentType() const
    {
        return AUDIO_COMPONENT;
    }

    void SoundComponent::CreateSoundBuffer(const std::string &soundFileName, const std::string& bufferName)
    {
        assert(!mSoundBuffersMap.count(bufferName));
        const auto &buffer = SoundBufferPool::GetInstance()->GetOrAllocateResource(soundFileName);
        mSoundBuffersMap.emplace(bufferName, buffer);
    }

    std::shared_ptr<SoundBuffer> SoundComponent::GetSoundBufferByName(const std::string &soundName) const
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
}