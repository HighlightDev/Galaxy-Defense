#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Scene.h"

#include <memory>

using namespace EngineCore;

namespace Game {
class AiBrain : public ITickable {
    std::weak_ptr<Scene> mScene;

public:
    AiBrain(const std::weak_ptr<Scene>& scene);

    virtual void PreInit();

    virtual void PostInit();

    void Tick(const float deltaTime) override;

    void UnpausableTick(const float deltaTime) override { };
};
} // namespace Game