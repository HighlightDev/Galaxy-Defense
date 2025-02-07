#include "EngineObject.h"

#include "Core/CommonCore/Assertion.h"

#include <cstddef>

namespace EngineCore {

int32_t EngineObject::mTotalObjectIdCounter = 0;

EngineObject::EngineObject(const std::string& gameObjectName)
    : mObjectId(mTotalObjectIdCounter++)
    , EngineObjectName(gameObjectName)
{
}

int32_t EngineObject::GetObjectId() const
{
    return mObjectId;
}

void EngineObject::AddEngineProperty(const std::shared_ptr<EngineObjectPropertyBase>& goPtr)
{
    assert((!mEngineProperties.count(goPtr->Key)));
    mEngineProperties[goPtr->Key] = goPtr;
}

const std::weak_ptr<EngineObjectPropertyBase>& EngineObject::GetEnginePropertyByName(const std::string& key) const
{
    assert((mEngineProperties.count(key)));
    return mEngineProperties.at(key);
}

std::string EngineObject::GetEngineObjectName() const
{
    return EngineObjectName;
}
} // namespace EngineCore
