#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Scene.h"

#include <string>

using namespace Thread;

namespace EngineCore {
class Level : public ITickable {
protected:
    std::weak_ptr<Scene> mSceneWp;

    std::string mLevelName;

public:
    Level(const std::string& levelName);

    virtual ~Level();

    std::string GetLevelName() const;

    void SetScene(const std::shared_ptr<Scene>& scene);

    void PostPhysicsInitialize();

    std::weak_ptr<Scene> GetSceneWP() const;

    virtual void PreLevelInit();

    virtual void InitLevel();

    virtual void PostLevelInit();

    virtual void PostPlayLevelFinished();

    virtual void UnloadLevel() = 0;

    void Tick(const float deltaTime) override;

    void UnpausableTick(const float deltaTime) override;

    virtual void RestartLuaScripts();
};
} // namespace EngineCore
