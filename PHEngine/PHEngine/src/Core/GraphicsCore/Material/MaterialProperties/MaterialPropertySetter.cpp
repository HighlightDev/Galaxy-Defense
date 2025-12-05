
#include "MaterialPropertySetter.h"

#include "BindingMaterialProperty.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObject.h"
#include "Core/GameCore/EngineObjectPropertyBindings/FloatPropertyBinding.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/GraphicsCore/Material/DynamicMaterial.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/MaterialProperties/DynamicMaterialProperties/DynamicInstancedFloatMaterialProperty.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResourceCreator.h"
#include "Core/ResourceManagerCore/MaterialInstanceDataProviders/MaterialInstanceDataProvider.h"
#include "DeferredTextureMaterialProperty.h"
#include "FloatMaterialProperty.h"
#include "IntegerMaterialProperty.h"
#include "TextureMaterialProperty.h"
#include "Vec2MaterialProperty.h"
#include "Vec3MaterialProperty.h"
#include "iVec2MaterialProperty.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics::Texture;

namespace Graphics {
std::shared_ptr<DynamicMaterial>
MaterialPropertySetter::TryCastToDynamicMaterial(const std::shared_ptr<IMaterial>& materialInstance)
{
    return materialInstance->GetMaterialType() == IMaterial::eMaterialType::DYNAMIC
        ? std::static_pointer_cast<DynamicMaterial>(materialInstance)
        : nullptr;
}

void MaterialPropertySetter::SetTextureValue(
    const std::shared_ptr<MaterialProperty>& materialProperty, const std::shared_ptr<ITexture>& texture)
{
    const auto propertyType = materialProperty->GetPropertyType();
    ext_assert(
        propertyType == MaterialProperty::eMaterialPropertyType::TEXTURE_PROPERTY,
        "MaterialPropertySetter::SetTextureValue: materialProperty type is not TEXTURE_PROPERTY");
    const auto& textureProperty = std::static_pointer_cast<TextureMaterialProperty>(materialProperty);
    ext_assert(textureProperty, "MaterialPropertySetter::SetTextureValue: textureProperty is null");
    textureProperty->SetValue(texture);
}

void MaterialPropertySetter::SetFloatValue(const std::shared_ptr<MaterialProperty>& materialProperty, const float value)
{
    const auto propertyType = materialProperty->GetPropertyType();
    ext_assert(
        propertyType == MaterialProperty::eMaterialPropertyType::FLOAT_PROPERTY,
        "MaterialPropertySetter::SetFloatValue: materialProperty type is not FLOAT_PROPERTY");
    const auto& floatProperty = std::static_pointer_cast<FloatMaterialProperty>(materialProperty);
    ext_assert(floatProperty, "MaterialPropertySetter::SetFloatValue: floatProperty is null");
    floatProperty->SetValue(value);
}

void MaterialPropertySetter::SetMaterialPropertyValue(
    const std::shared_ptr<IMaterial>& materialInstance, const std::string& propertyName, const int32_t value)
{
    ext_assert(materialInstance, "MaterialPropertySetter::SetMaterialPropertyValue: materialInstance is null");

    // first try to find material property among related to dynamic property
    if (const auto& dynamicMaterial = TryCastToDynamicMaterial(materialInstance)) {
        if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetIntegerValue(property, value);
        else
            ext_assert(false, "MaterialPropertySetter::SetMaterialPropertyValue: property not found in dynamic material");
    } else {
        SetIntegerValue(materialInstance->GetMaterialPropertyByName(propertyName), value);
    }
}

void MaterialPropertySetter::SetIntegerValue(const std::shared_ptr<MaterialProperty>& materialProperty, const int32_t value)
{
    const auto propertyType = materialProperty->GetPropertyType();
    ext_assert(
        propertyType == MaterialProperty::eMaterialPropertyType::INTEGER_PROPERTY,
        "MaterialPropertySetter::SetIntegerValue: materialProperty type is not INTEGER_PROPERTY");
    const auto& intProperty = std::static_pointer_cast<IntegerMaterialProperty>(materialProperty);
    ext_assert(intProperty, "MaterialPropertySetter::SetIntegerValue: intProperty is null");
    intProperty->SetValue(value);
}

void MaterialPropertySetter::SetIVec2Value(const std::shared_ptr<MaterialProperty>& materialProperty, const glm::ivec2& value)
{
    const auto propertyType = materialProperty->GetPropertyType();
    ext_assert(
        propertyType == MaterialProperty::eMaterialPropertyType::IVEC2_PROPERTY,
        "MaterialPropertySetter::SetIVec2Value: materialProperty type is not IVEC2_PROPERTY");
    const auto& ivec2Property = std::static_pointer_cast<iVec2MaterialProperty>(materialProperty);
    ext_assert(ivec2Property, "MaterialPropertySetter::SetIVec2Value: ivec2Property is null");
    ivec2Property->SetValue(value);
}

void MaterialPropertySetter::SetVec2Value(const std::shared_ptr<MaterialProperty>& materialProperty, const glm::vec2& value)
{
    const auto propertyType = materialProperty->GetPropertyType();
    ext_assert(
        propertyType == MaterialProperty::eMaterialPropertyType::VEC2_PROPERTY,
        "MaterialPropertySetter::SetVec2Value: materialProperty type is not VEC2_PROPERTY");
    const auto& vec2Property = std::static_pointer_cast<Vec2MaterialProperty>(materialProperty);
    ext_assert(vec2Property, "MaterialPropertySetter::SetVec2Value: vec2Property is null");
    vec2Property->SetValue(value);
}

void MaterialPropertySetter::SetVec3Value(const std::shared_ptr<MaterialProperty>& materialProperty, const glm::vec3& value)
{
    const auto propertyType = materialProperty->GetPropertyType();
    ext_assert(
        propertyType == MaterialProperty::eMaterialPropertyType::VEC3_PROPERTY,
        "MaterialPropertySetter::SetVec3Value: materialProperty type is not VEC3_PROPERTY");
    const auto& vec3Property = std::static_pointer_cast<Vec3MaterialProperty>(materialProperty);
    ext_assert(vec3Property, "MaterialPropertySetter::SetVec3Value: vec3Property is null");
    vec3Property->SetValue(value);
}

void MaterialPropertySetter::SetDeferredResourceValue(
    const std::shared_ptr<MaterialProperty>& materialProperty,
    const std::shared_ptr<IDeferredResourceCreator>& deferredResourceCreator)
{
    if (materialProperty->GetPropertyType() == MaterialProperty::eMaterialPropertyType::DEFERRED_TEXTURE_PROPERTY) {
        auto deferredTextureProperty = std::static_pointer_cast<DeferredTextureMaterialProperty>(materialProperty);
        ext_assert(deferredTextureProperty, "MaterialPropertySetter::SetDeferredResourceValue: deferredTextureProperty is null");
        const auto& textureResource
            = std::static_pointer_cast<DeferredResource<std::shared_ptr<ITexture>, eDeferredResourceType::TEXTURE>>(
                deferredResourceCreator->GetDeferredResource());
        ext_assert(textureResource, "MaterialPropertySetter::SetDeferredResourceValue: textureResource is null");
        deferredTextureProperty->SetValue(textureResource);
    } else {
        ext_assert(false, "MaterialPropertySetter::SetDeferredResourceValue: unsupported material property type");
    }
}

void MaterialPropertySetter::SetMaterialPropertyValue(
    const std::shared_ptr<IMaterial>& materialInstance, const std::string& propertyName, const std::shared_ptr<ITexture>& texture)
{
    ext_assert(materialInstance, "MaterialPropertySetter::SetMaterialPropertyValue: materialInstance is null");

    // first try to find material property among related to dynamic property
    if (const auto& dynamicMaterial = TryCastToDynamicMaterial(materialInstance)) {
        if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetTextureValue(property, texture);
        else
            ext_assert(false, "MaterialPropertySetter::SetMaterialPropertyValue: property not found in dynamic material");
    } else {
        SetTextureValue(materialInstance->GetMaterialPropertyByName(propertyName), texture);
    }
}

void MaterialPropertySetter::SetMaterialPropertyValue(
    const std::shared_ptr<IMaterial>& materialInstance, const std::string& propertyName, const float value)
{
    ext_assert(materialInstance, "MaterialPropertySetter::SetMaterialPropertyValue: materialInstance is null");

    // first try to find material property among related to dynamic property
    if (const auto& dynamicMaterial = TryCastToDynamicMaterial(materialInstance)) {
        if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetFloatValue(property, value);
        else
            ext_assert(false, "MaterialPropertySetter::SetMaterialPropertyValue: property not found in dynamic material");
    } else {
        SetFloatValue(materialInstance->GetMaterialPropertyByName(propertyName), value);
    }
}

void MaterialPropertySetter::SetMaterialPropertyValue(
    const std::shared_ptr<IMaterial>& materialInstance, const std::string& propertyName, const glm::ivec2& value)
{
    ext_assert(materialInstance, "MaterialPropertySetter::SetMaterialPropertyValue: materialInstance is null");

    // first try to find material property among related to dynamic property
    if (const auto& dynamicMaterial = TryCastToDynamicMaterial(materialInstance)) {
        if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetIVec2Value(property, value);
        else
            ext_assert(false, "MaterialPropertySetter::SetMaterialPropertyValue: property not found in dynamic material");
    } else {
        SetIVec2Value(materialInstance->GetMaterialPropertyByName(propertyName), value);
    }
}

void MaterialPropertySetter::SetMaterialPropertyValue(
    const std::shared_ptr<IMaterial>& materialInstance, const std::string& propertyName, const glm::vec2& value)
{
    ext_assert(materialInstance, "MaterialPropertySetter::SetMaterialPropertyValue: materialInstance is null");

    // first try to find material property among related to dynamic property
    if (const auto& dynamicMaterial = TryCastToDynamicMaterial(materialInstance)) {
        if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetVec2Value(property, value);
        else
            ext_assert(false, "MaterialPropertySetter::SetMaterialPropertyValue: property not found in dynamic material");
    } else {
        SetVec2Value(materialInstance->GetMaterialPropertyByName(propertyName), value);
    }
}

void MaterialPropertySetter::SetMaterialPropertyValue(
    const std::shared_ptr<IMaterial>& materialInstance, const std::string& propertyName, const glm::vec3& value)
{
    ext_assert(materialInstance, "MaterialPropertySetter::SetMaterialPropertyValue: materialInstance is null");

    // first try to find material property among related to dynamic property
    if (const auto& dynamicMaterial = TryCastToDynamicMaterial(materialInstance)) {
        if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetVec3Value(property, value);
        else
            ext_assert(false, "MaterialPropertySetter::SetMaterialPropertyValue: property not found in dynamic material");
    } else {
        SetVec3Value(materialInstance->GetMaterialPropertyByName(propertyName), value);
    }
}

void MaterialPropertySetter::SetMaterialPropertyValue(
    const std::shared_ptr<IMaterial>& materialInstance,
    const std::string& propertyName,
    const std::shared_ptr<IDeferredResourceCreator>& deferredResourceCreator)
{
    ext_assert(materialInstance, "MaterialPropertySetter::SetMaterialPropertyValue: materialInstance is null");

    // first try to find material property among related to dynamic property
    if (const auto& dynamicMaterial = TryCastToDynamicMaterial(materialInstance)) {
        if (auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(propertyName))
            SetDeferredResourceValue(property, deferredResourceCreator);
        else
            ext_assert(false, "MaterialPropertySetter::SetMaterialPropertyValue: property not found in dynamic material");
    } else {
        SetDeferredResourceValue(materialInstance->GetMaterialPropertyByName(propertyName), deferredResourceCreator);
    }
}

bool MaterialPropertySetter::IsPropertyBindingType(const std::shared_ptr<MaterialProperty>& property)
{
    const auto propertyType = property->GetPropertyType();
    return propertyType == MaterialProperty::eMaterialPropertyType::FLOAT_BINDING_PROPERTY
        || propertyType == MaterialProperty::eMaterialPropertyType::IVEC2_BINDING_PROPERTY
        || propertyType == MaterialProperty::eMaterialPropertyType::VEC2_BINDING_PROPERTY
        || propertyType == MaterialProperty::eMaterialPropertyType::VEC3_BINDING_PROPERTY;
}

void MaterialPropertySetter::SetMaterialPropertyValue(
    const std::shared_ptr<IMaterial>& materialInstance,
    const std::shared_ptr<EngineObject>& gameObjectSp,
    const std::string& gamePropertyName,
    const std::string& bindingName)
{
    ext_assert(materialInstance, "MaterialPropertySetter::SetMaterialPropertyValue: materialInstance is null");

    // first try to find material property among related to dynamic property
    if (const auto& dynamicMaterial = TryCastToDynamicMaterial(materialInstance)) {
        if (const auto property = dynamicMaterial->TryGetAnyMaterialPropertyByName(bindingName)) {
            if (IsPropertyBindingType(property)) {
                const auto bindingProperty = std::static_pointer_cast<BindingMaterialProperty>(property);
                BindingAttachmentBuilder::SetAttachment(gameObjectSp, bindingProperty->GetMaterialBinding(), gamePropertyName);
            } else
                ext_assert(false, "MaterialPropertySetter::SetMaterialPropertyValue: property is not a binding type");
        } else
            ext_assert(false, "Binding name doesn't exist: " + bindingName);
    } else {
        ext_assert(false, "MaterialPropertySetter::SetMaterialPropertyValue: materialInstance is not a dynamic material");
    }
}

void MaterialPropertySetter::SetMaterialInstancedPropertyValue(
    const std::shared_ptr<IMaterial>& materialInstance,
    const std::shared_ptr<MaterialInstanceDataProvider>& instanceDataProvider,
    const std::shared_ptr<EngineObject>& gameObjectSp,
    const std::string& gamePropertyName,
    const std::string& bindingName)
{
    ext_assert(materialInstance, "MaterialPropertySetter::SetMaterialInstancedPropertyValue: materialInstance is null");

    const auto& dynamicMaterial = TryCastToDynamicMaterial(materialInstance);
    ext_assert(
        dynamicMaterial, "MaterialPropertySetter::SetMaterialInstancedPropertyValue: materialInstance is not a dynamic material");
    const auto property = std::dynamic_pointer_cast<DynamicInstancedFloatMaterialProperty>(
        dynamicMaterial->TryGetDynamicPropertyByName(bindingName));
    ext_assert(
        property,
        "MaterialPropertySetter::SetMaterialInstancedPropertyValue: property is not a DynamicInstancedFloatMaterialProperty");
    auto floatBinding = std::make_shared<FloatPropertyBinding>(bindingName);
    property->AddInstancedBinding(floatBinding, instanceDataProvider);
    BindingAttachmentBuilder::SetAttachment(gameObjectSp, floatBinding, gamePropertyName);
}

} // namespace Graphics