#pragma once

#include "Core/GameCore/ILevelFactory.h"

using namespace EngineCore;

namespace TestFeatures {

class TestFeaturesLevelFactory : public ILevelFactory {
public:
    std::shared_ptr<Level> CreateLevel(const std::string& levelName) const override;
};

} // namespace TestFeatures
