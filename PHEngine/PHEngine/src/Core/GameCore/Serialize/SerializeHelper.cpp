#include "SerializeHelper.h"

#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhySphereShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyBoxShape.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/StateMachine/StateMachine.h"
#include "Core/GameCore/StateMachine/FSMParser.h"
#include "Core/GameCore/ScriptingCore/LuaToCPPAdapter.h"
#include "Core/GameCore/Scene.h"

using namespace Graphics;
using namespace EnginePhysics;

namespace Game {

   std::shared_ptr<SerializeDataPhysicsComponent> SerializeHelper::GetSerializeDataPhysicsComponent(PhysicsComponent* component)
   {
      std::shared_ptr<SerializeDataPhysicsComponent> physCompData = std::make_shared<SerializeDataPhysicsComponent>();

      std::shared_ptr<SerializeDataPhysicsShape> PhysicsShape;

      PhyShapeBase* physShape = component->GetDescriptor()->GetShape();
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
         std::make_shared<SceneComponent>(data.ActorName + "_RootComp", data.RootCompTranslation, data.RootCompRotation, data.RootCompScale));
      return actor;
   }

   std::shared_ptr<StateMachine> SerializeHelper::CreateFsmFromSerializedData(std::shared_ptr<SerializeDataStateMachine> data) {
      FSMParser fsmParser;
      return fsmParser.ParseFSMDescriptor(data->FsmRelPath);
   }

   std::shared_ptr<SerializeDataMesh> SerializeHelper::GetSerializedDataStaticMesh(const StaticMeshComponent* component)
   {
      auto meshData = std::make_shared<SerializeDataMesh>();
      meshData->ComponentName = component->GameObjectName;
      meshData->ModelName = MeshPool::GetInstance()->GetKey(component->GetRenderData().m_skin);
      meshData->Translation = component->GetTranslation();
      meshData->Rotation = component->GetRotationEuler();
      meshData->Scale = component->GetScale();
      meshData->LuaScriptName = ""; // TODO: for now

      const SerializeDataMaterial& material = SerializeHelper::GetSerializeDataMaterial(component->GetRenderData().mMaterialInstance);

      meshData->MeshMaterial = material;

      return meshData;
   }

   std::shared_ptr<Component> SerializeHelper::CreateComponentFromSerializedData(Scene* scene, std::shared_ptr<SerializeDataBase> data) {

      std::shared_ptr<Component> result;

      const auto dataType = data->GetSerializeDataType();

      if (dataType == SerializeDataBase::SerializeDataType::StaticMesh || dataType == SerializeDataBase::SerializeDataType::SkeletalMesh)
      {
         SerializeDataMesh* meshData = static_cast<SerializeDataMesh*>(data.get());

         IMaterial* material = CreateMaterialFromSerializedData(meshData->MeshMaterial);

         auto meshCompData = LuaToCPPAdapter::CreateMeshComponentData(meshData->ComponentName,
            meshData->ModelName, meshData->Translation, meshData->Rotation, meshData->Scale, meshData->LuaScriptName, material);
         result = LuaToCPPAdapter::CreateComponentByString("StaticMeshComponent", meshCompData, scene);
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
