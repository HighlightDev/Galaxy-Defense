#pragma once
#include "Component.h"
#include "Core/CommonCore/VariableWrapper.h"
#include "Core/GameCore/Components/Transform.h"

#include <glm/mat4x4.hpp>

namespace EngineCore {
class Actor;

class SceneComponent : public Component {
    using Base = Component;

    std::shared_ptr<EngineObjectProperty<glm::vec3>>
        m_TransformScale; // This property is used only for bindings. For all other purposes use transform instead

protected:
    bool bTransformationDirty;

    std::shared_ptr<Transform> mTransform;

    std::shared_ptr<EngineObjectProperty<glm::vec3>> m_additionalRotationEuler;

    glm::mat4 m_relativeMatrix;

public:
    bool bIsRootComponent = false;

    SceneComponent(
        const std::string& gameObjectName, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale);

    ~SceneComponent() override;

    void Tick(const float deltaTime) override;

    void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

    eComponentType GetComponentType() const override;

    /* This method works every time when this component has dirty transform */
    virtual void UpdateRelativeMatrix(const glm::mat4& parentRelativeMatrix = glm::mat4(1));

    void SetIsTransformationDirty(const bool isDirty);

    void SetTranslation(const glm::vec3& translation);

    void SetRotator(const glm::quat& rotator);

    void SetScale(glm::vec3 scale);

    void SetAdditionalRotation(const glm::vec3& rotationDegrees);

    glm::vec3 GetAdditionalRotation() const;

    std::weak_ptr<Transform> GetTransformWeakPtr() const;

    bool GetIsTransformationDirty() const;

    glm::vec3 GetTranslation() const;

    glm::quat GetRotator() const;

    glm::vec3 GetRotationDegrees() const;

    glm::vec3 GetScale() const;

    glm::mat4 GetRelativeMatrix() const;

    void AddTranslation(const glm::vec3& offsetTranslation);

    glm::vec3 GetHierarchyAccumulatedTranslation() const;

    glm::quat GetHierarchyAccumulatedRotator() const;

private:
    void IterateHierarchyUpCollectRotator(const std::weak_ptr<Actor>& currentOwnerWp, glm::quat& accumulatedRotator) const;
    void
    IterateHierarchyUpCollectTranslation(const std::weak_ptr<Actor>& currentOwnerWp, glm::vec3& accumulatedTranslation) const;

    void SyncScale(const glm::vec3& scale);
};
} // namespace EngineCore
