#include "DefaultComponentCreatorFactory.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/CubemapComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/InputComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/LightComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ParticleSystemComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PlanarReflectionComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PlatformTraverseComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/SkeletalMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/SkyboxComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/PlanarReflectionComponentData.h"
#include "Core/GameCore/Components/ComponentData/SpotlightComponentData.h"
#include "Core/GameCore/Components/DirectionalLightComponent.h"
#include "Core/GameCore/Components/HumanoidPhysicsMovementComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/CharacterPhysicsComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/RigidBodyPhysicsComponent.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/GameCore/Components/PlatformTraverseComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkyboxComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/WaterPlaneComponent.h"
#include "Core/GameCore/Components/SpotlightComponent.h"
#include "Core/GameCore/Components/UiInputComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/DynamicCharacterController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/RigidBodyController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionBoxShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionCompoundShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionPlaneShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionSphereShape.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirectionalLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedPointLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedSpotlightShadowInfo.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <unordered_map>

using namespace EngineCore;

namespace EngineCore {
namespace Scripts {
void DefaultComponentCreatorFactory::CreateComponent(
    const std::weak_ptr<::EngineCore::Scene>& sceneWp,
    const int32_t actorObjectId,
    const std::string& componentType,
    const std::string& componentDataJsonStr) const
{
    const auto& sceneSp = sceneWp.lock();
    assert(sceneSp);
    const auto& actor = sceneSp->GetActorById(actorObjectId);
    assert(actor);

    std::unordered_map<std::string, std::shared_ptr<IComponentCreatable>> creatorsMap
        = {{"PointLightComponent", std::make_shared<LightComponentCreator<PointLightComponent>>()},
           {"DirectionalLightComponent", std::make_shared<LightComponentCreator<DirectionalLightComponent>>()},
           {"SpotlightComponent", std::make_shared<LightComponentCreator<SpotlightComponent>>()},
           {"StaticMeshComponent", std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true)},
           {"SkeletalMeshComponent", std::make_shared<SkeletalMeshComponentCreator<SkeletalMeshComponent>>()},
           {"RigidBodyPhysicsComponent", std::make_shared<PhysicsComponentCreator<RigidBodyPhysicsComponent>>()},
           {"CharacterPhysicsComponent", std::make_shared<PhysicsComponentCreator<CharacterPhysicsComponent>>()},
           {"GhostPhysicsComponent", std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>()},
           {"HumanoidPhysicsMovementComponent", std::make_shared<MovementComponentCreator<HumanoidPhysicsMovementComponent>>()},
           {"PlatformTraverseComponent", std::make_shared<PlatformTraverseComponentCreator<PlatformTraverseComponent>>()},
           {"SkyboxComponent", std::make_shared<SkyboxComponentCreator<SkyboxComponent>>()},
           {"WaterPlaneComponent", std::make_shared<StaticMeshComponentCreator<WaterPlaneComponent>>(false)},
           {"InputComponent", std::make_shared<InputComponentCreator<InputComponent>>()},
           {"UiInputComponent", std::make_shared<InputComponentCreator<UiInputComponent>>()}};

    assert(creatorsMap.count(componentType));

    const auto componentDataSp = CreateComponentData(sceneSp, componentType, componentDataJsonStr);
    actor->AddComponent(sceneSp->CreateComponent_GameThread(creatorsMap.at(componentType), componentDataSp));
}

void DefaultComponentCreatorFactory::CreatePlanarReflectionComponent(
    const std::weak_ptr<::EngineCore::Scene>& sceneWp, const std::string& componentDataJsonStr) const
{
    const auto& sceneSp = sceneWp.lock();
    assert(sceneSp);
    const auto& jsonObj = nlohmann::json::parse(componentDataJsonStr);
    const std::string objectName = nlohmann_utilities::GetStringFromJson(jsonObj["gameObjectName"]);
    const auto& componentCreatorSp = std::make_shared<PlanarReflectionComponentCreator<PlanarReflectionComponent>>();
    const auto translation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["translation"]);
    const auto rotation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["rotation"]);
    const auto scale = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["scale"]);
    const auto cameraName = nlohmann_utilities::GetStringFromJson(jsonObj["cameraName"]);
    const auto ownerCameraSp = sceneSp->GetCamera(cameraName);
    assert(ownerCameraSp);
    const auto viewPortX = nlohmann_utilities::GetIntFromJson(jsonObj["viewPortX"]);
    const auto viewPortY = nlohmann_utilities::GetIntFromJson(jsonObj["viewPortY"]);
    const auto viewPortWidth = nlohmann_utilities::GetIntFromJson(jsonObj["viewPortWidth"]);
    const auto viewPortHeight = nlohmann_utilities::GetIntFromJson(jsonObj["viewPortHeight"]);
    const auto& componentData = std::make_shared<PlanarReflectionComponentData>(
        objectName,
        translation,
        rotation,
        scale,
        ownerCameraSp,
        ::Graphics::ViewPortInfo(viewPortX, viewPortY, viewPortWidth, viewPortHeight));

    sceneSp->CreateComponent_GameThread(componentCreatorSp, componentData);
}

std::shared_ptr<ComponentData> DefaultComponentCreatorFactory::CreateComponentData(
    const std::shared_ptr<::EngineCore::Scene>& sceneSp,
    const std::string& componentType,
    const std::string& componentDataJsonStr) const
{
    const auto& jsonObj = nlohmann::json::parse(componentDataJsonStr);
    const std::string objectName = nlohmann_utilities::GetStringFromJson(jsonObj["gameObjectName"]);

    std::shared_ptr<ComponentData> componentData;

    if ("PointLightComponent" == componentType) {
        const auto translation = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["translation"]);
        const auto ambient = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["ambient"]);
        const auto diffuse = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["diffuse"]);
        const auto specular = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["specular"]);
        const auto attenutation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["attenutation"]);
        const auto radianceRadius = nlohmann_utilities::GetFloatFromJson(jsonObj["radianceRadius"]);
        const bool isEnabled = static_cast<bool>(nlohmann_utilities::GetIntFromJson(jsonObj["is_enabled"]));
        const bool isVisible = static_cast<bool>(nlohmann_utilities::GetIntFromJson(jsonObj["is_visible"]));
        std::shared_ptr<ProjectedShadowInfo> shadowInfo;
        if (jsonObj.contains("shadowAtlasSize")) {
            const auto shadowAtlasSize = nlohmann_utilities::GetIntFromJson(jsonObj["shadowAtlasSize"]);
            const auto& pointLightTAR
                = TextureAtlasFactory::GetInstance()->AddTextureCubeAtlasRequest(glm::ivec2(shadowAtlasSize));
            shadowInfo = std::make_shared<ProjectedPointLightShadowInfo>(pointLightTAR);
        }

        componentData = std::make_shared<PointLightComponentData>(
            objectName, translation, attenutation, radianceRadius, ambient, diffuse, specular, shadowInfo, isEnabled, isVisible);
    } else if ("DirectionalLightComponent" == componentType) {
        const auto rotation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["rotation"]);
        const auto direction = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["direction"]);
        const auto ambient = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["ambient"]);
        const auto diffuse = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["diffuse"]);
        const auto specular = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["specular"]);
        const bool isEnabled = (bool)nlohmann_utilities::GetIntFromJson(jsonObj["is_enabled"]);
        const bool isVisible = (bool)nlohmann_utilities::GetIntFromJson(jsonObj["is_visible"]);
        std::shared_ptr<ProjectedShadowInfo> shadowInfo;
        if (jsonObj.contains("shadowAtlasSize")) {
            const auto& cfg = EngineUtility::EngineConfigHolder::GetInstance()->GetEngineConfig();
            const float orthoHalfExtent = cfg.ShadowOrthoProjectionHalfExtent;
            const auto shadowAtlasSize = nlohmann_utilities::GetIntFromJson(jsonObj["shadowAtlasSize"]);
            const auto& directionalLightTAR
                = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(shadowAtlasSize));
            shadowInfo = std::make_shared<ProjectedDirectionalLightShadowInfo>(directionalLightTAR, orthoHalfExtent);
        }

        componentData = std::make_shared<DirectionalLightComponentData>(
            objectName, rotation, direction, ambient, diffuse, specular, shadowInfo, isEnabled, isVisible);
    } else if ("SpotlightComponent" == componentType) {
        const auto translation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["translation"]);
        const auto rotation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["rotation"]);
        const auto ambient = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["ambient"]);
        const auto diffuse = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["diffuse"]);
        const auto specular = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["specular"]);
        const auto attenutation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["attenutation"]);
        const auto radianceRadius = nlohmann_utilities::GetFloatFromJson(jsonObj["radianceRadius"]);
        const auto cutoff = nlohmann_utilities::GetFloatFromJson(jsonObj["cutoff"]);
        const bool isEnabled = static_cast<bool>(nlohmann_utilities::GetIntFromJson(jsonObj["is_enabled"]));
        const bool isVisible = static_cast<bool>(nlohmann_utilities::GetIntFromJson(jsonObj["is_visible"]));
        std::shared_ptr<ProjectedShadowInfo> shadowInfo;
        if (jsonObj.contains("shadowAtlasSize")) {
            const auto shadowAtlasSize = nlohmann_utilities::GetIntFromJson(jsonObj["shadowAtlasSize"]);
            const auto& pointLightTAR = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(shadowAtlasSize));
            shadowInfo = std::make_shared<ProjectedSpotlightShadowInfo>(pointLightTAR);
        }

        componentData = std::make_shared<SpotlightComponentData>(
            objectName,
            translation,
            rotation,
            attenutation,
            radianceRadius,
            cutoff,
            ambient,
            diffuse,
            specular,
            shadowInfo,
            isEnabled,
            isVisible);
    } else if ("StaticMeshComponent" == componentType || "SkeletalMeshComponent" == componentType) {
        const auto pathToMesh = nlohmann_utilities::GetStringFromJson(jsonObj["meshName"]);
        const auto translation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["translation"]);
        const auto rotation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["rotation"]);
        const auto scale = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["scale"]);
        const auto luaScriptRelPath = nlohmann_utilities::GetStringFromJson(jsonObj["luaScriptName"]);
        const auto materialProxyId = nlohmann_utilities::GetIntFromJson(jsonObj["materialProxyId"]);
        const auto& material = sceneSp->GetMaterialByProxyId(materialProxyId);
        assert(material);

        componentData = std::make_shared<MeshComponentData>(
            objectName, pathToMesh, translation, rotation, scale, luaScriptRelPath, material);
    } else if ("RigidBodyPhysicsComponent" == componentType || "GhostPhysicsComponent" == componentType) {
        std::shared_ptr<PhysicsDescriptor> descriptor;
        std::shared_ptr<CollisionShapeBase> collisionShape;
        const auto collisionShapeStr = nlohmann_utilities::GetStringFromJson(jsonObj["collisionShape"]);
        if ("compoundShape" == collisionShapeStr) {
            const auto& compoundCollisionShape = std::make_shared<CollisionCompoundShape>();

            const auto& compoundShapeRoot = jsonObj["subshapes"];
            for (auto it = compoundShapeRoot.cbegin(); it != compoundShapeRoot.cend(); ++it) {
                const auto& subShapeRootName = nlohmann_utilities::GetStringFromJson(it->at("collisionShape"));
                const auto& collisionSubShape = collisionShape = CreateCollisionShapeFromJson(*it, subShapeRootName);
                const auto& subShapeTranslation = nlohmann_utilities::GetXyzFromJsonMap(it->at("translation"));
                const auto& subShapeRotation = nlohmann_utilities::GetXyzFromJsonMap(it->at("rotation"));
                compoundCollisionShape->AddChildShape(
                    NoScaleEulerRotationTransform(subShapeTranslation, subShapeRotation), collisionSubShape);
            }
            collisionShape = compoundCollisionShape;
        } else {
            collisionShape = CreateCollisionShapeFromJson(jsonObj, collisionShapeStr);
        }

        const auto mass = nlohmann_utilities::GetFloatFromJson(jsonObj["mass"]);
        if ("RigidBodyPhysicsComponent" == componentType) {
            const auto physicsBodyType
                = static_cast<ePhysicsBodyType>(nlohmann_utilities::GetIntFromJson(jsonObj["physicsBodyType"]));
            descriptor = std::make_shared<RigidBodyController>(sceneSp->GetPhysicsWorld(), collisionShape, physicsBodyType, mass);
        } else if ("GhostPhysicsComponent" == componentType) {
            descriptor = std::make_shared<GhostController>(sceneSp->GetPhysicsWorld(), collisionShape, mass);
        }

        componentData = std::make_shared<PhysicsComponentData>(objectName, descriptor);
    } else if ("CharacterPhysicsComponent" == componentType) {
        const auto capsuleRadius = nlohmann_utilities::GetFloatFromJson(jsonObj["capsuleRadius"]);
        const auto capsuleHeight = nlohmann_utilities::GetFloatFromJson(jsonObj["capsuleHeight"]);
        const auto mass = nlohmann_utilities::GetFloatFromJson(jsonObj["mass"]);
        const auto stepHeight = nlohmann_utilities::GetFloatFromJson(jsonObj["stepHeight"]);
        const auto& descriptor = std::make_shared<DynamicCharacterController>(
            sceneSp->GetPhysicsWorld(), capsuleRadius, capsuleHeight, mass, stepHeight);
        componentData = std::make_shared<PhysicsComponentData>(objectName, descriptor);
    } else if ("HumanoidPhysicsMovementComponent" == componentType) {
        const auto launchDirection = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["launchDirection"]);
        const auto cameraName = nlohmann_utilities::GetStringFromJson(jsonObj["cameraName"]);
        componentData = std::make_shared<HumanoidMovementComponentData>(objectName, launchDirection, cameraName);
    } else if ("PlatformTraverseComponent" == componentType) {
        const auto scriptName = nlohmann_utilities::GetStringFromJson(jsonObj["scriptName"]);
        std::vector<std::tuple<std::string, EulerAnglesTransform, float>> routePoints;
        const auto& routePointsRoot = jsonObj.at("routePoints");
        for (auto it = routePointsRoot.cbegin(); it != routePointsRoot.cend(); ++it) {
            const auto routeName = it.key();
            const glm::vec3& translation = nlohmann_utilities::GetXyzFromJsonMap(it->at("translation"));
            const glm::vec3& rotation = nlohmann_utilities::GetXyzFromJsonMap(it->at("rotation"));
            const glm::vec3& scale = nlohmann_utilities::GetXyzFromJsonMap(it->at("scale"));
            const float transitionTime = nlohmann_utilities::GetFloatFromJson(it->at("transitionTime"));
            routePoints.emplace_back(
                std::make_tuple(routeName, EulerAnglesTransform(translation, rotation, scale), transitionTime));
        }
        componentData = std::make_shared<PlatformTraverseComponentData>(objectName, routePoints);
    } else if ("SkyboxComponent" == componentType) {
        const auto scale = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["scale"]);
        const auto materialProxyId = nlohmann_utilities::GetIntFromJson(jsonObj["materialProxyId"]);
        const auto& material = sceneSp->GetMaterialByProxyId(materialProxyId);
        assert(material);

        componentData = std::make_shared<SkyboxComponentData>(objectName, scale, material);
    } else if ("WaterPlaneComponent" == componentType) {
        const auto translation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["translation"]);
        const auto rotation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["rotation"]);
        const auto scale = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["scale"]);
        const auto materialProxyId = nlohmann_utilities::GetIntFromJson(jsonObj["materialProxyId"]);
        const auto& material = sceneSp->GetMaterialByProxyId(materialProxyId);
        assert(material);
        componentData = std::make_shared<MeshComponentData>(objectName, "", translation, rotation, scale, "", material);
    } else if ("InputComponent" == componentType || "UiInputComponent" == componentType) {
        componentData = std::make_shared<ComponentData>(objectName);
    }

    assert(componentData);

    return componentData;
}

std::shared_ptr<CollisionShapeBase> DefaultComponentCreatorFactory::CreateCollisionShapeFromJson(
    const nlohmann::json& shapeRoot, const std::string& collisionShapeName) const
{
    std::shared_ptr<CollisionShapeBase> collisionShape;
    if ("box" == collisionShapeName) {
        const auto halfExtent = nlohmann_utilities::GetXyzFromJsonMap(shapeRoot["halfExtent"]);
        collisionShape = std::make_shared<CollisionBoxShape>(halfExtent);
    } else if ("capsule" == collisionShapeName) {
        const auto radius = nlohmann_utilities::GetFloatFromJson(shapeRoot["radius"]);
        const auto height = nlohmann_utilities::GetFloatFromJson(shapeRoot["height"]);
        collisionShape = std::make_shared<CollisionCapsuleShape>(radius, height);
    } else if ("plane" == collisionShapeName) {
        const auto normal = nlohmann_utilities::GetXyzFromJsonMap(shapeRoot["normal"]);
        const auto d = nlohmann_utilities::GetFloatFromJson(shapeRoot["d"]);
        collisionShape = std::make_shared<CollisionPlaneShape>(normal, d);
    } else if ("sphere" == collisionShapeName) {
        const auto radius = nlohmann_utilities::GetFloatFromJson(shapeRoot["radius"]);
        collisionShape = std::make_shared<CollisionSphereShape>(radius);
    }
    return collisionShape;
}
} // namespace Scripts
} // namespace EngineCore
