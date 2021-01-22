#pragma once
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <memory>

#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>

#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"

using namespace EnginePhysics;

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
      PointLight,
      Spotlight,
      CharacterMovement,
      Movement,
      Input,
      Physics,
      CharacterPhysics,
      FSM,
      PlayerController
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
   std::string MaterialShaderName;

   std::vector<SerializeDataMaterialProperty> Properties;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      archive(MaterialName, MaterialShaderName, Properties);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::Material;
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


struct SerializeDataSkeletalMesh
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
      return SerializeDataBase::SerializeDataType::SkeletalMesh;
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
      return SerializeDataBase::SerializeDataType::DirectionalLight;
   }
};

struct SerializeDataPointLightComponent
   : public SerializeDataComponent
{
   glm::vec3 Translation;

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

      archive(Translation, Attenuation, RadianceSqrRadius, AmbientLight, DiffuseLight, SpecularLight, bHasShadowMap, ShadowMapSize);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::PointLight;
   }
};

struct SerializeDataSpotlightComponent
   : public SerializeDataPointLightComponent
{
   glm::vec3 Rotation;

   float Cutoff;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      SerializeDataPointLightComponent::serialize(archive);

      archive(Rotation, Cutoff);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::Spotlight;
   }
};

struct SerializeDataCharacterMovementComponent
   : public SerializeDataComponent
{
   glm::vec3 LaunchDirection;
   std::string CameraName;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      SerializeDataComponent::serialize(archive);

      archive(LaunchDirection, CameraName);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::CharacterMovement;
   }
};

struct SerializeDataMovementComponent
   : public SerializeDataComponent
{
   std::string ScriptName;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      SerializeDataComponent::serialize(archive);

      archive(ScriptName);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::Movement;
   }
};

struct SerializeDataInputComponent
   : public SerializeDataComponent
{
   template <typename Archive>
   void serialize(Archive& archive)
   {
      SerializeDataComponent::serialize(archive);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::Input;
   }
};

struct SerializeDataPhysicsShape
{
   virtual int32_t GetShapeProxyType() = 0;
};

struct SerializeDataBoxPhysicsShape
   : public SerializeDataPhysicsShape
{
   glm::vec3 HalfExtent;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      archive(HalfExtent);
   }

   virtual int32_t GetShapeProxyType() override
   {
      return BOX_SHAPE_PROXYTYPE;
   }
};

struct SerializeDataCapsulePhysicsShape
   : public SerializeDataPhysicsShape
{
   float Radius;
   float Height;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      archive(Radius, Height);
   }

   virtual int32_t GetShapeProxyType() override
   {
      return CAPSULE_SHAPE_PROXYTYPE;
   }
};

struct SerializeDataSpherePhysicsShape
   : public SerializeDataPhysicsShape
{
   float Radius;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      archive(Radius);
   }

   virtual int32_t GetShapeProxyType() override
   {
      return SPHERE_SHAPE_PROXYTYPE;
   }
};

struct SerializeDataPhysicsComponent
   : public SerializeDataComponent
{
   std::shared_ptr<SerializeDataPhysicsShape> PhysicsShape;

   PhysicsBodyType BodyType;
   /*Motion modifiers*/
   glm::vec3 LinearFactor;
   glm::vec3 AngularFactor;
   /*Motion modifiers*/
   float Mass;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      SerializeDataComponent::serialize(archive);

      archive(PhysicsShape, BodyType, LinearFactor, AngularFactor, Mass);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::Physics;
   }
};

struct SerializeDataStateMachine
   : public SerializeDataBase
{
   struct SerializeFSMBinding
   {
      std::string GameObjectName;
      std::string BindingName;
      std::string GameObjectPropertyName;

      template <typename Archive>
      void serialize(Archive& archive)
      {
         archive(GameObjectName, BindingName, GameObjectPropertyName);
      }
   };

   std::string FsmRelPath;

   std::vector<SerializeFSMBinding> Bindings;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      archive(FsmRelPath, Bindings);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::FSM;
   }
};

struct SerializeDataActor
   : public SerializeDataBase
{
   std::string ActorName;
   glm::vec3 RootCompTranslation;
   glm::vec3 RootCompRotation;
   glm::vec3 RootCompScale;
   std::shared_ptr<SerializeDataStateMachine> StateMachineData;
   std::vector<std::shared_ptr<SerializeDataBase>> ComponentsData;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      archive(ActorName, RootCompTranslation, RootCompRotation, RootCompScale, StateMachineData, ComponentsData);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::Actor;
   }
};

struct SerializeDataCharacterPhysicsComponent
   : public SerializeDataComponent
{
   float CapsuleRadius;
   float CapsuleHeight;
   float Mass;
   float StepHeight;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      SerializeDataComponent::serialize(archive);

      archive(CapsuleRadius, CapsuleHeight, Mass, StepHeight);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::CharacterPhysics;
   }
};

struct SerializeDataSkyboxComponent
   : public SerializeDataComponent
{
   glm::vec3 Scale;

   SerializeDataMaterial Material;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      SerializeDataComponent::serialize(archive);

      archive(Scale, Material);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::Skybox;
   }
};

struct SerializeDataPlayerController
   : public SerializeDataBase
{
   std::string BindedActorName;

   template <typename Archive>
   void serialize(Archive& archive)
   {
      archive(BindedActorName);
   }

   virtual SerializeDataType GetSerializeDataType() const override
   {
      return SerializeDataBase::SerializeDataType::PlayerController;
   }

   SerializeDataPlayerController(const std::string& actorName)
      : BindedActorName(actorName)
   {
   }

   SerializeDataPlayerController() = default;
};

CEREAL_REGISTER_TYPE(SerializeDataActor);
CEREAL_REGISTER_TYPE(SerializeDataStaticMesh);
CEREAL_REGISTER_TYPE(SerializeDataSkeletalMesh);
CEREAL_REGISTER_TYPE(SerializeDataDirLightComponent);
CEREAL_REGISTER_TYPE(SerializeDataPointLightComponent);
CEREAL_REGISTER_TYPE(SerializeDataSpotlightComponent);
CEREAL_REGISTER_TYPE(SerializeDataMaterial);
CEREAL_REGISTER_TYPE(SerializeDataMovementComponent);
CEREAL_REGISTER_TYPE(SerializeDataCharacterMovementComponent);
CEREAL_REGISTER_TYPE(SerializeDataPhysicsComponent);
CEREAL_REGISTER_TYPE(SerializeDataCharacterPhysicsComponent);
CEREAL_REGISTER_TYPE(SerializeDataInputComponent);
CEREAL_REGISTER_TYPE(SerializeDataSkyboxComponent);
CEREAL_REGISTER_TYPE(SerializeDataPlayerController);

CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataActor)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataStaticMesh)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataSkeletalMesh)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataDirLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataPointLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataSpotlightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataMaterial)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataCharacterMovementComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataMovementComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataPhysicsComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataCharacterPhysicsComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataInputComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataSkyboxComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataBase, SerializeDataPlayerController);

CEREAL_REGISTER_TYPE(SerializeDataCapsulePhysicsShape);
CEREAL_REGISTER_TYPE(SerializeDataSpherePhysicsShape);
CEREAL_REGISTER_TYPE(SerializeDataBoxPhysicsShape);

CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataPhysicsShape, SerializeDataCapsulePhysicsShape)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataPhysicsShape, SerializeDataSpherePhysicsShape)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeDataPhysicsShape, SerializeDataBoxPhysicsShape)