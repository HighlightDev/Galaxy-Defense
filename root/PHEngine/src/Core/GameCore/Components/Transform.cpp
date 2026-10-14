#include "Transform.h"

#include <utility>

namespace EngineCore {
TranslationTransform::TranslationTransform(const glm::vec3& translation)
    : Translation(translation)
{
}

RotatorTransform::RotatorTransform(const glm::quat& rotator)
    : Rotator(rotator)
{
}

EulerRotationTransform::EulerRotationTransform(const glm::vec3& eulerRotationAngles)
    : RotationEulerAngles(eulerRotationAngles)
{
}

ScaleTransform::ScaleTransform(const glm::vec3& scale)
    : Scale(scale)
{
}

NoScaleEulerRotationTransform::NoScaleEulerRotationTransform(const glm::vec3& translation, const glm::vec3& eulerAngles)
    : TranslationTransform(translation)
    , EulerRotationTransform(eulerAngles)
{
}

BoundingBoxTransform::BoundingBoxTransform()
    : TranslationTransform(glm::vec3())
    , ScaleTransform(glm::vec3(1))
{
}

BoundingBoxTransform::BoundingBoxTransform(const glm::vec3& translation, const glm::vec3& scale)
    : TranslationTransform(translation)
    , ScaleTransform(scale)
{
}

EulerAnglesTransform::EulerAnglesTransform()
    : TranslationTransform(glm::vec3())
    , EulerRotationTransform(glm::vec3())
    , ScaleTransform(glm::vec3(1))
{
}

EulerAnglesTransform::EulerAnglesTransform(const EulerAnglesTransform& transform)
    : TranslationTransform(transform.Translation)
    , EulerRotationTransform(transform.RotationEulerAngles)
    , ScaleTransform(transform.Scale)
{
}

EulerAnglesTransform::EulerAnglesTransform(const glm::vec3& translation, const glm::vec3& eulerAngles, const glm::vec3& scale)
    : TranslationTransform(translation)
    , EulerRotationTransform(eulerAngles)
    , ScaleTransform(scale)
{
}

EulerAnglesTransform& EulerAnglesTransform::operator=(const EulerAnglesTransform& t)
{
    Translation = t.Translation;
    RotationEulerAngles = t.RotationEulerAngles;
    Scale = t.Scale;
    return *this;
}

Transform::Transform()
    : TranslationTransform(glm::vec3())
    , RotatorTransform(glm::quat())
    , ScaleTransform(glm::vec3(1))
{
}

Transform::Transform(const Transform& transform)
    : TranslationTransform(transform.Translation)
    , RotatorTransform(transform.Rotator)
    , ScaleTransform(transform.Scale)
{
}

Transform::Transform(const glm::vec3& translation, const glm::quat& rotator, const glm::vec3& scale)
    : TranslationTransform(translation)
    , RotatorTransform(rotator)
    , ScaleTransform(scale)
{
}

Transform& Transform::operator=(const Transform& t)
{
    Translation = t.Translation;
    Rotator = t.Rotator;
    Scale = t.Scale;
    return *this;
}
} // namespace EngineCore