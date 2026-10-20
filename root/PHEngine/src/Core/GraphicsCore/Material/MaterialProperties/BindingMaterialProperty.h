#pragma once

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "MaterialProperty.h"

#include <memory>

using namespace EngineCore;

namespace Graphics {

struct BindingMaterialProperty : public MaterialProperty {
protected:
    std::weak_ptr<EngineObjectPropertyBase> mEngineObjectPropertyWp;

    std::weak_ptr<IMaterial> mOwnerMaterialWp;

    std::shared_ptr<ObserverConnection<void(void)>::SubscriptionHandler> mEngineObjectPropertyChangeSubscription;

    template<typename Type>
    Type GetEngineObjectPropertyValue() const
    {
        if (const auto propertySp = std::static_pointer_cast<EngineObjectProperty<Type>>(mEngineObjectPropertyWp.lock())) {
            return propertySp->GetValue();
        }
        return Type();
    }

    virtual void OnEngineObjectPropertyChanged()
    {
        if (const auto& ownerSp = mOwnerMaterialWp.lock()) {
            ownerSp->SetIsDataDirty(true);
        }
    }

public:
    explicit BindingMaterialProperty(const std::string& propertyName, const std::shared_ptr<IMaterial>& ownerMaterial)
        : MaterialProperty(propertyName)
        , mOwnerMaterialWp(ownerMaterial)
    {
    }

    virtual eEnginePropertyType GetBindingEnginePropertyType() const = 0;

    void BindEngineObjectProperty(const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty)
    {
        ext_assert(engineObjectProperty, "BindingMaterialProperty::BindEngineObjectProperty: property is null: " + mPropertyName);
        ext_assert(
            engineObjectProperty->GetPropertyType() == GetBindingEnginePropertyType(),
            "BindingMaterialProperty::BindEngineObjectProperty: property type mismatch: " + mPropertyName);
        mEngineObjectPropertyWp = engineObjectProperty;
        if (std::shared_ptr<ObserverConnection<void(void)>> observerConnectionSp
            = engineObjectProperty->GetObserverConnection().lock()) {
            mEngineObjectPropertyChangeSubscription
                = observerConnectionSp->Subscribe(std::bind(&BindingMaterialProperty::OnEngineObjectPropertyChanged, this));
        }

        OnEngineObjectPropertyChanged();
    }
};
} // namespace Graphics
