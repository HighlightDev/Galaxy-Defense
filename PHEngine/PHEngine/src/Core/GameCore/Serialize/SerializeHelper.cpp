#include "SerializeHelper.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhySphereShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyBoxShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyCompoundShape.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GameCore/ScriptingCore/EngineObjectCreator.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GlobalSettings.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirectionalLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedPointLightShadowInfo.h"
#include "Core/GraphicsCore/Material/MaterialProperties/TextureMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/FloatMaterialProperty.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/MainThirdPersonCamera.h"
#include "Core/IoCore/FolderManager.h"

#include <TinyLogger/LogInterface.h>

using namespace Graphics;
using namespace EnginePhysics;

namespace Game
{

   std::shared_ptr<SerializeDataCamera> SerializeHelper::GetSerializedDataCamera(const ACamera *camera)
   {
      std::shared_ptr<SerializeDataCamera> cameraData;

      if (ACamera::CameraType::SECONDARY_THIRD_PERSON_CAMERA & camera->GetCameraType())
      {
         // third person camera
         auto thirdPersonCameraData = std::make_shared<SerializeDataThirdPersonCamera>();
         cameraData = thirdPersonCameraData;
         const auto thirdPersonCamera = static_cast<const ThirdPersonCamera *>(camera);
         thirdPersonCameraData->ThirdPersonTargetOffset = thirdPersonCamera->GetThirdPersonTargetOffset();
         thirdPersonCameraData->CameraDistanceToThirdPersonTarget = thirdPersonCamera->GetMaxDistanceFromTargetToCamera();
         thirdPersonCameraData->ThirdPersonTargetActorName = thirdPersonCamera->GetThirdPersonTarget()->GetName();
      }
      else if (ACamera::CameraType::SECONDARY_FIRST_PERSON_CAMERA & camera->GetCameraType())
      {
         // first person camera
         auto firstPersonCameraData = std::make_shared<SerializeDataFirstPersonCamera>();
         cameraData = firstPersonCameraData;
         const auto firstPersonCamera = static_cast<const FirstPersonCamera *>(camera);
         firstPersonCameraData->CameraPosition = firstPersonCamera->GetEyeVector();
      }
      else
      {
         assert(false);
      }

      cameraData->CameraName = camera->GetCameraName();
      cameraData->ViewPortInfo = glm::ivec4(camera->GetViewPort());
      cameraData->InitPitchDeg = camera->GetRotationPitch();
      cameraData->InitYawDeg = camera->GetRotationYaw();
      cameraData->CameraType = camera->GetCameraTypeName();

      if (camera->GetPlanarReflectionComponent())
      {
         cameraData->mPlanarReflectionComponentData = GetSerializedDataPlanarReflectionComponent(camera->GetPlanarReflectionComponent().get());
      }

      return cameraData;
   }

   std::shared_ptr<SerializeDataPhysicsShape> SerializeHelper::GetSerializePhysicsShapeData(PhysicsShapeBase *physicsShape)
   {
      std::shared_ptr<SerializeDataPhysicsShape> resultData;

      int32_t shapeType = physicsShape->GetCollisionShape()->getShapeType();

      if (SPHERE_SHAPE_PROXYTYPE == shapeType)
      {
         PhySphereShape *sphere = static_cast<PhySphereShape *>(physicsShape);
         auto shapeData = std::make_shared<SerializeDataSpherePhysicsShape>();
         shapeData->Radius = sphere->GetRadius();
         resultData = shapeData;
      }
      else if (BOX_SHAPE_PROXYTYPE == shapeType)
      {
         PhyBoxShape *box = static_cast<PhyBoxShape *>(physicsShape);
         auto shapeData = std::make_shared<SerializeDataBoxPhysicsShape>();
         shapeData->HalfExtent = box->GetHalfExtent();
         resultData = shapeData;
      }
      else if (CAPSULE_SHAPE_PROXYTYPE == shapeType)
      {
         PhyCapsuleShape *capsule = static_cast<PhyCapsuleShape *>(physicsShape);
         auto shapeData = std::make_shared<SerializeDataCapsulePhysicsShape>();
         shapeData->Height = capsule->GetHeight();
         shapeData->Radius = capsule->GetRadius();
         resultData = shapeData;
      }
      else if (COMPOUND_SHAPE_PROXYTYPE == shapeType)
      {
         PhyCompoundShape *compoundShape = static_cast<PhyCompoundShape *>(physicsShape);
         auto shapeData = std::make_shared<SerializeDataCompoundPhysicsShape>();
         auto childShapeMap = compoundShape->GetChildShapes();

         for (const auto &phyShapeAndTransform : childShapeMap)
         {
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
      }
      else
      {
         assert(false);
      }

      return resultData;
   }

   std::shared_ptr<SerializeDataPhysicsComponent> SerializeHelper::GetSerializeDataPhysicsComponent(const PhysicsComponent *component)
   {
      std::shared_ptr<SerializeDataPhysicsComponent> physCompData = std::make_shared<SerializeDataPhysicsComponent>();

      PhysicsShapeBase *physShape = component->GetDescriptor()->GetShape();

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

      const auto &properties = materialInstance->GetProperties();

      for (const auto &propertyItem : properties)
      {
         const std::string &uniformName = propertyItem->GetPropertyName();
         std::shared_ptr<MaterialProperty> prop = propertyItem;
         auto type = prop->GetPropertyType();

         std::string uniformValue, propertyType;

         if (type == MaterialProperty::eMaterialPropertyType::TEXTURE_PROPERTY)
         {
            std::shared_ptr<TextureMaterialProperty> texProp = std::static_pointer_cast<TextureMaterialProperty>(prop);
            propertyType = "texture";
            uniformValue = EngineUtility::FromOsSpecificUrlToGeneral(
                Resources::TexturePool::GetInstance()->GetKey(
                    texProp->GetValue()));
         }
         else if (type == MaterialProperty::eMaterialPropertyType::FLOAT_PROPERTY)
         {
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

   std::shared_ptr<Actor> SerializeHelper::CreateActorFromSerializedData(const SerializeDataActor &data)
   {

      const auto &actor = std::make_shared<Actor>(data.ActorName,
                                                  std::make_shared<SceneComponent>(data.ActorName + "_RootComponent", data.RootCompTranslation, data.RootCompRotation, data.RootCompScale));
      return actor;
   }

   std::shared_ptr<Tweener> SerializeHelper::CreateTweenerFromSerializedData(std::shared_ptr<SerializeDataTweener> data)
   {
      TweenerParser fsmParser;
      return fsmParser.ParseTweenerDescriptor(data->TweenerRelPath);
   }

   std::shared_ptr<SerializeDataStaticMesh> SerializeHelper::GetSerializedDataStaticMesh(const StaticMeshComponent *component)
   {
      auto meshData = std::make_shared<SerializeDataStaticMesh>();
      meshData->ComponentName = component->GetGameObjectName();
      const std::string &generalUrlToModel = EngineUtility::FromOsSpecificUrlToGeneral(
          MeshPool::GetInstance()->GetKey(
              component->GetRenderData().m_skin));
      meshData->ModelName = generalUrlToModel;
      meshData->Translation = component->GetTranslation();
      meshData->Rotation = component->GetRotationEuler();
      meshData->Scale = component->GetScale();
      meshData->LuaScriptName = ""; // TODO: for now

      const SerializeDataMaterial &material = SerializeHelper::GetSerializeDataMaterial(component->GetMaterial());

      meshData->MeshMaterial = material;

      return meshData;
   }

   std::shared_ptr<SerializeDataSkeletalMesh> SerializeHelper::GetSerializedDataSkeletalMesh(const SkeletalMeshComponent *component)
   {
      auto meshData = std::make_shared<SerializeDataSkeletalMesh>();
      meshData->ComponentName = component->GetGameObjectName();
      const std::string &generalUrlToModel = EngineUtility::FromOsSpecificUrlToGeneral(
          MeshPool::GetInstance()->GetKey(
              component->GetRenderData().m_skin));
      meshData->ModelName = generalUrlToModel;
      meshData->Translation = component->GetTranslation();
      meshData->Rotation = component->GetRotationEuler();
      meshData->Scale = component->GetScale();
      meshData->LuaScriptName = component->LuaScriptName;

      const SerializeDataMaterial &material = SerializeHelper::GetSerializeDataMaterial(component->GetMaterial());

      meshData->MeshMaterial = material;

      return meshData;
   }

   std::shared_ptr<SerializeDataPlanarReflectionComponent> SerializeHelper::GetSerializedDataPlanarReflectionComponent(const PlanarReflectionComponent *component)
   {
      const auto &planarReflectionData = std::make_shared<SerializeDataPlanarReflectionComponent>();
      planarReflectionData->ComponentName = component->GetGameObjectName();
      planarReflectionData->Translation = component->GetTranslation();
      planarReflectionData->EulerAnglesRotation = component->GetRotationEuler();
      planarReflectionData->Scale = component->GetScale();
      planarReflectionData->OwnerCameraName = component->GetOwnerCamera()->GetCameraName();
      const auto &viewPortInfo = component->GetRenderTargetViewPortInfo();
      planarReflectionData->ViewPortInfo = glm::vec4(viewPortInfo.OriginX, viewPortInfo.OriginY, viewPortInfo.Width, viewPortInfo.Height);
      return planarReflectionData;
   }

   std::shared_ptr<ACamera> SerializeHelper::CreateCameraFromSerializedData(std::shared_ptr<Scene> scene, std::shared_ptr<SerializeDataCamera> data, bool &outIsMainSceneCamera)
   {
      std::shared_ptr<ACamera> result;

      const auto &cameraTypeName = data->CameraType;

      if (cameraTypeName == "FirstPersonCamera")
      {
         auto fpCameraData = std::static_pointer_cast<SerializeDataFirstPersonCamera>(data);
         assert(fpCameraData);
         auto fpCamera = std::make_shared<FirstPersonCamera>(fpCameraData->CameraName, scene, ViewPortInfo(fpCameraData->ViewPortInfo), fpCameraData->InitPitchDeg,
                                                             fpCameraData->InitYawDeg, fpCameraData->CameraPosition);
         result = fpCamera;
      }
      else
      {
         auto thpCameraData = std::static_pointer_cast<SerializeDataThirdPersonCamera>(data);
         assert(thpCameraData);

         std::shared_ptr<ThirdPersonCamera> thirdPersonCamera;

         if (cameraTypeName == "ThirdPersonCamera")
         {
            thirdPersonCamera = std::make_shared<ThirdPersonCamera>(thpCameraData->CameraName, scene, ViewPortInfo(thpCameraData->ViewPortInfo),
                                                                    thpCameraData->InitPitchDeg, thpCameraData->InitYawDeg, thpCameraData->CameraDistanceToThirdPersonTarget, thpCameraData->ThirdPersonTargetOffset);
         }
         else if (cameraTypeName == "MainThirdPersonCamera")
         {
            outIsMainSceneCamera = true;
            thirdPersonCamera = std::make_shared<MainThirdPersonCamera>(thpCameraData->CameraName, scene, ViewPortInfo(thpCameraData->ViewPortInfo),
                                                                        thpCameraData->InitPitchDeg, thpCameraData->InitYawDeg, thpCameraData->CameraDistanceToThirdPersonTarget, thpCameraData->ThirdPersonTargetOffset);
         }

         thirdPersonCamera->SetThirdPersonTargetDeferred(thpCameraData->ThirdPersonTargetActorName);

         result = thirdPersonCamera;
      }

      return result;
   }

   std::shared_ptr<Component> SerializeHelper::CreateComponentFromSerializedData(std::shared_ptr<Scene> scene, std::shared_ptr<SerializeDataBase> data)
   {
      std::shared_ptr<Component> result;

      const auto dataType = data->GetSerializeDataType();

      std::string logCompType = "";

      const auto &folderManagerInstance = IO::FolderManager::GetInstance();
      switch (dataType)
      {
      case SerializeDataBase::SerializeDataType::StaticMesh:
      {
         logCompType = "StaticMesh";

         SerializeDataStaticMesh *meshData = static_cast<SerializeDataStaticMesh *>(data.get());

         IMaterial *material = CreateMaterialFromSerializedData(meshData->MeshMaterial);
         const auto &meshCompData = EngineObjectCreator::CreateMeshComponentData(meshData->ComponentName,
                                                                                 meshData->ModelName, meshData->Translation, meshData->Rotation, meshData->Scale, meshData->LuaScriptName, material);
         result = EngineObjectCreator::CreateComponentByString("StaticMeshComponent", meshCompData, scene);
         break;
      }
      case SerializeDataBase::SerializeDataType::SkeletalMesh:
      {
         logCompType = "SkeletalMesh";

         SerializeDataSkeletalMesh *meshData = static_cast<SerializeDataSkeletalMesh *>(data.get());

         IMaterial *material = CreateMaterialFromSerializedData(meshData->MeshMaterial);
         const auto &meshCompData = EngineObjectCreator::CreateMeshComponentData(meshData->ComponentName,
                                                                                 meshData->ModelName, meshData->Translation, meshData->Rotation, meshData->Scale, meshData->LuaScriptName, material);
         result = EngineObjectCreator::CreateComponentByString("SkeletalMeshComponent", meshCompData, scene);
         break;
      }
      case SerializeDataBase::SerializeDataType::Skybox:
      {
         logCompType = "Skybox";
         SerializeDataSkyboxComponent *skyboxData = static_cast<SerializeDataSkyboxComponent *>(data.get());

         IMaterial *material = CreateMaterialFromSerializedData(skyboxData->Material);

         const auto &skyboxCompData = EngineObjectCreator::CreateSkyboxComponentData(skyboxData->ComponentName, skyboxData->Scale, material);
         result = EngineObjectCreator::CreateComponentByString("SkyboxComponent", skyboxCompData, scene);
         break;
      }
      case SerializeDataBase::SerializeDataType::DirectionalLight:
      {
         logCompType = "DirectionalLight";
         SerializeDataDirLightComponent *dirLightSerData = static_cast<SerializeDataDirLightComponent *>(data.get());

         ProjectedShadowInfo *dirShadowProjInfo = nullptr;
         if (dirLightSerData->bHasShadowMap)
         {
            const float orthoHalfExtent = GlobalSettings::GetInstance()->GetShadowOrthoProjectionHalfExtent();

            const auto &rezolution = glm::ivec2(dirLightSerData->ShadowMapSize, dirLightSerData->ShadowMapSize);
            const auto &directionalLightTextureAtlasRequest = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(rezolution);
            dirShadowProjInfo = new ProjectedDirectionalLightShadowInfo(directionalLightTextureAtlasRequest, orthoHalfExtent);
         }

         const auto &dirLightCompData = EngineObjectCreator::CreateDirLightComponentData(dirLightSerData->ComponentName,
                                                                                         dirLightSerData->Rotation, dirLightSerData->Direction,
                                                                                         dirLightSerData->AmbientLight,
                                                                                         dirLightSerData->DiffuseLight,
                                                                                         dirLightSerData->SpecularLight, dirShadowProjInfo);

         result = EngineObjectCreator::CreateComponentByString("DirectionalLightComponent", dirLightCompData, scene);
         break;
      }
      case SerializeDataBase::SerializeDataType::PointLight:
      {
         logCompType = "PointLight";
         SerializeDataPointLightComponent *pointLightSerData = static_cast<SerializeDataPointLightComponent *>(data.get());

         ProjectedShadowInfo *pointLightShadowProjInfo = nullptr;
         if (pointLightSerData->bHasShadowMap)
         {
            const auto &rezolution = glm::ivec2(pointLightSerData->ShadowMapSize, pointLightSerData->ShadowMapSize);
            const auto &pointLightTextureAtlasRequest = TextureAtlasFactory::GetInstance()->AddTextureCubeAtlasRequest(rezolution);
            pointLightShadowProjInfo = new ProjectedPointLightShadowInfo(pointLightTextureAtlasRequest);
         }

         const auto &pointLightCompData = EngineObjectCreator::CreatePointLightComponentData(pointLightSerData->ComponentName,
                                                                                             pointLightSerData->Translation,
                                                                                             pointLightSerData->AmbientLight,
                                                                                             pointLightSerData->DiffuseLight,
                                                                                             pointLightSerData->SpecularLight,
                                                                                             pointLightSerData->Attenuation,
                                                                                             pointLightSerData->RadianceRadius,
                                                                                             pointLightShadowProjInfo);

         result = EngineObjectCreator::CreateComponentByString("PointLightComponent", pointLightCompData, scene);
         break;
      }
      case SerializeDataBase::SerializeDataType::Spotlight:
      {
         logCompType = "Spotlight";
         SerializeDataSpotlightComponent *spotlightSerData = static_cast<SerializeDataSpotlightComponent *>(data.get());

         ProjectedShadowInfo *spotlightShadowProjInfo = nullptr;
         if (spotlightSerData->bHasShadowMap)
         {
            const auto &rezolution = glm::ivec2(spotlightSerData->ShadowMapSize, spotlightSerData->ShadowMapSize);
            const auto &spotlightTextureAtlasRequest = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(rezolution);
            spotlightShadowProjInfo = new ProjectedPointLightShadowInfo(spotlightTextureAtlasRequest);
         }

         const auto &spotlightCompData = EngineObjectCreator::CreateSpotlightComponentData(
             spotlightSerData->ComponentName,
             spotlightSerData->Translation,
             spotlightSerData->Rotation,
             spotlightSerData->AmbientLight,
             spotlightSerData->DiffuseLight,
             spotlightSerData->SpecularLight,
             spotlightSerData->Attenuation,
             spotlightSerData->RadianceRadius,
             spotlightSerData->Cutoff,
             spotlightShadowProjInfo);

         result = EngineObjectCreator::CreateComponentByString("SpotlightComponent", spotlightCompData, scene);
         break;
      }
      case SerializeDataBase::SerializeDataType::Input:
      {
         logCompType = "Input";
         SerializeDataInputComponent *inputSerData = static_cast<SerializeDataInputComponent *>(data.get());
         const auto &inputCompData = EngineObjectCreator::CreateInputComponentData(inputSerData->ComponentName);
         result = EngineObjectCreator::CreateComponentByString("InputComponent", inputCompData, scene);
         break;
      }
      case SerializeDataBase::SerializeDataType::CharacterMovement:
      {
         logCompType = "CharacterMovement";
         SerializeDataCharacterMovementComponent *charMovSerData = static_cast<SerializeDataCharacterMovementComponent *>(data.get());
         const auto &charMoveCompData = EngineObjectCreator::CreateCharacterMovementComponentData(charMovSerData->ComponentName, charMovSerData->LaunchDirection, charMovSerData->CameraName);
         result = EngineObjectCreator::CreateComponentByString("CharacterMovementComponent", charMoveCompData, scene);
         break;
      }
      case SerializeDataBase::SerializeDataType::PlatformMovement:
      {
         logCompType = "PlatformMovement";
         SerializeDataPlatformMovementComponent *movSerData = static_cast<SerializeDataPlatformMovementComponent *>(data.get());
         const auto &moveCompData = EngineObjectCreator::CreatePlatformMovementComponentData(movSerData->ComponentName, movSerData->ScriptName);
         result = EngineObjectCreator::CreateComponentByString("PlatformMovementComponent", moveCompData, scene);
         break;
      }
      case SerializeDataBase::SerializeDataType::Physics:
      {
         logCompType = "Physics";
         SerializeDataPhysicsComponent *serData = static_cast<SerializeDataPhysicsComponent *>(data.get());
         auto physShape = CreatePhysicsShape(serData->PhysicsShape.get());
         auto compController = EngineObjectCreator::CreateRigidBodyController(scene->GetPhysicsWorld(), physShape, serData->BodyType, serData->Mass);
         const auto &compData = EngineObjectCreator::CreatePhysicsComponentData(serData->ComponentName, compController);
         result = EngineObjectCreator::CreateComponentByString("PhysicsComponent", compData, scene);
         break;
      }
      case SerializeDataBase::SerializeDataType::CharacterPhysics:
      {
         logCompType = "CharacterPhysics";
         SerializeDataCharacterPhysicsComponent *serData = static_cast<SerializeDataCharacterPhysicsComponent *>(data.get());
         auto compController = EngineObjectCreator::CreateDynamicCharacterController(scene->GetPhysicsWorld(), serData->CapsuleRadius, serData->CapsuleHeight, serData->Mass, serData->StepHeight);
         const auto &compData = EngineObjectCreator::CreatePhysicsComponentData(serData->ComponentName, compController);
         result = EngineObjectCreator::CreateComponentByString("CharacterPhysicsComponent", compData, scene);
         break;
      }
      case SerializeDataBase::SerializeDataType::PlanarReflection:
      {
         logCompType = "PlanarReflection";
         SerializeDataPlanarReflectionComponent *serData = static_cast<SerializeDataPlanarReflectionComponent *>(data.get());
         auto camera = scene->GetCamera(serData->OwnerCameraName);
         const auto viewPortInfoVec4 = serData->ViewPortInfo;
         const auto &compData = EngineObjectCreator::CreatePlanarReflectionComponentData(serData->ComponentName, serData->Translation, serData->EulerAnglesRotation, serData->Scale,
                                                                                         camera.get(), ViewPortInfo(viewPortInfoVec4.x, viewPortInfoVec4.y, viewPortInfoVec4.z, viewPortInfoVec4.w));
         result = EngineObjectCreator::CreateComponentByString("PlanarReflectionComponent", compData, scene);
         break;
      }

      default:
      {
         assert(false);
         break;
      }
      }

      TinyLogger::LogProxy::LogMessages("Deserialize component, type:", std::string(logCompType));

      return result;
   }

   PhysicsShapeBase *SerializeHelper::CreatePhysicsShape(SerializeDataPhysicsShape *serDataShape)
   {
      PhysicsShapeBase *result = nullptr;

      switch (serDataShape->GetShapeProxyType())
      {
      case BOX_SHAPE_PROXYTYPE:
      {
         auto shapeData = static_cast<SerializeDataBoxPhysicsShape *>(serDataShape);
         result = new PhyBoxShape(shapeData->HalfExtent);
         break;
      }
      case CAPSULE_SHAPE_PROXYTYPE:
      {
         auto shapeData = static_cast<SerializeDataCapsulePhysicsShape *>(serDataShape);
         result = new PhyCapsuleShape(shapeData->Radius, shapeData->Height);
         break;
      }
      case SPHERE_SHAPE_PROXYTYPE:
      {
         auto shapeData = static_cast<SerializeDataSpherePhysicsShape *>(serDataShape);
         result = new PhySphereShape(shapeData->Radius);
         break;
      }
      case COMPOUND_SHAPE_PROXYTYPE:
      {
         auto shapeData = static_cast<SerializeDataCompoundPhysicsShape *>(serDataShape);
         const auto &childrenData = shapeData->ChildrenWithRotation;

         auto compoundPhyShape = new PhyCompoundShape();

         for (const auto &childData : childrenData)
         {
            const auto childPhysicsShape = CreatePhysicsShape(childData.Child.get());
            compoundPhyShape->AddChildShape(NoScaleEulerRotationTransform(childData.ChildTransform.Translation, childData.ChildTransform.Rotation),
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

   IMaterial *SerializeHelper::CreateMaterialFromSerializedData(const SerializeDataMaterial &materialData)
   {
      IMaterial *material = new IMaterial(materialData.MaterialName, materialData.MaterialShaderName);

      for (const auto &property : materialData.Properties)
      {
         if (property.PropertyType == "texture" && property.Value != "")
         {
            material->PushMaterialProperty(std::make_shared<TextureMaterialProperty>(property.UniformName));
            const auto &texture = TexturePool::GetInstance()->GetOrAllocateResource(property.Value);

            MaterialPropertySetter::SetMaterialPropertyValue(material, property.UniformName, texture);
         }
         else if (property.PropertyType == "float")
         {
            material->PushMaterialProperty(std::make_shared<FloatMaterialProperty>(property.UniformName));
            MaterialPropertySetter::SetMaterialPropertyValue(material, property.UniformName, std::stof(property.Value));
         }
      }

      return material;
   }

   std::vector<std::string> SerializeHelper::GetSerializedAllocatedResources(const SerializeAllocatedResources &allocatedResources)
   {
      std::vector<std::string> resourceNames;
      resourceNames.reserve(allocatedResources.ResourceNames.size());
      for (const auto &name : allocatedResources.ResourceNames)
      {
         if (std::string::npos != EngineUtility::IndexOf(name, ","))
         {
            const std::vector<std::string> &splitedNames = EngineUtility::Split(name, ',');
            for (std::string splitName : splitedNames)
            {
               splitName = EngineUtility::TrimEnd(splitName);
               resourceNames.emplace_back(std::move(splitName));
            }
         }
         else
         {
            resourceNames.emplace_back(name);
         }
      }
      return resourceNames;
   }
}
