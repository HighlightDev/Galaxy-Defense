#pragma once
#include "EngineObjectProperty.h"

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>

namespace EngineCore {
class EngineObject {
    static int32_t mTotalObjectIdCounter;

protected:
    int32_t mObjectId;

    std::unordered_map<std::string, std::weak_ptr<EngineObjectPropertyBase>> mEngineProperties;

    std::string EngineObjectName;

public:
    virtual ~EngineObject() = default;

    EngineObject(const std::string& gameObjectName);

    int32_t GetObjectId() const;

    const std::weak_ptr<EngineObjectPropertyBase>& GetEnginePropertyByName(const std::string& key) const;

    void AddEngineProperty(const std::shared_ptr<EngineObjectPropertyBase>& goPtr);

    std::string GetEngineObjectName() const;
};

} // namespace EngineCore
