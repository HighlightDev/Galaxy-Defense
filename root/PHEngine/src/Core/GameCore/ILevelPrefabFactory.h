#pragma once

#include <memory>
#include <string>

namespace EngineCore {

class Level;

class ILevelPrefabFactory {
public:
    virtual std::shared_ptr<Level> CreateLevel(const std::string& levelPrefabName, const std::string& levelDataJson) const = 0;
};

} // namespace EngineCore
