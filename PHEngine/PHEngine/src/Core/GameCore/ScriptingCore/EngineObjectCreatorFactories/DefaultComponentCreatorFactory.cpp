#include "DefaultComponentCreatorFactory.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Actor.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/DirectionalLightComponent.h"
#include "Core/GameCore/Components/PlatformTraverseComponent.h"
#include "Core/GameCore/Components/HumanoidPhysicsMovementComponent.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/PlanarReflectionComponentData.h"
#include "Core/GameCore/Components/ComponentData/SpotlightComponentData.h"
#include "Core/GameCore/Components/SpotlightComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkyboxComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/WaterPlaneComponent.h"
#include "Core/GameCore/Components/UiInputComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionBoxShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionPlaneShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionSphereShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionCompoundShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/RigidBodyController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/DynamicCharacterController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Components/PhysicsComponents/RigidBodyPhysicsComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/CharacterPhysicsComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirectionalLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedPointLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedSpotlightShadowInfo.h"
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
#include "Core/GameCore/Components/ComponentCreators/ForwardShadingMeshComponentCreator.h"
#include "Core/GameCore/ScriptingCore/Common/JsonParserHelper.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

#include <unordered_map>
#include <json/json.hpp>

using namespace EngineCore;

namespace EngineCore
{
    namespace Scripts
    {
        void DefaultComponentCreatorFactory::CreateComponent(const std::weak_ptr<::EngineCore::Scene> &sceneWp,
                                                             const int32_t actorObjectId,
                                                             const std::string &componentType,
                                                             const std::string &componentDataJsonStr) const
        {
            const auto &sceneSp = sceneWp.lock();
            assert(sceneSp);
            const auto &actor = sceneSp->GetActorById(actorObjectId);
            assert(actor);

            std::unordered_map<std::string, std::shared_ptr<IComponentCreatable>> creatorsMap = {
                {"PointLightComponent", std::make_shared<LightComponentCreator<PointLightComponent>>()},
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
                {"WaterPlaneComponent", std::make_shared<ForwardShadingMeshComponentCreator<WaterPlaneComponent>>()},
                {"PlanarReflectionComponent", std::make_shared<PlanarReflectionComponentCreator<PlanarReflectionComponent>>()},
                {"InputComponent", std::make_shared<InputComponentCreator<InputComponent>>()},
                {"UiInputComponent", std::make_shared<InputComponentCreator<UiInputComponent>>()}};

            assert(creatorsMap.count(componentType));

            const auto componentDataSp = CreateComponentData(sceneSp, componentType, componentDataJsonStr);
            actor->AddComponent(sceneSp->CreateComponent_GameThread(creatorsMap.at(componentType), componentDataSp));
        }

        std::shared_ptr<ComponentData> DefaultComponentCreatorFactory::CreateComponentData(const std::shared_ptr<::EngineCore::Scene> &sceneSp,
                                                                                           const std::string &componentType,
                                                                                           const std::string &componentDataJsonStr) const
        {
            const auto &jsonObj = nlohmann::json::parse(componentDataJsonStr);
            const std::string objectName = JsonParserHelper::FromJsonToString("gameObjectName", jsonObj);

            std::shared_ptr<ComponentData> componentData;

            if ("PointLightComponent" == componentType)
            {
                const auto translation = JsonParserHelper::FromJsonToVec3("translation", jsonObj);
                const auto ambient = JsonParserHelper::FromJsonToVec3Color("ambient", jsonObj);
                const auto diffuse = JsonParserHelper::FromJsonToVec3Color("diffuse", jsonObj);
                const auto specular = JsonParserHelper::FromJsonToVec3Color("specular", jsonObj);
                const auto attenutation = JsonParserHelper::FromJsonToVec3("attenutation", jsonObj);
                const auto radianceRadius = JsonParserHelper::FromJsonToFloat("radianceRadius", jsonObj);
                std::shared_ptr<ProjectedShadowInfo> shadowInfo;
                if (jsonObj.contains("shadowAtlasSize"))
                {
                    const auto shadowAtlasSize = JsonParserHelper::FromJsonToInt("shadowAtlasSize", jsonObj);
                    const auto &pointLightTAR = TextureAtlasFactory::GetInstance()->AddTextureCubeAtlasRequest(glm::ivec2(shadowAtlasSize));
                    shadowInfo = std::make_shared<ProjectedPointLightShadowInfo>(pointLightTAR);
                }

                componentData = std::make_shared<PointLightComponentData>(objectName, translation, attenutation, radianceRadius, ambient, diffuse, specular, shadowInfo);
            }
            else if ("DirectionalLightComponent" == componentType)
            {
                const auto rotation = JsonParserHelper::FromJsonToVec3("rotation", jsonObj);
                const auto direction = JsonParserHelper::FromJsonToVec3("direction", jsonObj);
                const auto ambient = JsonParserHelper::FromJsonToVec3Color("ambient", jsonObj);
                const auto diffuse = JsonParserHelper::FromJsonToVec3Color("diffuse", jsonObj);
                const auto specular = JsonParserHelper::FromJsonToVec3Color("specular", jsonObj);
                std::shared_ptr<ProjectedShadowInfo> shadowInfo;
                if (jsonObj.contains("shadowAtlasSize"))
                {
                    const auto &cfg = EngineUtility::EngineConfigHolder::GetInstance()->GetEngineConfig();
                    const float orthoHalfExtent = cfg.ShadowOrthoProjectionHalfExtent;
                    const auto shadowAtlasSize = JsonParserHelper::FromJsonToInt("shadowAtlasSize", jsonObj);
                    const auto &directionalLightTAR = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(shadowAtlasSize));
                    shadowInfo = std::make_shared<ProjectedDirectionalLightShadowInfo>(directionalLightTAR, orthoHalfExtent);
                }

                componentData = std::make_shared<DirectionalLightComponentData>(objectName, rotation, direction, ambient, diffuse, specular, shadowInfo);
            }
            else if ("SpotlightComponent" == componentType)
            {
                const auto translation = JsonParserHelper::FromJsonToVec3("translation", jsonObj);
                const auto rotation = JsonParserHelper::FromJsonToVec3("rotation", jsonObj);
                const auto ambient = JsonParserHelper::FromJsonToVec3Color("ambient", jsonObj);
                const auto diffuse = JsonParserHelper::FromJsonToVec3Color("diffuse", jsonObj);
                const auto specular = JsonParserHelper::FromJsonToVec3Color("specular", jsonObj);
                const auto attenutation = JsonParserHelper::FromJsonToVec3("attenutation", jsonObj);
                const auto radianceRadius = JsonParserHelper::FromJsonToFloat("radianceRadius", jsonObj);
                const auto cutoff = JsonParserHelper::FromJsonToFloat("cutoff", jsonObj);
                std::shared_ptr<ProjectedShadowInfo> shadowInfo;
                if (jsonObj.contains("shadowAtlasSize"))
                {
                    const auto shadowAtlasSize = JsonParserHelper::FromJsonToInt("shadowAtlasSize", jsonObj);
                    const auto &pointLightTAR = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(shadowAtlasSize));
                    shadowInfo = std::make_shared<ProjectedSpotlightShadowInfo>(pointLightTAR);
                }

                componentData = std::make_shared<SpotlightComponentData>(objectName, translation, rotation, attenutation, radianceRadius, cutoff, ambient, diffuse, specular, shadowInfo);
            }
            else if ("StaticMeshComponent" == componentType || "SkeletalMeshComponent" == componentType)
            {
                const auto pathToMesh = JsonParserHelper::FromJsonToString("meshName", jsonObj);
                const auto translation = JsonParserHelper::FromJsonToVec3("translation", jsonObj);
                const auto rotation = JsonParserHelper::FromJsonToVec3("rotation", jsonObj);
                const auto scale = JsonParserHelper::FromJsonToVec3("scale", jsonObj);
                const auto luaScriptRelPath = JsonParserHelper::FromJsonToString("luaScriptName", jsonObj);
                const auto materialProxyId = JsonParserHelper::FromJsonToInt("materialProxyId", jsonObj);
                const auto &material = sceneSp->GetMaterialByProxyId(materialProxyId);
                assert(material);

                componentData = std::make_shared<MeshComponentData>(objectName, pathToMesh, translation, rotation, scale, luaScriptRelPath, material);
            }
            else if ("RigidBodyPhysicsComponent" == componentType || "CharacterPhysicsComponent" == componentType || "GhostPhysicsComponent" == componentType)
            {
                std::shared_ptr<PhysicsDescriptor> descriptor;
                std::shared_ptr<CollisionShapeBase> collisionShape;
                const auto collisionShapeStr = JsonParserHelper::FromJsonToString("collisionShape", jsonObj);
                if ("box" == collisionShapeStr)
                {
                    const auto halfExtent = JsonParserHelper::FromJsonToVec3("halfExtent", jsonObj);
                    collisionShape = std::make_shared<CollisionBoxShape>(halfExtent);
                }
                else if ("capsule" == collisionShapeStr)
                {
                    const auto radius = JsonParserHelper::FromJsonToFloat("radius", jsonObj);
                    const auto height = JsonParserHelper::FromJsonToFloat("height", jsonObj);
                    collisionShape = std::make_shared<CollisionCapsuleShape>(radius, height);
                }
                else if ("plane" == collisionShapeStr)
                {
                    const auto normal = JsonParserHelper::FromJsonToVec3("normal", jsonObj);
                    const auto d = JsonParserHelper::FromJsonToFloat("d", jsonObj);
                    collisionShape = std::make_shared<CollisionPlaneShape>(normal, d);
                }
                else if ("sphere" == collisionShapeStr)
                {
                    const auto radius = JsonParserHelper::FromJsonToFloat("radius", jsonObj);
                    collisionShape = std::make_shared<CollisionSphereShape>(radius);
                }
                else if ("compoundShape" == collisionShapeStr)
                {
                    collisionShape = std::make_shared<CollisionCompoundShape>();
                    // todo:
                    /*void EngineObjectCreator::AddChildShapeToCompoundShape(CollisionShapeBase * compoundShape, CollisionShapeBase * childShape,
                                                                           const glm::vec3 &translation, const glm::vec3 &rotation)
                    {
                        CollisionCompoundShape *mCompoundShape = static_cast<CollisionCompoundShape *>(compoundShape);
                        assert(mCompoundShape);
                        NoScaleEulerRotationTransform childTransform = NoScaleEulerRotationTransform(translation, rotation);
                        mCompoundShape->AddChildShape(childTransform, childShape);
                    }*/
                }

                const auto mass = JsonParserHelper::FromJsonToFloat("mass", jsonObj);
                if ("RigidBodyPhysicsComponent" == componentType)
                {
                    const auto physicsBodyType = static_cast<ePhysicsBodyType>(JsonParserHelper::FromJsonToInt("physicsBodyType", jsonObj));
                    descriptor = std::make_shared<RigidBodyController>(sceneSp->GetPhysicsWorld(), collisionShape, physicsBodyType, mass);
                }
                else if ("CharacterPhysicsComponent" == componentType)
                {
                    const auto capsuleRadius = JsonParserHelper::FromJsonToFloat("capsuleRadius", jsonObj);
                    const auto capsuleHeight = JsonParserHelper::FromJsonToFloat("capsuleHeight", jsonObj);

                    const auto stepHeight = JsonParserHelper::FromJsonToFloat("stepHeight", jsonObj);
                    descriptor = std::make_shared<DynamicCharacterController>(sceneSp->GetPhysicsWorld(), capsuleRadius, capsuleHeight, mass, stepHeight);
                }
                else if ("GhostPhysicsComponent" == componentType)
                {
                    descriptor = std::make_shared<GhostController>(sceneSp->GetPhysicsWorld(), collisionShape, mass);
                }

                componentData = std::make_shared<PhysicsComponentData>(objectName, descriptor);
            }
            else if ("HumanoidPhysicsMovementComponent" == componentType)
            {
                const auto launchDirection = JsonParserHelper::FromJsonToVec3("launchDirection", jsonObj);
                const auto cameraName = JsonParserHelper::FromJsonToString("cameraName", jsonObj);
                componentData = std::make_shared<HumanoidMovementComponentData>(objectName, launchDirection, cameraName);
            }
            else if ("PlatformTraverseComponent" == componentType)
            {
                const auto scriptName = JsonParserHelper::FromJsonToString("scriptName", jsonObj);
                componentData = std::make_shared<PlatformTraverseComponentData>(objectName, scriptName);
            }
            else if ("SkyboxComponent" == componentType)
            {
                const auto scale = JsonParserHelper::FromJsonToVec3("scale", jsonObj);
                const auto materialProxyId = JsonParserHelper::FromJsonToInt("materialProxyId", jsonObj);
                const auto &material = sceneSp->GetMaterialByProxyId(materialProxyId);
                assert(material);

                componentData = std::make_shared<SkyboxComponentData>(objectName, scale, material);
            }
            else if ("WaterPlaneComponent" == componentType)
            {
                const auto translation = JsonParserHelper::FromJsonToVec3("translation", jsonObj);
                const auto rotation = JsonParserHelper::FromJsonToVec3("rotation", jsonObj);
                const auto scale = JsonParserHelper::FromJsonToVec3("scale", jsonObj);
                const auto materialProxyId = JsonParserHelper::FromJsonToInt("materialProxyId", jsonObj);
                const auto &material = sceneSp->GetMaterialByProxyId(materialProxyId);
                assert(material);
                componentData = std::make_shared<ForwardShadingMeshComponentData>("", objectName, translation, rotation, scale, material);
            }
            else if ("PlanarReflectionComponent" == componentType)
            {
                const auto translation = JsonParserHelper::FromJsonToVec3("translation", jsonObj);
                const auto rotation = JsonParserHelper::FromJsonToVec3("rotation", jsonObj);
                const auto scale = JsonParserHelper::FromJsonToVec3("scale", jsonObj);
                const auto cameraName = JsonParserHelper::FromJsonToString("cameraName", jsonObj);
                const auto ownerCameraSp = sceneSp->GetCamera(cameraName);
                assert(ownerCameraSp);
                const auto viewPortX = JsonParserHelper::FromJsonToInt("viewPortX", jsonObj);
                const auto viewPortY = JsonParserHelper::FromJsonToInt("viewPortY", jsonObj);
                const auto viewPortWidth = JsonParserHelper::FromJsonToInt("viewPortWidth", jsonObj);
                const auto viewPortHeight = JsonParserHelper::FromJsonToInt("viewPortHeight", jsonObj);
                componentData = std::make_shared<PlanarReflectionComponentData>(objectName,
                                                                                translation,
                                                                                rotation,
                                                                                scale,
                                                                                ownerCameraSp,
                                                                                ::Graphics::ViewPortInfo(viewPortX, viewPortY, viewPortWidth, viewPortHeight));
            }
            else if ("InputComponent" == componentType ||
                     "UiInputComponent" == componentType)
            {
                componentData = std::make_shared<ComponentData>(objectName);
            }

            assert(componentData);

            return componentData;
        }
    }
}
