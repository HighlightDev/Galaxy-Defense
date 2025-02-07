#pragma once
#include "Component.h"

#include <glm/vec3.hpp>

namespace EngineCore {
class Actor;

struct MovementComponentData;

class MovementComponent : public Component {
protected:
    float mReferenceSpeed;
    float mCurrentSpeed;

    glm::vec3 mDirection;

public:
    MovementComponent(const std::shared_ptr<MovementComponentData>& movementComponentData);

    ~MovementComponent() override;

    eComponentType GetComponentType() const override;

    virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) = 0;

    virtual void Move(const float deltaTime) = 0;

    virtual void Move(const glm::vec3& direction, const float deltaTime) = 0;

    virtual void Jump() = 0;

    virtual void Teleport(const glm::vec3& teleportPosition) = 0;

    void SetCurrentSpeed(const float speed);

    float GetCurrentSpeed() const;

    void SetReferenceSpeed(const float speed);

    float GetReferenceSpeed() const;

    void SetCurrentSpeedToReferenceValue();

    void SetDirection(const glm::vec3& direction);

    glm::vec3 GetDirection() const;
};

} // namespace EngineCore
