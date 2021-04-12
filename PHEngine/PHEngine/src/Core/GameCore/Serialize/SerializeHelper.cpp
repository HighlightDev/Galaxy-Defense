#include "SerializeHelper.h"

#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhySphereShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyBoxShape.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GameCore/ScriptingCore/LuaToCPPAdapter.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GlobalSettings.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirectionalLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedPointLightShadowInfo.h"
#include "Core/GraphicsCore/Material/TextureMaterialProperty.h"
#include "Core/GraphicsCore/Material/FloatMaterialProperty.h"

#include <TinyLogger/LogInterface.h>

using namespace Graphics;
using namespace EnginePhysics;

namespace Game {

   std::shared_ptr<SerializeDataPhysicsComponent> SerializeHelper::GetSerializeDataPhysicsComponent(PhysicsComponent* component)
   {
      std::shared_ptr<SerializeDataPhysicsComponent> physCompData = std::make_shared<SerializeDataPhysicsComponent>();

      std::shared_ptr<SerializeDataPhysicsShape> PhysicsShape;

      PhysicsShapeBase* physShape = component->GetDescriptor()->GetShape();
      int32_t shapeType = physShape->GetCollisionShape()->getShapeType();

      if (SPHERE_SHAPE_PROXYTYPE == shapeType)
      {
         PhySphereShape* sphere = static_cast<PhySphereShape*>(physShape);
         auto shape = std::make_shared<SerializeDataSpherePhysicsShape>();
         shape->Radius = sphere->GetRadius();
         PhysicsShape = shape;
      }
      else if (BOX_SHAPE_PROXYTYPE == shapeType)
      {
         PhyBoxShape* box = static_cast<PhyBoxShape*>(physShape);
         auto shape = std::make_shared<SerializeDataBoxPhysicsShape>();
         shape->HalfExtent = box->GetHalfExtent();
         PhysicsShape = shape;
      }
      else if (CAPSULE_SHAPE_PROXYTYPE == shapeType)
      {
         PhyCapsuleShape* capsule = static_cast<PhyCapsuleShape*>(physShape);
         auto shape = std::make_shared<SerializeDataCapsulePhysicsShape>();
         shape->Height = capsule->GetHeight();
         shape->Radius = capsule->GetRadius();
         PhysicsShape = shape;
      }

      physCompData->PhysicsShape = PhysicsShape;
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

      for (const auto& propertyItem : properties)
      {
         const std::string& uniformName = propertyItem.first;
         std::shared_ptr<MaterialProperty> prop = propertyItem.second;
         auto type = prop->GetMaterialPropertyType();

         std::string uniformValue, propertyType;

         if (type == MaterialProperty::MaterialPropertyType::TEXTURE_PROPERTY)
         {
            std::shared_ptr<TextureMaterialProperty> texProp = std::static_pointer_cast<TextureMaterialProperty>(prop);
            propertyType = "texture";
            uniformValue = Resources::TexturePool::GetInstance()->GetKey(texProp->GetValue());
         }
         else if (type == MaterialProperty::MaterialPropertyType::FLOAT_PROPERTY)
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

   std::shared_ptr<Actor> SerializeHelper::CreateActorFromSerializedData(const SerializeDataActor& data) {

      auto actor = std::make_shared<Actor>(data.ActorName,
         std::make_shared<SceneComponent>(data.ActorName + "_RootComponent", data.RootCompTranslation, data.RootCompRotation, data.RootCompScale));
      return actor;
   }

   std::shared_ptr<Tweener> SerializeHelper::CreateTweenerFromSerializedData(std::shared_ptr<SerializeDataTweener> data) {
      TweenerParser fsmParser;
      return fsmParser.ParseTweenerDescriptor(data->TweenerRelPath);
   }

   std::shared_ptr<SerializeDataStaticMesh> SerializeHelper::GetSerializedDataStaticMesh(const StaticMeshComponent* component)
   {
      auto meshData = std::make_shared<SerializeDataStaticMesh>();
      meshData->ComponentName = component->GetGameObjectName();
      meshData->ModelName = MeshPool::GetInstance()->GetKey(component->GetRenderData().m_skin);
      meshData->Translation = component->GetTranslation();
      meshData->Rotation = component->GetRotationEuler();
      meshData->Scale = component->GetScale();
      meshData->LuaScriptName = ""; // TODO: for now

      const SerializeDataMaterial& material = SerializeHelper::GetSerializeDataMaterial(component->GetMaterial());

      meshData->MeshMaterial = material;

      return meshData;
   }

   std::shared_ptr<SerializeDataSkeletalMesh> SerializeHelper::GetSerializedDataSkeletalMesh(const SkeletalMeshComponent* component)
   {
      auto meshData = std::make_shared<SerializeDataSkeletalMesh>();
      meshData->ComponentName = component->GetGameObjectName();
      meshData->ModelName = MeshPool::GetInstance()->GetKey(component->GetRenderData().m_skin);
      meshData->Translation = component->GetTranslation();
      meshData->Rotation = component->GetRotationEuler();
      meshData->Scale = component->GetScale();
      meshData->LuaScriptName = component->LuaScriptName;

      const SerializeDataMaterial& material = SerializeHelper::GetSerializeDataMaterial(component->GetMaterial());

      meshData->MeshMaterial = material;

      return meshData;
   }

   std::shared_ptr<Component> SerializeHelper::CreateComponentFromSerializedData(Scene* scene, std::shared_ptr<SerializeDataBase> data) 
   {
      std::shared_ptr<Component> result;

      const auto dataType = data->GetSerializeDataType();

      std::string logCompType = "";

      switch (dataType)
      {
         case SerializeDataBase::SerializeDataType::StaticMesh:
         {
            logCompType = "StaticMesh";

            SerializeDataStaticMesh* meshData = static_cast<SerializeDataStaticMesh*>(data.get());

            IMaterial* material = CreateMaterialFromSerializedData(meshData->MeshMaterial);

            auto meshCompData = LuaToCPPAdapter::CreateMeshComponentData(meshData->ComponentName,
               meshData->ModelName, meshData->Translation, meshData->Rotation, meshData->Scale, meshData->LuaScriptName, material);
            result = LuaToCPPAdapter::CreateComponentByString("StaticMeshComponent", meshCompData, scene);
            break;
         }
         case SerializeDataBase::SerializeDataType::SkeletalMesh:
         {
            logCompType = "SkeletalMesh";

            SerializeDataSkeletalMesh* meshData = static_cast<SerializeDataSkeletalMesh*>(data.get());

            IMaterial* material = CreateMaterialFromSerializedData(meshData->MeshMaterial);

            auto meshCompData = LuaToCPPAdapter::CreateMeshComponentData(meshData->ComponentName,
               meshData->ModelName, meshData->Translation, meshData->Rotation, meshData->Scale, meshData->LuaScriptName, material);
            result = LuaToCPPAdapter::CreateComponentByString("SkeletalMeshComponent", meshCompData, scene);
            break;
         }
         case SerializeDataBase::SerializeDataType::Skybox: {
            logCompType = "Skybox";
            SerializeDataSkyboxComponent* skyboxData = static_cast<SerializeDataSkyboxComponent*>(data.get());

            IMaterial* material = CreateMaterialFromSerializedData(skyboxData->Material);

            auto skyboxCompData = LuaToCPPAdapter::CreateSkyboxComponentData(skyboxData->ComponentName, skyboxData->Scale, material);
            result = LuaToCPPAdapter::CreateComponentByString("SkyboxComponent", skyboxCompData, scene);
            break;
         }
         case SerializeDataBase::SerializeDataType::DirectionalLight:
         {
            logCompType = "DirectionalLight";
            SerializeDataDirLightComponent* dirLightSerData = static_cast<SerializeDataDirLightComponent*>(data.get());

            ProjectedShadowInfo* dirShadowProjInfo = nullptr;
            if (dirLightSerData->bHasShadowMap)
            {
               const float orthoHalfExtent = GlobalSettings::GetInstance()->GetShadowOrthoProjectionHalfExtent();

               auto rezolution = glm::ivec2(dirLightSerData->ShadowMapSize, dirLightSerData->ShadowMapSize);
               auto directionalLightTextureAtlasRequest = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(rezolution);
               dirShadowProjInfo = new ProjectedDirectionalLightShadowInfo(directionalLightTextureAtlasRequest, orthoHalfExtent);
            }

               auto dirLightCompData = LuaToCPPAdapter::CreateDirLightComponentData(dirLightSerData->ComponentName,
                  dirLightSerData->Rotation, dirLightSerData->Direction,
                  dirLightSerData->AmbientLight,
                  dirLightSerData->DiffuseLight,
                  dirLightSerData->SpecularLight, dirShadowProjInfo);
            

               result = LuaToCPPAdapter::CreateComponentByString("DirectionalLightComponent", dirLightCompData, scene);
            break;
         }
         case SerializeDataBase::SerializeDataType::PointLight:
         {
            logCompType = "PointLight";
            SerializeDataPointLightComponent* pointLightSerData = static_cast<SerializeDataPointLightComponent*>(data.get());

            ProjectedShadowInfo* pointLightShadowProjInfo = nullptr;
            if (pointLightSerData->bHasShadowMap)
            {
               auto rezolution = glm::ivec2(pointLightSerData->ShadowMapSize, pointLightSerData->ShadowMapSize);
               auto pointLightTextureAtlasRequest = TextureAtlasFactory::GetInstance()->AddTextureCubeAtlasRequest(rezolution);
               pointLightShadowProjInfo = new ProjectedPointLightShadowInfo(pointLightTextureAtlasRequest);
            }

            auto pointLightCompData = LuaToCPPAdapter::CreatePointLightComponentData(pointLightSerData->ComponentName,
               pointLightSerData->Translation,
               pointLightSerData->AmbientLight,
               pointLightSerData->DiffuseLight,
               pointLightSerData->SpecularLight,
               pointLightSerData->Attenuation,
               pointLightSerData->RadianceRadius,
               pointLightShadowProjInfo);


            result = LuaToCPPAdapter::CreateComponentByString("PointLightComponent", pointLightCompData, scene);
            break;
         }
         case SerializeDataBase::SerializeDataType::Spotlight:
         {
            logCompType = "Spotlight";
            SerializeDataSpotlightComponent* spotlightSerData = static_cast<SerializeDataSpotlightComponent*>(data.get());

            ProjectedShadowInfo* spotlightShadowProjInfo = nullptr;
            if (spotlightSerData->bHasShadowMap)
            {
               auto rezolution = glm::ivec2(spotlightSerData->ShadowMapSize, spotlightSerData->ShadowMapSize);
               auto spotlightTextureAtlasRequest = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(rezolution);
               spotlightShadowProjInfo = new ProjectedPointLightShadowInfo(spotlightTextureAtlasRequest);
            }

            auto spotlightCompData = LuaToCPPAdapter::CreateSpotlightComponentData(spotlightSerData->ComponentName,
               spotlightSerData->Translation,
               spotlightSerData->Rotation,
               spotlightSerData->AmbientLight,
               spotlightSerData->DiffuseLight,
               spotlightSerData->SpecularLight,
               spotlightSerData->Attenuation,
               spotlightSerData->RadianceRadius,
               spotlightSerData->Cutoff,
               spotlightShadowProjInfo);


            result = LuaToCPPAdapter::CreateComponentByString("SpotlightComponent", spotlightCompData, scene);
            break;
         }
         case SerializeDataBase::SerializeDataType::Input:
         {
            logCompType = "Input";
            SerializeDataInputComponent* inputSerData = static_cast<SerializeDataInputComponent*>(data.get());
            auto inputCompData = LuaToCPPAdapter::CreateInputComponentData(inputSerData->ComponentName);
            result = LuaToCPPAdapter::CreateComponentByString("InputComponent", inputCompData, scene);
            break;
         }
         case SerializeDataBase::SerializeDataType::CharacterMovement:
         {
            logCompType = "CharacterMovement";
            SerializeDataCharacterMovementComponent* charMovSerData = static_cast<SerializeDataCharacterMovementComponent*>(data.get());
            auto charMoveCompData = LuaToCPPAdapter::CreateCharacterMovementComponentData(charMovSerData->ComponentName, charMovSerData->LaunchDirection, charMovSerData->CameraName);
            result = LuaToCPPAdapter::CreateComponentByString("CharacterMovementComponent", charMoveCompData, scene);
            break;
         }
         case SerializeDataBase::SerializeDataType::Movement:
         {
            logCompType = "Movement";
            SerializeDataMovementComponent* movSerData = static_cast<SerializeDataMovementComponent*>(data.get());
            auto moveCompData = LuaToCPPAdapter::CreateMovementComponentData(movSerData->ComponentName, movSerData->ScriptName);
            result = LuaToCPPAdapter::CreateComponentByString("MovementComponent", moveCompData, scene);
            break;
         }
         case SerializeDataBase::SerializeDataType::Physics:
         {
            logCompType = "Physics";
            SerializeDataPhysicsComponent* serData = static_cast<SerializeDataPhysicsComponent*>(data.get());
            auto physShape = CreatePhysicsShape(serData);
            auto compController = LuaToCPPAdapter::CreateRigidBodyController(scene->mPhysicsWorld, physShape, serData->BodyType, serData->Mass);
            auto compData = LuaToCPPAdapter::CreatePhysicsComponentData(serData->ComponentName, compController);
            result = LuaToCPPAdapter::CreateComponentByString("PhysicsComponent", compData, scene);
            break;
         }
         case SerializeDataBase::SerializeDataType::CharacterPhysics:
         {
            logCompType = "CharacterPhysics";
            SerializeDataCharacterPhysicsComponent* serData = static_cast<SerializeDataCharacterPhysicsComponent*>(data.get());
            auto compController = LuaToCPPAdapter::CreateDynamicCharacterController(scene->mPhysicsWorld, serData->CapsuleRadius, serData->CapsuleHeight, serData->Mass, serData->StepHeight);
            auto compData = LuaToCPPAdapter::CreatePhysicsComponentData(serData->ComponentName, compController);
            result = LuaToCPPAdapter::CreateComponentByString("CharacterPhysicsComponent", compData, scene);
            break;
         }

         default:
            break;
      }

      TinyLogger::LogProxy::LogMessages(std::string("Deserialize component, type:"), std::string(logCompType));

      return result;
   }

   PhysicsShapeBase* SerializeHelper::CreatePhysicsShape(const SerializeDataPhysicsComponent* serData) {
      PhysicsShapeBase* result = nullptr;

      switch (serData->PhysicsShape->GetShapeProxyType())
      {
         case BOX_SHAPE_PROXYTYPE:
         {
            auto shape = static_cast<SerializeDataBoxPhysicsShape*>(serData->PhysicsShape.get());
            result = new PhyBoxShape(shape->HalfExtent);
            break;
         }
         case CAPSULE_SHAPE_PROXYTYPE:
         {
            auto shape = static_cast<SerializeDataCapsulePhysicsShape*>(serData->PhysicsShape.get());
            result = new PhyCapsuleShape(shape->Radius, shape->Height);
            break;
         }
         case SPHERE_SHAPE_PROXYTYPE:
         {
            auto shape = static_cast<SerializeDataSpherePhysicsShape*>(serData->PhysicsShape.get());
            result = new PhySphereShape(shape->Radius);
            break;
         }
         default:
            break;
      }

      return result;
   }

   IMaterial* SerializeHelper::CreateMaterialFromSerializedData(const SerializeDataMaterial& materialData) {
      IMaterial* material = new IMaterial(materialData.MaterialName, materialData.MaterialShaderName);

      for (const auto& property : materialData.Properties)
      {
         if (property.PropertyType == "texture" && property.Value != "")
         {
            material->PushMaterialProperty(property.UniformName, std::make_shared<TextureMaterialProperty>());
            const std::vector<std::string>& pathToTextures = Split(property.Value, ',');
            std::shared_ptr<ITexture> texture;

            std::string resultPathToAllTextures;
            for (size_t i = 0; i < pathToTextures.size(); ++i)
            {
               resultPathToAllTextures += pathToTextures[i];

               if (i + 1 < pathToTextures.size())
               {
                  resultPathToAllTextures += ",";
               }
            }
            texture = TexturePool::GetInstance()->GetOrAllocateResource(resultPathToAllTextures);

            MaterialPropertySetter::SetMaterialPropertyValue(material, property.UniformName, texture);
         }
         else if (property.PropertyType == "float")
         {
            material->PushMaterialProperty(property.UniformName, std::make_shared<FloatMaterialProperty>());
            MaterialPropertySetter::SetMaterialPropertyValue(material, property.UniformName, std::stof(property.Value));
         }
      }

      return material;
   }
}
