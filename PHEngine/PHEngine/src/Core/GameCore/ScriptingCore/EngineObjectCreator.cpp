#include "EngineObjectCreator.h"

#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewOrthographicInfo.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPerspectiveInfo.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewProjectionInfo.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

using namespace EngineUtility;

namespace EngineCore {
EngineObjectCreator::EngineObjectCreator()
{
    mDefaultComponentNames
        = {"LightComponent",
           "DirectionalLightComponent",
           "SpotlightComponent",
           "StaticMeshComponent",
           "SkeletalMeshComponent",
           "RigidBodyPhysicsComponent",
           "CharacterPhysicsComponent",
           "HumanoidPhysicsMovementComponent",
           "PlatformTraverseComponent",
           "SkyboxComponent",
           "WaterPlaneComponent",
           "PlanarReflectionComponent",
           "InputComponent"};
}

void EngineObjectCreator::SetScene(const std::weak_ptr<Scene>& scene)
{
    mSceneWp = scene;
}

void EngineObjectCreator::RegisterActorCreatorFactory(
    const std::string& factoryKey, const std::shared_ptr<IEngineActorCreatorFactory>& creatorFactoryInstance)
{
    assert(!mActorCreatorFactoriesMap.count(factoryKey));
    mActorCreatorFactoriesMap[factoryKey] = creatorFactoryInstance;
}

void EngineObjectCreator::RegisterComponentCreatorFactory(
    const std::string& factoryKey, const std::shared_ptr<IEngineComponentCreatorFactory>& creatorFactoryInstance)
{
    assert(!mComponentCreatorFactoriesMap.count(factoryKey));
    mComponentCreatorFactoriesMap[factoryKey] = creatorFactoryInstance;
}

void EngineObjectCreator::RegisterActorControllerCreatorFactory(
    const std::string& factoryKey, const std::shared_ptr<IEngineActorControllerCreatorFactory>& creatorFactoryInstance)
{
    assert(!mActorControllerCreatorFactoriesMap.count(factoryKey));
    mActorControllerCreatorFactoriesMap[factoryKey] = creatorFactoryInstance;
}

// Actor will be created and added to the scene
int32_t EngineObjectCreator::CreateActor(
    const std::string& actorType,
    const std::string& actorName,
    const glm::vec3& rootTranslation,
    const glm::vec3& rootEulerRotation,
    const glm::vec3& rootScale,
    const std::string& jsonParamStr) const
{
    assert(mActorCreatorFactoriesMap.count(actorType));
    return mActorCreatorFactoriesMap.at(actorType)->CreateActor(
        mSceneWp, actorName, rootTranslation, rootEulerRotation, rootScale, jsonParamStr);
}

void EngineObjectCreator::CreateComponent(
    const int32_t actorObjectId, const std::string& componentType, const std::string& componentDataJsonStr) const
{
    std::string factoryName = "";
    if (mDefaultComponentNames.count(componentType)) {
        factoryName = "DefaultComponentCreatorFactory";
    } else {
        factoryName = componentType;
    }
    assert(mComponentCreatorFactoriesMap.count(factoryName));
    mComponentCreatorFactoriesMap.at(factoryName)->CreateComponent(mSceneWp, actorObjectId, componentType, componentDataJsonStr);
}

void EngineObjectCreator::CreatePlanarReflectionComponent(const std::string& componentDataJsonStr) const
{
    mComponentCreatorFactoriesMap.at("DefaultComponentCreatorFactory")
        ->CreatePlanarReflectionComponent(mSceneWp, componentDataJsonStr);
}

void EngineObjectCreator::CreateThirdPersonCamera(
    const std::string& cameraName,
    const ViewPortInfo& viewPort,
    const std::shared_ptr<ViewProjectionInfo>& viewProjectionInfo,
    const float initPitchDeg,
    const float initYawDeg,
    const float camDistanceToThirdPersonTarget,
    const glm::vec3& thirdPersonTargetOffset,
    const bool bIsMainSceneCamera)
{
    if (const auto& sceneSp = mSceneWp.lock()) {
        const eCameraType cameraType
            = bIsMainSceneCamera ? eCameraType::MAIN_THIRD_PERSON_CAMERA : eCameraType::SECONDARY_THIRD_PERSON_CAMERA;
        const auto camera = std::make_shared<ThirdPersonCamera>(
            cameraName,
            cameraType,
            sceneSp,
            viewPort,
            viewProjectionInfo,
            initPitchDeg,
            initYawDeg,
            camDistanceToThirdPersonTarget,
            thirdPersonTargetOffset);

        if (bIsMainSceneCamera) {
            sceneSp->RegisterMainCamera(camera);
        } else {
            sceneSp->RegisterCamera(camera);
        }
    }
}

void EngineObjectCreator::CreateFirstPersonCamera(
    const std::string& cameraName,
    const ViewPortInfo& viewPort,
    const std::shared_ptr<ViewProjectionInfo>& viewProjectionInfo,
    const float initPitchDeg,
    const float initYawDeg,
    const glm::vec3& cameraPosition,
    const bool bIsMainSceneCamera)
{
    if (const auto& sceneSp = mSceneWp.lock()) {
        const eCameraType cameraType
            = bIsMainSceneCamera ? eCameraType::MAIN_FIRST_PERSON_CAMERA : eCameraType::SECONDARY_FIRST_PERSON_CAMERA;
        const auto camera = std::make_shared<FirstPersonCamera>(
            cameraName, cameraType, sceneSp, viewPort, viewProjectionInfo, initPitchDeg, initYawDeg, cameraPosition);

        if (bIsMainSceneCamera) {
            sceneSp->RegisterMainCamera(camera);
        } else {
            sceneSp->RegisterCamera(camera);
        }
    }
}

std::shared_ptr<ViewProjectionInfo> EngineObjectCreator::CreateViewProjectionInfo(const std::string& jsonArgs) const
{
    const auto& jsonObj = nlohmann::json::parse(jsonArgs);
    const auto& projectionType = nlohmann_utilities::GetStringFromJson(jsonObj["projectionType"]);

    if ("Perspective" == projectionType) {
        const float fov = nlohmann_utilities::GetFloatFromJson(jsonObj["FoV"]);
        const float aspectRatio = nlohmann_utilities::GetFloatFromJson(jsonObj["AspectRatio"]);
        const float nearPlane = nlohmann_utilities::GetFloatFromJson(jsonObj["NearPlane"]);
        const float farPlane = nlohmann_utilities::GetFloatFromJson(jsonObj["FarPlane"]);
        return std::make_shared<ViewPerspectiveInfo>(fov, aspectRatio, nearPlane, farPlane);
    } else if ("Orthographic" == projectionType) {
        const float left = nlohmann_utilities::GetFloatFromJson(jsonObj["left"]);
        const float right = nlohmann_utilities::GetFloatFromJson(jsonObj["right"]);
        const float bottom = nlohmann_utilities::GetFloatFromJson(jsonObj["bottom"]);
        const float top = nlohmann_utilities::GetFloatFromJson(jsonObj["top"]);
        const float zNear = nlohmann_utilities::GetFloatFromJson(jsonObj["zNear"]);
        const float zFar = nlohmann_utilities::GetFloatFromJson(jsonObj["zFar"]);
        return std::make_shared<ViewOrthographicInfo>(left, right, bottom, top, zNear, zFar);
    }

    assert(false);
    return nullptr;
}

void EngineObjectCreator::CreateActorController(
    const std::string& factoryType,
    const std::string& actorName,
    const std::string& actorControllerTypeName,
    const std::string& jsonArgs)
{
    assert(mActorControllerCreatorFactoriesMap.count(factoryType) > 0);
    mActorControllerCreatorFactoriesMap.at(factoryType)
        ->CreateActorController(mSceneWp, actorName, actorControllerTypeName, jsonArgs);
}
} // namespace EngineCore
