#pragma once

#include "Core/GameCore/ILevelPrefabFactory.h"

using namespace EngineCore;

namespace TestFeatures {

class TestFeaturesPrefabLevelFactory : public ILevelPrefabFactory {
public:
    std::shared_ptr<Level> CreateLevel(const std::string& levelPrefabName, const std::string& levelDataJson) const override;
};

} // namespace TestFeatures
