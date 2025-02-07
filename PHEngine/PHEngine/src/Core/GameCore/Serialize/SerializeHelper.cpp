#include "SerializeHelper.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionBoxShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionCompoundShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionSphereShape.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/EngineObjectCreator.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/MaterialProperties/FloatMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/GraphicsCore/Material/MaterialProperties/TextureMaterialProperty.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPerspectiveInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirectionalLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedPointLightShadowInfo.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <TinyLogger/LogInterface.h>

using namespace Graphics;
using namespace EnginePhysics;
using namespace EngineUtility;
using namespace TinyLogger;

namespace EngineCore {

std::shared_ptr<SerializeDataCamera> SerializeHelper::GetSerializedDataCamera(const ACamera* camera)
{
    std::shared_ptr<SerializeDataCamera> cameraData;

    if (eCameraType::SECONDARY_THIRD_PERSON_CAMERA & camera->GetCameraType()) {
        // third person camera
        auto thirdPersonCameraData = std::make_shared<SerializeDataThirdPersonCamera>();
        cameraData = thirdPersonCameraData;
        const auto thirdPersonCamera = static_cast<const ThirdPersonCamera*>(camera);
        thirdPersonCameraData->ThirdPersonTargetOffset = thirdPersonCamera->GetThirdPersonTargetOffset();
        thirdPersonCameraData->CameraDistanceToThirdPersonTarget = thirdPersonCamera->GetMaxDistanceFromTargetToCamera();
        thirdPersonCameraData->ThirdPersonTargetActorName = thirdPersonCamera->GetThirdPersonTarget()->GetName();
    } else if (eCameraType::SECONDARY_FIRST_PERSON_CAMERA & camera->GetCameraType()) {
        // first person camera
        auto firstPersonCameraData = std::make_shared<SerializeDataFirstPersonCamera>();
        cameraData = firstPersonCameraData;
        const auto firstPersonCamera = static_cast<const FirstPersonCamera*>(camera);
        firstPersonCameraData->CameraPosition = firstPersonCamera->GetEyeVector();
    } else {
        assert(false);
    }

    cameraData->CameraName = camera->GetCameraName();
    cameraData->ViewPortInfo = glm::ivec4(camera->GetViewPort());
    cameraData->InitPitchDeg = camera->GetRotationPitch();
    cameraData->InitYawDeg = camera->GetRotationYaw();
    cameraData->CameraType = camera->GetCameraTypeName();

    if (camera->GetPlanarReflectionComponent()) {
        cameraData->mPlanarReflectionComponentData
            = GetSerializedDataPlanarReflectionComponent(camera->GetPlanarReflectionComponent());
    }

    return cameraData;
}

std::shared_ptr<SerializeDataPhysicsShape>
SerializeHelper::GetSerializePhysicsShapeData(const std::shared_ptr<CollisionShapeBase>& physicsShape)
{
    std::shared_ptr<SerializeDataPhysicsShape> resultData;

    int32_t shapeType = physicsShape->GetCollisionShape()->getShapeType();

    if (SPHERE_SHAPE_PROXYTYPE == shapeType) {
        const auto sphere = std::static_pointer_cast<CollisionSphereShape>(physicsShape);
        auto shapeData = std::make_shared<SerializeDataSpherePhysicsShape>();
        shapeData->Radius = sphere->GetRadius();
        resultData = shapeData;
    } else if (BOX_SHAPE_PROXYTYPE == shapeType) {
        const auto box = std::static_pointer_cast<CollisionBoxShape>(physicsShape);
        auto shapeData = std::make_shared<SerializeDataBoxPhysicsShape>();
        shapeData->HalfExtent = box->GetHalfExtent();
        resultData = shapeData;
    } else if (CAPSULE_SHAPE_PROXYTYPE == shapeType) {
        const auto capsule = std::static_pointer_cast<CollisionCapsuleShape>(physicsShape);
        auto shapeData = std::make_shared<SerializeDataCapsulePhysicsShape>();
        shapeData->Height = capsule->GetHeight();
        shapeData->Radius = capsule->GetRadius();
        resultData = shapeData;
    } else if (COMPOUND_SHAPE_PROXYTYPE == shapeType) {
        const auto compoundShape = std::static_pointer_cast<CollisionCompoundShape>(physicsShape);
        auto shapeData = std::make_shared<SerializeDataCompoundPhysicsShape>();
        auto childShapeMap = compoundShape->GetChildShapes();

        for (const auto& phyShapeAndTransform : childShapeMap) {
            auto childShape = phyShapeAndTransform.first;
            auto childTransform = phyShapeAndTransform.second;
            SerializeDataTranslationEulerRotation serializeTransformData;
            serializeTransformData.Translation = childTransform.Translation;
            serializeTransformData.Rotation = childTransform.RotationEulerAngles;

            auto packedPhysicsShape = GetSerializePhysicsShapeData(childShape);
            SerializeDataCompoundChildShape compoundChildShape;
            compoundChildShape.Child = packedPhysicsShape;
            compoundChildShape.ChildTransform = serializeTransformData;

            shapeData->ChildrenWithRotation.emplace_back(std::move(compoundChildShape));
        }

        resultData = shapeData;
    } else {
        assert(false);
    }

    return resultData;
}

std::shared_ptr<SerializeDataPhysicsComponent>
SerializeHelper::GetSerializeDataPhysicsComponent(const PhysicsComponent* component)
{
    std::shared_ptr<SerializeDataPhysicsComponent> physCompData = std::make_shared<SerializeDataPhysicsComponent>();

    const auto& physShape = component->GetDescriptor()->GetShape();

    std::shared_ptr<SerializeDataPhysicsShape> physicsShapeData = GetSerializePhysicsShapeData(physShape);

    physCompData->PhysicsShape = physicsShapeData;
    physCompData->Mass = component->GetDescriptor()->GetMass();
    auto motionModifiers = component->GetDescriptor()->GetMotionModifiers();
    physCompData->AngularFactor = Converter::bulletToGlm(motionModifiers.AngularFactor);
    physCompData->LinearFactor = Converter::bulletToGlm(motionModifiers.LinearFactor);
    physCompData->BodyType = component->GetDescriptor()->GetPhysicsBodyType();

    return physCompData;
}

SerializeDataMaterial SerializeHelper::GetSerializeDataMaterial(std::shared_ptr<IMaterial> materialInstance)
{
    SerializeDataMaterial materialData;

    materialData.MaterialName = materialInstance->MaterialName;
    materialData.MaterialShaderName = materialInstance->MaterialShaderName;

    const auto& properties = materialInstance->GetProperties();

    for (const auto& propertyItem : properties) {
        const std::string& uniformName = propertyItem->GetPropertyName();
        std::shared_ptr<MaterialProperty> prop = propertyItem;
        auto type = prop->GetPropertyType();

        std::string uniformValue, propertyType;

        if (type == MaterialProperty::eMaterialPropertyType::TEXTURE_PROPERTY) {
            std::shared_ptr<TextureMaterialProperty> texProp = std::static_pointer_cast<TextureMaterialProperty>(prop);
            propertyType = "texture";
            uniformValue = EngineUtility::FromOsSpecificUrlToGeneral(
                Resources::TexturePool::GetInstance()->GetKeyOptional(texProp->GetValue()).value_or(""));
        } else if (type == MaterialProperty::eMaterialPropertyType::FLOAT_PROPERTY) {
            std::shared_ptr<FloatMaterialProperty> floatProp = std::static_pointer_cast<FloatMaterialProperty>(prop);
            propertyType = "float";
            uniformValue = std::to_string(floatProp->GetValue());
        }

        SerializeDataMaterial::SerializeDataMaterialProperty property;
        property.PropertyType = propertyType;
        property.Value = uniformValue;
        property.UniformName = uniformName;

        materialData.Properties.emplace_back(property);
    }

    return materialData;
}

std::shared_ptr<Actor> SerializeHelper::CreateActorFromSerializedData(const SerializeDataActor& data)
{

    const auto& actor = std::make_shared<Actor>(
        data.ActorName,
        std::make_shared<SceneComponent>(
            data.ActorName + "_RootComponent", data.RootCompTranslation, data.RootCompRotation, data.RootCompScale));
    return actor;
}

std::shared_ptr<Tweener> SerializeHelper::CreateTweenerFromSerializedData(std::shared_ptr<SerializeDataTweener> data)
{
    TweenerParser fsmParser;
    return fsmParser.ParseTweenerDescriptor(data->TweenerRelPath);
}

std::shared_ptr<SerializeDataStaticMesh> SerializeHelper::GetSerializedDataStaticMesh(const StaticMeshComponent* component)
{
    auto meshData = std::make_shared<SerializeDataStaticMesh>();
    meshData->ComponentName = component->GetEngineObjectName();
    meshData->ModelName = component->GetRenderData().mModelPath;
    meshData->Translation = component->GetTranslation();
    meshData->Rotation = component->GetRotationDegrees();
    meshData->Scale = component->GetScale();
    meshData->LuaScriptName = ""; // TODO: for now

    const SerializeDataMaterial& material = SerializeHelper::GetSerializeDataMaterial(component->GetMaterial());

    meshData->MeshMaterial = material;

    return meshData;
}

std::shared_ptr<SerializeDataSkeletalMesh> SerializeHelper::GetSerializedDataSkeletalMesh(const SkeletalMeshComponent* component)
{
    auto meshData = std::make_shared<SerializeDataSkeletalMesh>();
    meshData->ComponentName = component->GetEngineObjectName();
    meshData->ModelName = component->GetRenderData().mModelPath;
    meshData->Translation = component->GetTranslation();
    meshData->Rotation = component->GetRotationDegrees();
    meshData->Scale = component->GetScale();
    meshData->LuaScriptName = component->LuaScriptName;

    const SerializeDataMaterial& material = SerializeHelper::GetSerializeDataMaterial(component->GetMaterial());

    meshData->MeshMaterial = material;

    return meshData;
}

std::shared_ptr<SerializeDataPlanarReflectionComponent>
SerializeHelper::GetSerializedDataPlanarReflectionComponent(const std::shared_ptr<PlanarReflectionComponent>& component)
{
    const auto& planarReflectionData = std::make_shared<SerializeDataPlanarReflectionComponent>();
    planarReflectionData->ComponentName = component->GetEngineObjectName();
    planarReflectionData->Translation = component->GetTranslation();
    planarReflectionData->EulerAnglesRotation = component->GetRotationDegrees();
    planarReflectionData->Scale = component->GetScale();
    const auto& ownerCameraSp = component->GetOwnerCameraWp().lock();
    assert(ownerCameraSp);
    planarReflectionData->OwnerCameraName = ownerCameraSp->GetCameraName();
    const auto& viewPortInfo = component->GetRenderTargetViewPortInfo();
    planarReflectionData->ViewPortInfo
        = glm::vec4(viewPortInfo.OriginX, viewPortInfo.OriginY, viewPortInfo.Width, viewPortInfo.Height);
    return planarReflectionData;
}

std::shared_ptr<ACamera> SerializeHelper::CreateCameraFromSerializedData(
    std::shared_ptr<Scene> scene, std::shared_ptr<SerializeDataCamera> data, bool& outIsMainSceneCamera)
{
    std::shared_ptr<ACamera> result;

    const auto& cameraTypeName = data->CameraType;

    static const std::map<std::string, eCameraType> cameraTypeMap = {
        {"FirstPersonCamera", eCameraType::SECONDARY_FIRST_PERSON_CAMERA},
        {"MainFirstPersonCamera", eCameraType::MAIN_FIRST_PERSON_CAMERA},
        {"MainThirdPersonCamera", eCameraType::MAIN_THIRD_PERSON_CAMERA},
        {"ThirdPersonCamera", eCameraType::SECONDARY_THIRD_PERSON_CAMERA},
    };

    const eCameraType cameraType = cameraTypeMap.at(cameraTypeName);

    if ((eCameraType::SECONDARY_FIRST_PERSON_CAMERA & cameraType) == eCameraType::SECONDARY_FIRST_PERSON_CAMERA) {
        auto fpCameraData = std::static_pointer_cast<SerializeDataFirstPersonCamera>(data);
        assert(fpCameraData);
        result = std::make_shared<FirstPersonCamera>(
            fpCameraData->CameraName,
            cameraType,
            scene,
            ViewPortInfo(fpCameraData->ViewPortInfo),
            std::make_shared<ViewPerspectiveInfo>(glm::radians<float>(60.0f), 16.0f / 9.0f, 0.1f, 500.0f),
            fpCameraData->InitPitchDeg,
            fpCameraData->InitYawDeg,
            fpCameraData->CameraPosition);
    }
    if ((eCameraType::SECONDARY_THIRD_PERSON_CAMERA & cameraType) == eCameraType::SECONDARY_THIRD_PERSON_CAMERA) {
        auto thpCameraData = std::static_pointer_cast<SerializeDataThirdPersonCamera>(data);
        assert(thpCameraData);

        const auto& thirdPersonCamera = std::make_shared<ThirdPersonCamera>(
            thpCameraData->CameraName,
            cameraType,
            scene,
            ViewPortInfo(thpCameraData->ViewPortInfo),
            std::make_shared<ViewPerspectiveInfo>(glm::radians<float>(60.0f), 16.0f / 9.0f, 0.1f, 500.0f),
            thpCameraData->InitPitchDeg,
            thpCameraData->InitYawDeg,
            thpCameraData->CameraDistanceToThirdPersonTarget,
            thpCameraData->ThirdPersonTargetOffset);
        thirdPersonCamera->SetThirdPersonTargetDeferred(thpCameraData->ThirdPersonTargetActorName);

        result = thirdPersonCamera;
    } else {
        assert(false);
    }

    return result;
}

std::shared_ptr<Component>
SerializeHelper::CreateComponentFromSerializedData(std::shared_ptr<Scene> scene, std::shared_ptr<SerializeDataBase> data)
{
    std::shared_ptr<Component> result;

    return result;
}

std::shared_ptr<CollisionShapeBase>
SerializeHelper::CreatePhysicsShape(const std::shared_ptr<SerializeDataPhysicsShape>& serDataShape)
{
    std::shared_ptr<CollisionShapeBase> result;

    switch (serDataShape->GetShapeProxyType()) {
    case BOX_SHAPE_PROXYTYPE: {
        auto shapeData = std::static_pointer_cast<SerializeDataBoxPhysicsShape>(serDataShape);
        result = std::make_shared<CollisionBoxShape>(shapeData->HalfExtent);
        break;
    }
    case CAPSULE_SHAPE_PROXYTYPE: {
        auto shapeData = std::static_pointer_cast<SerializeDataCapsulePhysicsShape>(serDataShape);
        result = std::make_shared<CollisionCapsuleShape>(shapeData->Radius, shapeData->Height);
        break;
    }
    case SPHERE_SHAPE_PROXYTYPE: {
        auto shapeData = std::static_pointer_cast<SerializeDataSpherePhysicsShape>(serDataShape);
        result = std::make_shared<CollisionSphereShape>(shapeData->Radius);
        break;
    }
    case COMPOUND_SHAPE_PROXYTYPE: {
        auto shapeData = std::static_pointer_cast<SerializeDataCompoundPhysicsShape>(serDataShape);
        const auto& childrenData = shapeData->ChildrenWithRotation;

        auto compoundPhyShape = std::make_shared<CollisionCompoundShape>();

        for (const auto& childData : childrenData) {
            const auto childPhysicsShape = CreatePhysicsShape(childData.Child);
            compoundPhyShape->AddChildShape(
                NoScaleEulerRotationTransform(childData.ChildTransform.Translation, childData.ChildTransform.Rotation),
                childPhysicsShape);
        }

        result = compoundPhyShape;

        break;
    }
    default:
        assert(false);
        break;
    }

    return result;
}

std::shared_ptr<IMaterial> SerializeHelper::CreateMaterialFromSerializedData(const SerializeDataMaterial& materialData)
{
    const auto material = std::make_shared<IMaterial>(materialData.MaterialName, materialData.MaterialShaderName);

    for (const auto& property : materialData.Properties) {
        if (property.PropertyType == "texture" && property.Value != "") {
            material->PushMaterialProperty(std::make_shared<TextureMaterialProperty>(property.UniformName));
            const auto& texture = TexturePool::GetInstance()->GetOrAllocateResource(property.Value);

            MaterialPropertySetter::SetMaterialPropertyValue(material, property.UniformName, texture);
        } else if (property.PropertyType == "float") {
            material->PushMaterialProperty(std::make_shared<FloatMaterialProperty>(property.UniformName));
            MaterialPropertySetter::SetMaterialPropertyValue(material, property.UniformName, std::stof(property.Value));
        }
    }

    return material;
}

std::vector<std::string> SerializeHelper::GetSerializedAllocatedResources(const SerializeAllocatedResources& allocatedResources)
{
    std::vector<std::string> resourceNames;
    resourceNames.reserve(allocatedResources.ResourceNames.size());
    for (const auto& name : allocatedResources.ResourceNames) {
        if (std::string::npos != EngineUtility::IndexOf(name, ",")) {
            const std::vector<std::string>& splitedNames = EngineUtility::Split(name, ',');
            for (std::string splitName : splitedNames) {
                splitName = EngineUtility::TrimEnd(splitName);
                resourceNames.emplace_back(std::move(splitName));
            }
        } else {
            resourceNames.emplace_back(name);
        }
    }
    return resourceNames;
}
} // namespace EngineCore
