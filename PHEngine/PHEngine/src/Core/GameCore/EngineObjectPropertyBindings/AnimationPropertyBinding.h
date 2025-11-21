#pragma once

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "PropertyBinding.h"

namespace EngineCore {

struct AnimationPropertyBinding : public PropertyBinding {
private:
    std::weak_ptr<EngineObjectProperty<std::string>> SrcNameWp;
    std::weak_ptr<EngineObjectProperty<std::string>> DstNameWp;

    std::weak_ptr<EngineObjectProperty<float>> SrcTimeWp;
    std::weak_ptr<EngineObjectProperty<float>> DstTimeWp;

    std::weak_ptr<EngineObjectProperty<bool>> bTranstitionEnabledWp;
    std::weak_ptr<EngineObjectProperty<float>> TransitionValueWp;

public:
    AnimationPropertyBinding(const std::string& bindingName)
        : PropertyBinding(bindingName)
        , SrcNameWp()
        , DstNameWp()
        , SrcTimeWp()
        , DstTimeWp()
        , bTranstitionEnabledWp()
        , TransitionValueWp()
    {
    }

    void SetSrcName(const std::string& name)
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: AnimationPropertyBinding::SetSrcName: Property not connected. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = SrcNameWp.lock()) {
            propertySp->SetValue(name);
        }
    }

    void SetDstName(const std::string& name) const
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: AnimationPropertyBinding::SetDstName: Property not connected. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = DstNameWp.lock()) {
            propertySp->SetValue(name);
        }
    }

    void SetSrcTime(const float value)
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: AnimationPropertyBinding::SetSrcTime: Property not connected. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = SrcTimeWp.lock()) {
            propertySp->SetValue(value);
        }
    }

    void SetDstTime(const float value)
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: AnimationPropertyBinding::SetDstTime: Property not connected. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = DstTimeWp.lock()) {
            propertySp->SetValue(value);
        }
    }

    void SetIsTransitionEnabled(bool bEnabled)
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: AnimationPropertyBinding::SetIsTransitionEnabled: Property not connected. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = bTranstitionEnabledWp.lock()) {
            propertySp->SetValue(bEnabled);
        }
    }

    void SetTransitionValue(float transitionValue)
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: AnimationPropertyBinding::SetTransitionValue: Property not connected. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = TransitionValueWp.lock()) {
            propertySp->SetValue(transitionValue);
        }
    }

    std::string GetSrcName() const
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: AnimationPropertyBinding::GetSrcName: Property not connected. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = SrcNameWp.lock()) {
            return propertySp->GetValue();
        }
        return "";
    }

    std::string GetDstName() const
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: AnimationPropertyBinding::GetDstName: Property not connected. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = DstNameWp.lock()) {
            return propertySp->GetValue();
        }
        return "";
    }

    float GetSrcTime() const
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: AnimationPropertyBinding::GetSrcTime: Property not connected. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = SrcTimeWp.lock()) {
            return propertySp->GetValue();
        }
        return 0.0f;
    }

    float GetDstTime() const
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: AnimationPropertyBinding::GetDstTime: Property not connected. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = DstTimeWp.lock()) {
            return propertySp->GetValue();
        }
        return 0.0f;
    }

    bool GetIsTransitionEnabled() const
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: AnimationPropertyBinding::GetIsTransitionEnabled: Property not connected. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = bTranstitionEnabledWp.lock()) {
            return propertySp->GetValue();
        }
        return 0.0f;
    }

    float GetTransitionValue() const
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: AnimationPropertyBinding::GetTransitionValue: Property not connected. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = TransitionValueWp.lock()) {
            return propertySp->GetValue();
        }
        return 0.0f;
    }

    void SetBindingProperties(
        const std::shared_ptr<EngineObjectProperty<std::string>>& srcName,
        const std::shared_ptr<EngineObjectProperty<std::string>>& dstName,
        const std::shared_ptr<EngineObjectProperty<float>>& srcTime,
        const std::shared_ptr<EngineObjectProperty<float>>& dstTime,
        const std::shared_ptr<EngineObjectProperty<bool>>& isTransitionEnabled,
        const std::shared_ptr<EngineObjectProperty<float>>& transitionValue)
    {
        SrcNameWp = srcName;
        DstNameWp = dstName;
        SrcTimeWp = srcTime;
        DstTimeWp = dstTime;
        bTranstitionEnabledWp = isTransitionEnabled;
        TransitionValueWp = transitionValue;

        bPropertyConnected = true;
    }

    eEnginePropertyBindingType GetBindingType() const override
    {
        return eEnginePropertyBindingType::Animation;
    }
};
} // namespace EngineCore