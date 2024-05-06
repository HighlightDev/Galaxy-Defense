#include "Vec3AnimationController.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/IAnimatable.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/AnimationData.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/EngineObjectProperty.h"

namespace EngineCore
{
    namespace GUI
    {
        void Vec3AnimationController::ProcessAnimation(const float animationTimePassed,
                                                       const AnimationData &data,
                                                       const std::shared_ptr<IAnimatable> &animatable)
        {
            const auto &engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
            assert(engineProperty);
            const auto propertyType = engineProperty->GetPropertyType();
            assert(eEnginePropertyType::Vec3 == propertyType);
            const auto vec3Property = std::static_pointer_cast<EngineObjectProperty<glm::vec3>>(engineProperty);

            if (animationTimePassed >= data.GetAnimationDuration())
            {
                mIsAnimationFinished = true;
                vec3Property->SetValue(std::any_cast<glm::vec3>(data.GetDstValue()));
            }
            else
            {
                if (eAnimationInterpolationFunctionType::LINEAR == data.GetAnimationFunctionType())
                {
                    const glm::vec3 srcValue = std::any_cast<glm::vec3>(data.GetSrcValue());
                    const glm::vec3 dstValue = std::any_cast<glm::vec3>(data.GetDstValue());
                    const glm::vec3 resultValue = EngineMath::LerpVec3(animationTimePassed, 0.0f, data.GetAnimationDuration(), srcValue, dstValue);
                    vec3Property->SetValue(resultValue);
                }
                else
                {
                    assert(false);
                }
            }
        }

        void Vec3AnimationController::ForceFinishAnimation(const AnimationData &data,
                                                           const std::shared_ptr<IAnimatable> &animatable)
        {
            const auto &engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
            assert(engineProperty);
            const auto propertyType = engineProperty->GetPropertyType();
            assert(eEnginePropertyType::Vec3 == propertyType);
            const auto vec3Property = std::static_pointer_cast<EngineObjectProperty<glm::vec3>>(engineProperty);

            mIsAnimationFinished = true;
            vec3Property->SetValue(std::any_cast<glm::vec3>(data.GetDstValue()));
        }

        void Vec3AnimationController::InitWithSrcValues(const AnimationData &data,
                                                        const std::shared_ptr<IAnimatable> &animatable)
        {
            const auto &engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
            assert(engineProperty);
            const auto propertyType = engineProperty->GetPropertyType();
            assert(eEnginePropertyType::Vec3 == propertyType);
            const auto vec3Property = std::static_pointer_cast<EngineObjectProperty<glm::vec3>>(engineProperty);
            vec3Property->SetValue(std::any_cast<glm::vec3>(data.GetSrcValue()));
            mIsAnimationFinished = false;
        }

        eEnginePropertyType Vec3AnimationController::GetPropertyType() const
        {
            return eEnginePropertyType::Vec3;
        }
    } // namespace GUI

} // namespace EngineCore
