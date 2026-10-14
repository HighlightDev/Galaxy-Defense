#pragma once

#include <memory>
#include <string>

namespace EngineCore {

class Level;

class ILevelFactory {
public:
    virtual std::shared_ptr<Level> CreateLevel(const std::string& levelName) const = 0;
};

} // namespace EngineCore
