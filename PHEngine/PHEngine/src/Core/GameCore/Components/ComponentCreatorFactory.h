#pragma once

#include <memory>

#include "Component.h"
#include "ComponentType.h"

#include "DirectionalLightComponent.h"
#include "PointLightComponent.h"

#include "PrimitiveComponents/SkyboxComponent.h"
#include "PrimitiveComponents/StaticMeshComponent.h"
#include "PrimitiveComponents/SkeletalMeshComponent.h"
#include "PrimitiveComponents/WaterPlaneComponent.h"
#include "PrimitiveComponents/BillboardComponent.h"
#include "PrimitiveComponents/CubemapComponent.h"

#include "ComponentData/ComponentData.h"
#include "ComponentData/SkyboxComponentData.h"
#include "ComponentData/MeshComponentData.h"
#include "ComponentData/DirectionalLightComponentData.h"
#include "ComponentData/PointLightComponentData.h"
#include "ComponentData/CubemapComponentData.h"
#include "ComponentData/WaterPlaneComponentData.h"
#include "ComponentData/MovementComponentData.h"
#include "ComponentData/BillboardComponentData.h"
#include "ComponentData/PhysicsComponentData.h"

#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"

#include "Core/GraphicsCore/OpenGL/Shader/CompositeShaderParams.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"

#include "Core/GameCore/ShaderImplementation/SkeletalMeshShader.h"
#include "Core/GameCore/ShaderImplementation/CubemapShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"

#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkeletalMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkyboxVertexFactory.h"

using namespace Resources;
using namespace IO;

namespace Game
{
   enum class ComponentMetaType
   {
      DirectionalLight,
      PointLight,
      Skybox,
      StaticMesh,
      WaterPlane,
      SkeletalMesh,
      Cubemap,
      Billboard,
      Input,
      CharacterMovement,
      Movement,
      Physics,
   };

   template <ComponentMetaType metaType, typename ComponentType>
   struct ComponentCreatorFactory
   {
   private:

      template <ComponentMetaType metaType, typename ConstructType> struct CreatorFromMetaType { };

      template <typename ConstructType> struct CreatorFromMetaType<ComponentMetaType::Skybox, ConstructType>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data)
         {

            const SkyboxComponentData& mData = static_cast<const SkyboxComponentData&>(data);

            int32_t primitive = (int32_t)SimplePrimitiveType::CUBE;
            auto skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);

            const ShaderParams shaderParams("Skybox ForwardShader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "forwardFS.glsl");

            TemplatedCompositeShaderParams<CompositeShader<SkyboxVertexFactory, SimpleShader>> compositeParams(COMPOSITE_SHADER_TO_STR(SkyboxVertexFactory, SimpleShader, mData.m_materialInstance->MaterialName), shaderParams, mData.m_materialInstance);
            CompositeShaderPool::sharedValue_t skyboxMeshShader = CompositeShaderPool::GetInstance()->template GetOrAllocateResource<CompositeShader<SkyboxVertexFactory, SimpleShader>>(compositeParams);

            return std::make_shared<ComponentType>(mData.GameObjectName, mData.m_scale, SkyboxRenderData(skin, skyboxMeshShader, mData.m_materialInstance));
         }
      };

      template <typename ConstructType> struct CreatorFromMetaType<ComponentMetaType::StaticMesh, ConstructType>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data)
         {
            const MeshComponentData& mData = static_cast<const MeshComponentData&>(data);

            typename MeshPool::sharedValue_t skin = MeshPool::GetInstance()->GetOrAllocateResource(mData.m_pathToMesh);

            const ShaderParams shaderParams("DeferredNonSkeletalBase Shader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "deferredCollectFS.glsl");

            TemplatedCompositeShaderParams<CompositeShader<StaticMeshVertexFactory, SimpleShader>> compositeParams(COMPOSITE_SHADER_TO_STR(StaticMeshVertexFactory, SimpleShader, mData.m_material->MaterialName), shaderParams, mData.m_material);
            CompositeShaderPool::sharedValue_t staticMeshShader = CompositeShaderPool::GetInstance()->template GetOrAllocateResource<CompositeShader<StaticMeshVertexFactory, SimpleShader>>(compositeParams);

            StaticMeshRenderData renderData(skin, staticMeshShader, mData.m_material);

            return std::make_shared<ComponentType>(mData.GameObjectName, mData.m_translation, mData.m_eulerRotationDegrees, mData.m_scale, renderData);
         }
      };

      template <typename ConstructType> struct CreatorFromMetaType<ComponentMetaType::SkeletalMesh, ConstructType>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data)
         {
            const MeshComponentData& mData = static_cast<const MeshComponentData&>(data);

            typename MeshPool::sharedValue_t skin = MeshPool::GetInstance()->GetOrAllocateResource(mData.m_pathToMesh);

            const ShaderParams shaderParams("DeferredNonSkeletalBase Shader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "deferredCollectFS.glsl");

            TemplatedCompositeShaderParams<CompositeShader<SkeletalMeshVertexFactory<4>, SimpleShader>> compositeParams(COMPOSITE_SHADER_TO_STR(SkeletalMeshVertexFactory<4>, SimpleShader, mData.m_material->MaterialName), shaderParams, mData.m_material);

            CompositeShaderPool::sharedValue_t skeletalMeshShader = CompositeShaderPool::GetInstance()->template GetOrAllocateResource<CompositeShader<SkeletalMeshVertexFactory<4>, SimpleShader>>(compositeParams);

            SkeletalMeshRenderData renderData(skin, skeletalMeshShader, mData.m_material);

            return std::make_shared<ComponentType>(mData.GameObjectName, mData.m_translation, mData.m_eulerRotationDegrees, mData.m_scale,
               mData.m_luaScriptPath, renderData);
         }
      };

      template <typename ConstructType> struct CreatorFromMetaType<ComponentMetaType::WaterPlane, ConstructType>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data)
         {

            const WaterPlaneComponentData& mData = static_cast<const WaterPlaneComponentData&>(data);

            const int32_t primitive = (int32_t)SimplePrimitiveType::PLANE_WITH_ATTRIBUTES;
            auto skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);

            const ShaderParams shaderParams("ForwardWaterPlane Shader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "forwardFS.glsl");

            TemplatedCompositeShaderParams<CompositeShader<StaticMeshVertexFactory, SimpleShader>> compositeParams(COMPOSITE_SHADER_TO_STR(StaticMeshVertexFactory, SimpleShader, mData.m_materialInstance->MaterialName), shaderParams, mData.m_materialInstance);
            CompositeShaderPool::sharedValue_t waterPlaneShader = CompositeShaderPool::GetInstance()->template GetOrAllocateResource<CompositeShader<StaticMeshVertexFactory, SimpleShader>>(compositeParams);

            return std::make_shared<ComponentType>(mData.GameObjectName, mData.m_translation, mData.m_eulerRotationDegrees, mData.m_scale, WaterPlaneRenderData(skin, waterPlaneShader, mData.m_materialInstance));
         }
      };

      template <typename ConstructType> struct CreatorFromMetaType<ComponentMetaType::Billboard, ConstructType>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data)
         {
            const BillboardComponentData& mData = static_cast<const BillboardComponentData&>(data);

            int32_t primitive = (int32_t)SimplePrimitiveType::POINT;
            SimplePrimitivePool::sharedValue_t skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);
            typename TexturePool::sharedValue_t texture = TexturePool::GetInstance()->GetOrAllocateResource(mData.m_pathToTexture);
            ShaderParams shaderParams("Billboard Shader", mData.m_vsShaderPath, mData.m_fsShaderPath, mData.m_gsShaderPath);
            ShaderPool::sharedValue_t shader = ShaderPool::GetInstance()->template GetOrAllocateResource<BillboardShader>(shaderParams);

            BillboardRenderData renderData(skin, shader, texture);

            return std::make_shared<ComponentType>(mData.GameObjectName, mData.m_translation, mData.m_eulerRotationDegrees, mData.m_scale, renderData);
         }
      };

      template <typename ConstructType> struct CreatorFromMetaType<ComponentMetaType::Cubemap, ConstructType>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data)
         {

            const CubemapComponentData& mData = static_cast<const CubemapComponentData&>(data);

            int32_t primitive = (int32_t)SimplePrimitiveType::CUBE;
            SimplePrimitivePool::sharedValue_t skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);
            ShaderParams shaderParams("Cubemap Shader", mData.m_vsShaderPath, mData.m_fsShaderPath);
            ShaderPool::sharedValue_t shader = ShaderPool::GetInstance()->template GetOrAllocateResource<CubemapShader>(shaderParams);

            CubemapRenderData renderData(skin, shader, mData.m_textureObtainer);

            return std::make_shared<ComponentType>(mData.GameObjectName, mData.m_translation, mData.m_eulerRotationDegrees, mData.m_scale, renderData);
         }
      };

      template <typename ConstructType> struct CreatorFromMetaType<ComponentMetaType::DirectionalLight, ConstructType>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data)
         {
            const DirectionalLightComponentData& mData = static_cast<const DirectionalLightComponentData&>(data);
            DirectionalLightRenderData renderData(mData.Direction, mData.Ambient, mData.Diffuse, mData.Specular, mData.ShadowInfo);
            return std::make_shared<ComponentType>(mData.GameObjectName, mData.Rotation, renderData);
         }
      };

      template <typename ConstructType> struct CreatorFromMetaType<ComponentMetaType::PointLight, ConstructType>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data)
         {
            const PointLightComponentData& mData = static_cast<const PointLightComponentData&>(data);
            PointLightRenderData renderData(mData.Attenuation, mData.RadianceSqrRadius, mData.Ambient, mData.Diffuse, mData.Specular, mData.ShadowInfo);
            return std::make_shared<ComponentType>(mData.GameObjectName, mData.Translation, mData.Rotation, renderData);
         }
      };

      template <typename ConstructType> struct CreatorFromMetaType<ComponentMetaType::CharacterMovement, ConstructType>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data)
         {
            const CharacterMovementComponentData& mData = static_cast<const CharacterMovementComponentData&>(data);
            return std::make_shared<ComponentType>(mData.GameObjectName, mData.m_launchDirection, mData.mCameraName);
         }
      };

      template <typename ConstructType> struct CreatorFromMetaType<ComponentMetaType::Movement, ConstructType>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data)
         {
            const MovementComponentData& mData = static_cast<const MovementComponentData&>(data);
            return std::make_shared<ComponentType>(mData.GameObjectName, mData.mScriptName);
         }
      };

      template <typename ConstructType> struct CreatorFromMetaType<ComponentMetaType::Input, ConstructType>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data)
         {
            return std::make_shared<ComponentType>(data.GameObjectName);
         }
      };

      template <typename ConstructType> struct CreatorFromMetaType<ComponentMetaType::Physics, ConstructType>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data)
         {
            const PhysicsComponentData& mData = static_cast<const PhysicsComponentData&>(data);
            return std::make_shared<ComponentType>(mData.GameObjectName, mData.mPhysicsDescriptor);
         }
      };

   public:

      static std::shared_ptr<Component> CreateComponent(const ComponentData& data)
      {
         CreatorFromMetaType<metaType, ComponentType> creator;
         return creator.CreateComponent(data);
      }
   };
}