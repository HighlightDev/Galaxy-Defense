#include "SpaceStationActor.h"
#include "Core/GameCore/Components/SceneComponent.h"

namespace Game
{
    SpaceStationActor::SpaceStationActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : Actor(gameObjectName, rootComponent)
    {
    }
}
