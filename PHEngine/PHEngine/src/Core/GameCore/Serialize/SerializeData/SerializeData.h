#pragma once
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <memory>

#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>

#include "Core/GraphicsCore/Material/IMaterial.h"

namespace glm 
{
   template<class Archive> void serialize(Archive& archive, glm::vec3& v) { archive(v.x, v.y, v.z); }
}

struct SerializeDataBase
{
   enum class SerializeDataType
   {
      Actor,
      StaticMesh,
      SkeletalMesh,
      Skybox,
      Material,
      DirectionalLight,
      PointLight
   };

   virtual SerializeDataType GetSerializeDataType() const = 0;
};

struct SerializeDataComponent
   : SerializeDataBase
{
   std::string ComponentName;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      archive(ComponentName);
   }
};

struct SerializeDataMaterial
   : public SerializeDataBase
{
   struct SerializeDataMaterialProperty
   {
      std::string PropertyType;
      std::string UniformName;
      std::string Value;

      template <typename Archive>
      void serialize(Archive& archive)
      {
         archive(PropertyType, UniformName, Value);
      }
   };

   std::string MaterialName;
   std::string MaterialShaderRelPath;

   std::vector<SerializeDataMaterialProperty> Properties;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      archive(MaterialName, MaterialShaderRelPath, Properties);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::Material;
   }
};

struct SerializeDataMesh
   : public SerializeDataComponent
{
   std::string ModelName;
   glm::vec3 Translation;

   glm::vec3 Rotation;
   glm::vec3 Scale;
   std::string LuaScriptName;
   SerializeDataMaterial MeshMaterial;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      SerializeDataComponent::serialize(archive);

      archive(ModelName, Translation, Rotation, Scale, LuaScriptName, MeshMaterial);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::StaticMesh;
   }
};

struct SerializeDataDirLightComponent
   : public SerializeDataComponent
{
   glm::vec3 Direction;
   glm::vec3 Rotation;

   glm::vec3 AmbientLight;
   glm::vec3 DiffuseLight;
   glm::vec3 SpecularLight;

   bool bHasShadowMap;
   float ShadowMapSize;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      SerializeDataComponent::serialize(archive);

      archive(Direction, Rotation, AmbientLight, DiffuseLight, SpecularLight, bHasShadowMap, ShadowMapSize);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::StaticMesh;
   }
};

struct SerializeDataPointLightComponent
   : public SerializeDataComponent
{
   glm::vec3 Translation;
   glm::vec3 Rotation;

   glm::vec3 Attenuation;
   float RadianceSqrRadius;

   glm::vec3 AmbientLight;
   glm::vec3 DiffuseLight;
   glm::vec3 SpecularLight;

   bool bHasShadowMap;
   float ShadowMapSize;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      SerializeDataComponent::serialize(archive);

      archive(Translation, Rotation, Attenuation, RadianceSqrRadius, AmbientLight, DiffuseLight, SpecularLight, bHasShadowMap, ShadowMapSize);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::PointLight;
   }
};

struct SerializeDataActor
   : public SerializeDataBase
{
   std::string ActorName;
   glm::vec3 RootCompTranslation;
   glm::vec3 RootCompRotation;
   glm::vec3 RootCompScale;

   std::vector<std::shared_ptr<SerializeDataBase>> ComponentsData;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      archive(ActorName, RootCompTranslation, RootCompRotation, RootCompScale, ComponentsData);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::DirectionalLight;
   }
};

CEREAL_REGISTER_TYPE(SerializeDataActor);
CEREAL_REGISTER_TYPE(SerializeDataMesh);
CEREAL_REGISTER_TYPE(SerializeDataDirLightComponent);
CEREAL_REGISTER_TYPE(SerializeDataPointLightComponent);
CEREAL_REGISTER_TYPE(SerializeDataMaterial);

CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataActor)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataMesh)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataDirLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataPointLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataMaterial)
