#include "CollisionCompoundShape.h"

#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"

using namespace EngineMath;

namespace EnginePhysics {

CollisionCompoundShape::CollisionCompoundShape()
    : CollisionShapeBase(new btCompoundShape())
    , mChildShapes()
{
}

void CollisionCompoundShape::AddChildShape(
    const NoScaleEulerRotationTransform& childTransform, const std::shared_ptr<CollisionShapeBase>& childShape)
{
    btTransform localTransform;
    localTransform.setIdentity();
    localTransform.setOrigin(Converter::glmToBullet(childTransform.Translation));
    localTransform.setRotation(Converter::glmToBullet(glm::quat(glm::vec3(
        DEG_TO_RAD(childTransform.RotationEulerAngles.x),
        DEG_TO_RAD(childTransform.RotationEulerAngles.y),
        DEG_TO_RAD(childTransform.RotationEulerAngles.z)))));

    auto compoundShape = static_cast<btCompoundShape*>(mCollisionShape);
    compoundShape->addChildShape(localTransform, childShape->GetCollisionShape());

    mChildShapes.emplace_back(std::make_pair(
        childShape, NoScaleEulerRotationTransform(childTransform.Translation, childTransform.RotationEulerAngles)));
}

void CollisionCompoundShape::RemoveAllChildShapes()
{
    auto* compoundShape = static_cast<btCompoundShape*>(mCollisionShape);
    const int32_t childCount = compoundShape->getNumChildShapes();
    for (int32_t i = childCount - 1; i >= 0; --i) {
        compoundShape->removeChildShapeByIndex(i);
    }
    mChildShapes.clear();
}

const std::vector<std::pair<std::shared_ptr<CollisionShapeBase>, NoScaleEulerRotationTransform>>&
CollisionCompoundShape::GetChildShapes() const
{
    return mChildShapes;
}

} // namespace EnginePhysics