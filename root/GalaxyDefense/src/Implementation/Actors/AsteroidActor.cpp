#include "AsteroidActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/MovementComponent.h"

namespace Game {
AsteroidActor::AsteroidActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent)
    : SpaceObjectActor(gameObjectName, rootComponent)

{
}

} // namespace Game