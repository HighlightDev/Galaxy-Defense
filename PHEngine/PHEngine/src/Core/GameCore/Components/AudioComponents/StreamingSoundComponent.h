#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Core/GameCore/Components/Component.h"

namespace EngineCore
{
    struct ComponentData;
    class StreamingSoundSource;

    class StreamingSoundComponent
        : public Component
    {
    protected:
        std::shared_ptr<StreamingSoundSource> mStreamingSoundSource;

    public:
        StreamingSoundComponent(const std::shared_ptr<ComponentData> &data);

        ~StreamingSoundComponent() override;

        void CleanUp() override;

        void Tick(const float deltaTime) override;

        void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

        eComponentType GetComponentType() const override;

        void CreateStreamingSoundSource(const std::string &soundFileName);

        std::shared_ptr<StreamingSoundSource> GetStreamingSoundSource() const;

        virtual void PlayStream();

        void SetIsLoopSound(const bool isLoopSound);

        void SetGain(const float gain);
    };
}