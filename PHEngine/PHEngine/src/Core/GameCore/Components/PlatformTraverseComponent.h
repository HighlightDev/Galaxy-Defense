#pragma once
#include "Component.h"
#include "PlatformTraverseComponentVisitor.h"

#include <optional>
#include <vector>

using namespace EnginePhysics;

namespace EngineCore {
struct PlatformTraverseComponentData;

class PlatformTraverseComponent : public Component {

    std::vector<std::tuple<std::string, EulerAnglesTransform, float>> mMovementPoints;

    std::unique_ptr<PlatformTraverseComponentVisitorBase> mBehaviorVisitor;

    std::optional<std::tuple<std::string, EulerAnglesTransform, float>> mDestinationPoint;
    std::string mLastDestinationPoint;

    float mTime;

public:
    PlatformTraverseComponent(const std::shared_ptr<PlatformTraverseComponentData>& data);

    ~PlatformTraverseComponent() override;

    eComponentType GetComponentType() const override;

    void Tick(const float deltaTime) override;

    void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

    void PostLevelInit() override;

    const std::vector<std::tuple<std::string, EulerAnglesTransform, float>>& GetMovementPoints() const;

private:
    void Move(const float deltaTime);

    void SetDestinationPoint(const std::string& pointName);
};

} // namespace EngineCore
