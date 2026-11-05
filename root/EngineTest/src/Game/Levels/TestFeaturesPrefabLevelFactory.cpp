#include "TestFeaturesPrefabLevelFactory.h"

#include "Game/Levels/TestFeaturesLevel.h"

namespace TestFeatures {
std::shared_ptr<Level>
TestFeaturesPrefabLevelFactory::CreateLevel(const std::string& levelPrefabName, const std::string& levelDataJson) const
{
    if ("TestFeaturesLevel" == levelPrefabName) {
        return std::make_shared<TestFeaturesLevel>(levelPrefabName, levelDataJson);
    }
    return nullptr;
}

} // namespace TestFeatures