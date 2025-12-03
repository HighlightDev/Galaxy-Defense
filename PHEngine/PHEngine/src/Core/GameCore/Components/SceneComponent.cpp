#include "SceneComponent.h"

#include "Core/GameCore/Actor.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cmath>

using namespace EngineMath;

namespace EngineCore {
SceneComponent::SceneComponent(
    const std::string& gameObjectName,
    const glm::vec3& translation = glm::vec3(0.0f),
    const glm::vec3& rotation = glm::vec3(0.0f),
    const glm::vec3& scale = glm::vec3(0.0f))
    : Component(gameObjectName)
    , m_TransformScale(std::make_shared<EngineObjectProperty<glm::vec3>>(
          scale, "p_scale", [this](const glm::vec3& scale) { SyncScale(scale); }))
    , m_TransformTranslation(std::make_shared<EngineObjectProperty<glm::vec3>>(glm::vec3(0.0f), "p_translation"))
    , bTransformationDirty(true)
    , mTransform(std::make_shared<Transform>(
          translation, glm::quat(glm::vec3(DEG_TO_RAD(rotation.x), DEG_TO_RAD(rotation.y), DEG_TO_RAD(rotation.z))), scale))
    , m_additionalRotationEuler(std::make_shared<EngineObjectProperty<glm::vec3>>(
          glm::vec3(0.0f), "p_rotator", [this](const glm::vec3& rotator) { SetIsTransformationDirty(true); }))
    , m_relativeMatrix(1)
    , m_outlineMatrix(1)
{
    AddEngineProperty(m_additionalRotationEuler);
    AddEngineProperty(m_TransformScale);
    AddEngineProperty(m_TransformTranslation);
}

SceneComponent::~SceneComponent()
{
}

void SceneComponent::SyncScale(const glm::vec3& scale)
{
    SetScale(scale);
}

void SceneComponent::Tick(const float deltaTimeSec)
{
    Component::Tick(deltaTimeSec);
}

eComponentType SceneComponent::GetComponentType() const
{
    return SCENE_COMPONENT;
}

void SceneComponent::AddTranslation(const glm::vec3& offsetTranslation)
{
    SetTranslation(mTransform->Translation + offsetTranslation);
}

void SceneComponent::SetOutlineThickness(const float outlineThickness)
{
    if (mOutlineThickness != outlineThickness) {
        mOutlineThickness = outlineThickness;
        SetIsTransformationDirty(false);
    }
}

float SceneComponent::GetOutlineThickness() const
{
    return mOutlineThickness;
}

void SceneComponent::UpdateOutlineMatrix(const glm::mat4& parentRelativeMatrix)
{
    if (!mIsEnabled)
        return;

    // Update current outline matrix

    const glm::vec3 thicknessScale = glm::vec3(mOutlineThickness) / mTransform->Scale;

    const glm::mat4 identityMatrix(1);
    m_outlineMatrix = identityMatrix;
    m_outlineMatrix *= parentRelativeMatrix;
    m_outlineMatrix *= glm::translate(identityMatrix, mTransform->Translation);
    m_outlineMatrix *= glm::scale(identityMatrix, mTransform->Scale + thicknessScale);

    if (bIsRootComponent) {
        const auto& additionalRotation = m_additionalRotationEuler->GetValue();
        const glm::mat4 pitchRotation = glm::rotate(identityMatrix, DEG_TO_RAD(additionalRotation.x), AXIS_RIGHT);
        const glm::mat4 yawRotation = glm::rotate(identityMatrix, DEG_TO_RAD(additionalRotation.y), AXIS_UP);
        const glm::mat4 rollRotation = glm::rotate(identityMatrix, DEG_TO_RAD(additionalRotation.z), AXIS_FORWARD);

        m_outlineMatrix *= pitchRotation;
        m_outlineMatrix *= yawRotation;
        m_outlineMatrix *= rollRotation;
    }

    const auto nRotator = glm::normalize(mTransform->Rotator);
    m_outlineMatrix *= glm::toMat4(nRotator);

    SetIsTransformationDirty(false);
}

void SceneComponent::UpdateRelativeMatrix(const glm::mat4& parentRelativeMatrix)
{
    if (!mIsEnabled)
        return;

    if (mIsOutlineApplied) {
        UpdateOutlineMatrix(parentRelativeMatrix);
    }

    // Update current relative matrix

    const glm::mat4 identityMatrix(1);
    m_relativeMatrix = identityMatrix;
    m_relativeMatrix *= parentRelativeMatrix;
    m_relativeMatrix *= glm::translate(identityMatrix, mTransform->Translation);
    m_relativeMatrix *= glm::scale(identityMatrix, mTransform->Scale);

    if (bIsRootComponent) {
        const auto& additionalRotation = m_additionalRotationEuler->GetValue();
        const glm::mat4 pitchRotation = glm::rotate(identityMatrix, DEG_TO_RAD(additionalRotation.x), AXIS_RIGHT);
        const glm::mat4 yawRotation = glm::rotate(identityMatrix, DEG_TO_RAD(additionalRotation.y), AXIS_UP);
        const glm::mat4 rollRotation = glm::rotate(identityMatrix, DEG_TO_RAD(additionalRotation.z), AXIS_FORWARD);

        m_relativeMatrix *= pitchRotation;
        m_relativeMatrix *= yawRotation;
        m_relativeMatrix *= rollRotation;
    }

    const auto nRotator = glm::normalize(mTransform->Rotator);
    m_relativeMatrix *= glm::toMat4(nRotator);

    SetIsTransformationDirty(false);
}

void SceneComponent::SetIsTransformationDirty(const bool isDirty)
{
    bTransformationDirty = isDirty;
}

void SceneComponent::SetTranslation(const glm::vec3& translation)
{
    mTransform->Translation = translation;
    m_TransformTranslation->SetValue(translation);
    SetIsTransformationDirty(true);
}

void SceneComponent::SetRotator(const glm::quat& rotator)
{
    mTransform->Rotator = rotator;
    SetIsTransformationDirty(true);
}

void SceneComponent::SetScale(glm::vec3 scale)
{
    mTransform->Scale = scale;
    SetIsTransformationDirty(true);
}

void SceneComponent::SetAdditionalRotation(const glm::vec3& rotationDegrees)
{
    m_additionalRotationEuler->SetValue(rotationDegrees);
    SetIsTransformationDirty(true);
}

glm::vec3 SceneComponent::GetAdditionalRotation() const
{
    return m_additionalRotationEuler->GetValue();
}

std::weak_ptr<Transform> SceneComponent::GetTransformWeakPtr() const
{
    return mTransform;
}

bool SceneComponent::GetIsTransformationDirty() const
{
    return bTransformationDirty;
}

glm::vec3 SceneComponent::GetTranslation() const
{
    return mTransform->Translation;
}

glm::quat SceneComponent::GetRotator() const
{
    return mTransform->Rotator;
}

glm::vec3 SceneComponent::GetRotationDegrees() const
{
    return EngineMath::QuatToEulerAngles(GetRotator());
}

glm::vec3 SceneComponent::GetScale() const
{
    return mTransform->Scale;
}

glm::mat4 SceneComponent::GetRelativeMatrix() const
{
    return m_relativeMatrix;
}

glm::vec3 SceneComponent::GetHierarchyAccumulatedTranslation() const
{
    glm::vec3 result(0.0f);

    if (auto ownerSp = GetOwner().lock()) {
        IterateHierarchyUpCollectTranslation(ownerSp->GetParent(), result);
    }

    result += mTransform->Translation;

    return result;
}

glm::quat SceneComponent::GetHierarchyAccumulatedRotator() const
{
    glm::quat result(glm::vec3(0, 0, 0));

    if (auto ownerSp = GetOwner().lock()) {
        IterateHierarchyUpCollectRotator(ownerSp->GetParent(), result);
    }

    result *= mTransform->Rotator;

    return result;
}

void SceneComponent::IterateHierarchyUpCollectRotator(
    const std::weak_ptr<Actor>& currentOwnerWp, glm::quat& accumulatedRotator) const
{
    if (auto currentOwnerSp = currentOwnerWp.lock()) {
        IterateHierarchyUpCollectRotator(currentOwnerSp->GetParent(), accumulatedRotator);
        accumulatedRotator *= currentOwnerSp->GetRootComponent()->GetRotator();
    }
}

void SceneComponent::IterateHierarchyUpCollectTranslation(
    const std::weak_ptr<Actor>& currentOwnerWp, glm::vec3& accumulatedTranslation) const
{
    if (auto currentOwnerSp = currentOwnerWp.lock()) {
        IterateHierarchyUpCollectTranslation(currentOwnerSp->GetParent(), accumulatedTranslation);
        accumulatedTranslation += currentOwnerSp->GetRootComponent()->GetTranslation();
    }
}

glm::mat4 SceneComponent::GetOutlineMatrix() const
{
    return m_outlineMatrix;
}

void SceneComponent::SetIsRootComponent(const bool isRootComponent)
{
    bIsRootComponent = isRootComponent;
}

bool SceneComponent::GetIsRootComponent() const
{
    return bIsRootComponent;
}

} // namespace EngineCore
