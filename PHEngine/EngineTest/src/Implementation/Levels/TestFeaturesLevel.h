#pragma once

#include "Core/GameCore/Level.h"
#include "Implementation/Levels/Controllers/TestFeaturesLevelUiController.h"

#include <memory>

using namespace EngineCore;

namespace EngineCore {
class Actor;
}

namespace TestFeatures {

class TestFeaturesLevel : public Level {
    using Base = Level;

    std::unique_ptr<TestFeaturesLevelUiController> mUiController;

    std::shared_ptr<Actor> mAmbientMusicDummy;

public:
    TestFeaturesLevel();

    ~TestFeaturesLevel() override;

    void InitLevel() override;

    void PreLevelInit() override;

    void PostLevelInit() override;

    void PostPlayLevelFinished() override;

    void UnloadLevel() override;

    void Tick(const float deltaTime) override;

    void UnpausableTick(const float deltaTime) override;

private:
    void CreateScene();

    void RunLuaBuildLevelScript();
};

} // namespace TestFeatures
