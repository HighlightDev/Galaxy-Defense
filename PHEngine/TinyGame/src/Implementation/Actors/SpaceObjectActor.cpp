#include "SpaceObjectActor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"

namespace Game
{
    SpaceObjectActor::SpaceObjectActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : Actor(gameObjectName, rootComponent)
    {
    }

    bool SpaceObjectActor::IsInsideLevel(const BoundingBox &boundingBox) const
    {
        return EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), GetRootComponent()->GetTranslation());
    }

    eSpaceObjectActivityState SpaceObjectActor::GetActivityState() const
    {
        return mActivityState;
    }
}