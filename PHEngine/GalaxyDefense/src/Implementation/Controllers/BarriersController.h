#pragma once

#include <glm/vec3.hpp>
#include <stdint.h>

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace EngineCore {
class Scene;
}

namespace Graphics {
class IMaterial;
namespace Texture {
class ITexture;
}
} // namespace Graphics

namespace Game {
class BarrierActor;

class BarriersController {
    int32_t mBarrierActorsCount{0};

    int32_t mCurrentBarrierPillarsCount{0};

    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    std::unordered_map<std::string, std::shared_ptr<::Game::BarrierActor>> mBarrierActors;

    std::shared_ptr<::Graphics::IMaterial /*ray material*/> mRayMaterial;

    std::shared_ptr<::Graphics::IMaterial /*barrier material*/> mBarrierMaterial;

    int32_t mBarrierActorCounter{0};

    std::string mCurrentActiveBarrierName;

public:
    explicit BarriersController(const std::weak_ptr<::EngineCore::Scene>& sceneWp);

    void OnPostLevelInit();

    void SelectNewBarrier(const std::string& barrierName);

    void CreateNewBarrierPillar(const glm::vec3& position);
};
} // namespace Game
