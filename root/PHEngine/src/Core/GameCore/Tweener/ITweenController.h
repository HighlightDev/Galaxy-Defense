#pragma once

#include "Core/GameCore/EngineObjectProperty.h"

#include <memory>

namespace EngineCore {
enum class StateType { SourceState = 0, DestinationState = 1 };

struct BaseStateProperty;

class ITweenController {
protected:
    std::shared_ptr<BaseStateProperty> TranstionProperties[2];

    std::weak_ptr<EngineObjectPropertyBase> mEngineObjectPropertyWp;

    template<typename Type>
    std::shared_ptr<EngineObjectProperty<Type>> GetEngineObjectPropertySp() const
    {
        return std::static_pointer_cast<EngineObjectProperty<Type>>(mEngineObjectPropertyWp.lock());
    }

public:
    ITweenController();

    virtual ~ITweenController() = default;

    virtual void OnTransitionFinished() = 0;

    virtual void OnTransitionStarted(
        const std::shared_ptr<BaseStateProperty>& srcStateProperty,
        const std::shared_ptr<BaseStateProperty>& dstStateProperty,
        const float transitionDuration,
        const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty);

    virtual void OnTransitionUpdate(const float deltaTimeSec, const float transitionParameter);

    virtual void InitWithPropsInstant(
        const std::shared_ptr<BaseStateProperty>& dstStateProperty,
        const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty);

    enum eEnginePropertyBindingType GetControllerPropertyType() const;
};
} // namespace EngineCore
