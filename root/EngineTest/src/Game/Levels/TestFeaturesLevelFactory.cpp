#include "TestFeaturesLevelFactory.h"

#include "Game/Levels/TestFeaturesLevel.h"

namespace TestFeatures {
std::shared_ptr<Level> TestFeaturesLevelFactory::CreateLevel(const std::string& levelName) const
{
    if ("TestFeaturesLevel" == levelName) {
        return std::make_shared<TestFeaturesLevel>();
    }
    return nullptr;
}

} // namespace TestFeatures