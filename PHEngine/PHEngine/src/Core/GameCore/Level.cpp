#include "Level.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/HumanoidPlayerController.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/MeshPoolParameters.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

#include <glm/vec3.hpp>

#include <algorithm>
#include <fstream>

using namespace IO;
using namespace EngineUtility;
using namespace Resources;

namespace EngineCore {

Level::Level(const std::string& levelName)
    : mLevelName(levelName)
{
}

Level::~Level()
{
}

std::string Level::GetLevelName() const
{
    return mLevelName;
}

void Level::SetScene(const std::shared_ptr<Scene>& scene)
{
    ext_assert(scene, "Level::SetScene: scene is null");
    mSceneWp = scene;
}

void Level::PreLevelInit()
{
    LogInfo("Level::PreLevelInit");
}

void Level::PostLevelInit()
{
    LogInfo("Level::PostLevelInit");
}

void Level::PostPlayLevelFinished()
{
    LogInfo("Level::PostPlayLevelFinished");
}

void Level::PostPhysicsInitialize()
{
    LogInfo("Level::PostPhysicsInitialize");
}

void Level::InitLevel()
{
    LogInfo("Level::InitLevel");
}

std::weak_ptr<Scene> Level::GetSceneWP() const
{
    return mSceneWp;
}

void Level::Tick(const float deltaTimeSec, const float playSpeed)
{
}

void Level::UnpausableTick(const float deltaTimeSec, const float playSpeed)
{
}

void Level::RestartLuaScripts()
{
}
} // namespace EngineCore
