#pragma once
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <memory>

#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>

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

struct SerializeDataStaticMesh
   : public SerializeDataComponent
{
   std::string ModelName;
   glm::vec3 Translation;

   glm::vec3 Rotation;
   glm::vec3 Scale;
   std::string LuaScriptName;
   // todo: Material

   template <typename Archive>
   void serialize(Archive& archive)
   {
      SerializeDataComponent::serialize(archive);

      archive(ModelName, Translation, Rotation, Scale, LuaScriptName);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::StaticMesh;
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
      return SerializeDataBase::SerializeDataType::Actor;
   }
};

CEREAL_REGISTER_TYPE(SerializeDataStaticMesh);
CEREAL_REGISTER_TYPE(SerializeDataActor);

CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataActor)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataStaticMesh)
