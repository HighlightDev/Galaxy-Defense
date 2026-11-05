#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Scene.h"

#include <string>

using namespace Thread;

namespace EngineCore {
class Level : public ITickable {
protected:
    std::weak_ptr<Scene> mSceneWp;

    std::string mPrefabLevelName;

    std::string mLevelDataJson;

public:
    Level(const std::string& levelPrefabName, const std::string& levelDataJson);

    virtual ~Level();

    std::string GetPrefabLevelName() const;

    std::string GetLevelDataJson() const;

    void SetScene(const std::shared_ptr<Scene>& scene);

    void PostPhysicsInitialize();

    std::weak_ptr<Scene> GetSceneWP() const;

    virtual void PreLevelInit();

    virtual void InitLevel();

    virtual void PostLevelInit();

    virtual void PostPlayLevelFinished();

    virtual void UnloadLevel() = 0;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override;

    virtual void RestartLuaScripts();
};
} // namespace EngineCore
