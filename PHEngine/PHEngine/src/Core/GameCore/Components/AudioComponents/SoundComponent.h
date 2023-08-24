#pragma once

#include <unordered_map>
#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Core/GameCore/Components/Component.h"

namespace EngineCore
{
    struct ComponentData;
    class SoundBuffer;
    class SoundSource;

    class SoundComponent
        : public Component
    {
    protected:
        std::unordered_map<std::string, std::shared_ptr<SoundBuffer>> mSoundBuffersMap;

        std::shared_ptr<SoundSource> mSoundSource;

    public:
        SoundComponent(const std::shared_ptr<ComponentData> &data);

        ~SoundComponent() override;

        void CleanUp() override;

        void Tick(const float deltaTime) override;

        void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

        eComponentType GetComponentType() const override;

        void CreateSoundBuffer(const std::string& soundFileName, const std::string& bufferName);

        std::shared_ptr<SoundBuffer> GetSoundBufferByName(const std::string& soundName) const;

        std::shared_ptr<SoundSource> GetSoundSource() const;

        virtual void PlayBuffer(const std::string& soundName);
    };
}