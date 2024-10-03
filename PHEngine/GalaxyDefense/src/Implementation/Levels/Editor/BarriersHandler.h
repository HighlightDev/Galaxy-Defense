#pragma once

#include <stdint.h>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <unordered_map>
#include <glm/vec3.hpp>

namespace EngineCore
{
    class Scene;
}

namespace Graphics
{
    class IMaterial;
    namespace Texture
    {
        class ITexture;
    }
}

namespace Game
{
    class BarrierActor;

    class BarriersHandler
    {
        int32_t mBarrierActorsCount{0};
        
        int32_t mCurrentBarrierPillarsCount{0};

        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        std::unordered_map<std::string, std::shared_ptr<::Game::BarrierActor>> mBarrierActors;

        std::unordered_map<std::string, std::pair<std::shared_ptr<::Graphics::IMaterial> /*pillar material*/, std::shared_ptr<::Graphics::IMaterial /*ray material*/>>> mBarrierMaterials;

        int32_t mBarrierActorCounter{0};

        std::string mCurrentActiveBarrierName;

        std::shared_ptr<::Graphics::Texture::ITexture> mNoiseTex;

    public:
        explicit BarriersHandler(const std::weak_ptr<::EngineCore::Scene> &sceneWp);

        void OnPostLevelInit();

        void SelectNewBarrier(const std::string &barrierName);

        void CreateNewBarrierPillar(const glm::vec3 &position,
                                    const glm::vec3 &scale);

        void SetBarrierColor(const glm::vec3 &currentBarrierColor);

        void SetRayColor(const glm::vec3& currentRayColor);
    };
}
