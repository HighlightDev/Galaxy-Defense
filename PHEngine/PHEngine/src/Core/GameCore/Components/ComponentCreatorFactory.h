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
#include "ComponentData/StaticMeshComponentData.h"
#include "ComponentData/DirectionalLightComponentData.h"
#include "ComponentData/PointLightComponentData.h"
#include "ComponentData/SkeletalMeshComponentData.h"
#include "ComponentData/CubemapComponentData.h"
#include "ComponentData/WaterPlaneComponentData.h"
#include "ComponentData/MovementComponentData.h"
#include "ComponentData/BillboardComponentData.h"
#include "ComponentData/PhyShapeDebugComponentData.h"

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
   template <typename ComponentType>
   struct ComponentCreatorFactory;

   // Skybox component
   template <>
   struct ComponentCreatorFactory<SkyboxComponent>
   {
      static std::shared_ptr<Component> CreateComponent(const ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;
         if (SKYBOX_COMPONENT == data.GetType())
         {
            const SkyboxComponentData& mData = static_cast<const SkyboxComponentData&>(data);

            int32_t primitive = (int32_t)SimplePrimitiveType::CUBE;
            auto skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);

            const ShaderParams shaderParams("Skybox ForwardShader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "forwardFS.glsl");

            TemplatedCompositeShaderParams<CompositeShader<SkyboxVertexFactory, SimpleShader>> compositeParams(COMPOSITE_SHADER_TO_STR(SkyboxVertexFactory, SimpleShader, mData.m_materialInstance->MaterialName), shaderParams, mData.m_materialInstance);
            CompositeShaderPool::sharedValue_t skyboxMeshShader = CompositeShaderPool::GetInstance()->template GetOrAllocateResource<CompositeShader<SkyboxVertexFactory, SimpleShader>>(compositeParams);

            resultComponent = std::make_shared<SkyboxComponent>(mData.m_scale, SkyboxRenderData(skin, skyboxMeshShader, mData.m_materialInstance));
         }

         return resultComponent;
      }
   };

   //Static Mesh component
   template <>
   struct ComponentCreatorFactory<StaticMeshComponent>
   {
      static std::shared_ptr<Component> CreateComponent(const ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;
         if (STATIC_MESH_COMPONENT == data.GetType())
         {
            const StaticMeshComponentData& mData = static_cast<const StaticMeshComponentData&>(data);

            typename MeshPool::sharedValue_t skin = MeshPool::GetInstance()->GetOrAllocateResource(mData.m_pathToMesh);

            const ShaderParams shaderParams("DeferredNonSkeletalBase Shader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "deferredCollectFS.glsl");

            TemplatedCompositeShaderParams<CompositeShader<StaticMeshVertexFactory, SimpleShader>> compositeParams(COMPOSITE_SHADER_TO_STR(StaticMeshVertexFactory, SimpleShader, mData.m_material->MaterialName), shaderParams, mData.m_material);
            CompositeShaderPool::sharedValue_t staticMeshShader = CompositeShaderPool::GetInstance()->template GetOrAllocateResource<CompositeShader<StaticMeshVertexFactory, SimpleShader>>(compositeParams);

            StaticMeshRenderData renderData(skin, staticMeshShader, mData.m_material);

            resultComponent = std::make_shared<StaticMeshComponent>(std::move(mData.m_translation), std::move(mData.m_eulerRotationDegrees), std::move(mData.m_scale), renderData);
         }

         return resultComponent;
      }
   };

   // Directional light component
   template <>
   struct ComponentCreatorFactory<DirectionalLightComponent>
   {
      static std::shared_ptr<Component> CreateComponent(const ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;
         if (DIR_LIGHT_COMPONENT == data.GetType())
         {
            const DirectionalLightComponentData& mData = static_cast<const DirectionalLightComponentData&>(data);
            DirectionalLightRenderData renderData(mData.Direction, mData.Ambient, mData.Diffuse, mData.Specular, mData.ShadowInfo);
            resultComponent = std::make_shared<DirectionalLightComponent>(std::move(mData.Rotation), renderData);
         }

         return resultComponent;
      }
   };

   // Point light component
   template <>
   struct ComponentCreatorFactory<PointLightComponent>
   {
      static std::shared_ptr<Component> CreateComponent(const ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;
         if (POINT_LIGHT_COMPONENT == data.GetType())
         {
            const PointLightComponentData& mData = static_cast<const PointLightComponentData&>(data);
            PointLightRenderData renderData(mData.Attenuation, mData.RadianceSqrRadius, mData.Ambient, mData.Diffuse, mData.Specular, mData.ShadowInfo);
            resultComponent = std::make_shared<PointLightComponent>(std::move(mData.Translation), std::move(mData.Rotation), renderData);
         }

         return resultComponent;
      }
   };

   // Water plane component
   template <>
   struct ComponentCreatorFactory<WaterPlaneComponent>
   {
      static std::shared_ptr<Component> CreateComponent(const ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;

         if (WATER_PLANE_COMPONENT == data.GetType())
         {
            const WaterPlaneComponentData& mData = static_cast<const WaterPlaneComponentData&>(data);

            const int32_t primitive = (int32_t)SimplePrimitiveType::PLANE_WITH_ATTRIBUTES;
            auto skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);

            const ShaderParams shaderParams("ForwardWaterPlane Shader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "forwardFS.glsl");

            TemplatedCompositeShaderParams<CompositeShader<StaticMeshVertexFactory, SimpleShader>> compositeParams(COMPOSITE_SHADER_TO_STR(StaticMeshVertexFactory, SimpleShader, mData.m_materialInstance->MaterialName), shaderParams, mData.m_materialInstance);
            CompositeShaderPool::sharedValue_t waterPlaneShader = CompositeShaderPool::GetInstance()->template GetOrAllocateResource<CompositeShader<StaticMeshVertexFactory, SimpleShader>>(compositeParams);

            resultComponent = std::make_shared<WaterPlaneComponent>(mData.m_translation, mData.m_eulerRotationDegrees, mData.m_scale, WaterPlaneRenderData(skin, waterPlaneShader, mData.m_materialInstance));
         }

         return resultComponent;
      }
   };

   // Input component
   template <>
   struct ComponentCreatorFactory<InputComponent>
   {
      static std::shared_ptr<Component> CreateComponent(const ComponentData& data)
      {
         return std::make_shared<InputComponent>();
      }
   };

   // Movement component 
   template <>
   struct ComponentCreatorFactory<MovementComponent>
   {
      static std::shared_ptr<Component> CreateComponent(const ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;
         if (MOVEMENT_COMPONENT == data.GetType())
         {
            const MovementComponentData& mData = static_cast<const MovementComponentData&>(data);
            resultComponent = std::make_shared<MovementComponent>(mData.mCameraName, mData.m_launchVelocity);
         }

         return resultComponent;
      }
   };

   // Skeletal mesh component
   template <>
   struct ComponentCreatorFactory<SkeletalMeshComponent>
   {

      static std::shared_ptr<Component> CreateComponent(const ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;

         if (SKELETAL_MESH_COMPONENT == data.GetType())
         {
            const SkeletalMeshComponentData& mData = static_cast<const SkeletalMeshComponentData&>(data);

            typename MeshPool::sharedValue_t skin = MeshPool::GetInstance()->GetOrAllocateResource(mData.m_pathToMesh);

            const ShaderParams shaderParams("DeferredNonSkeletalBase Shader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "deferredCollectFS.glsl");
           
            TemplatedCompositeShaderParams<CompositeShader<SkeletalMeshVertexFactory<3>, SimpleShader>> compositeParams(COMPOSITE_SHADER_TO_STR(SkeletalMeshVertexFactory<3>, SimpleShader, mData.m_material->MaterialName), shaderParams, mData.m_material);

            CompositeShaderPool::sharedValue_t skeletalMeshShader = CompositeShaderPool::GetInstance()->template GetOrAllocateResource<CompositeShader<SkeletalMeshVertexFactory<3>, SimpleShader>>(compositeParams);

            SkeletalMeshRenderData renderData(skin, skeletalMeshShader, mData.m_material);
             
            resultComponent = std::make_shared<SkeletalMeshComponent>(std::move(mData.m_translation), std::move(mData.m_eulerRotationDegrees), std::move(mData.m_scale), renderData);
         }

         return resultComponent;
      }

   };

   // Billboard component
   template<>
   struct ComponentCreatorFactory<BillboardComponent>
   {
      static std::shared_ptr<Component> CreateComponent(const ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;

         {
            const BillboardComponentData& mData = static_cast<const BillboardComponentData&>(data);

            int32_t primitive = (int32_t)SimplePrimitiveType::POINT;
            SimplePrimitivePool::sharedValue_t skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);
            typename TexturePool::sharedValue_t texture = TexturePool::GetInstance()->GetOrAllocateResource(mData.m_pathToTexture);
            ShaderParams shaderParams("Billboard Shader", mData.m_vsShaderPath, mData.m_fsShaderPath, mData.m_gsShaderPath);
            ShaderPool::sharedValue_t shader = ShaderPool::GetInstance()->template GetOrAllocateResource<BillboardShader>(shaderParams);

            BillboardRenderData renderData(skin, shader, texture);

            resultComponent = std::make_shared<BillboardComponent>(std::move(mData.m_translation), std::move(mData.m_eulerRotationDegrees), std::move(mData.m_scale), renderData);
         }

         return resultComponent;
      }
   };

   // Cubemap component
   template<>
   struct ComponentCreatorFactory<CubemapComponent>
   {
      static std::shared_ptr<Component> CreateComponent(const ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;

         {
            const CubemapComponentData& mData = static_cast<const CubemapComponentData&>(data);

            int32_t primitive = (int32_t)SimplePrimitiveType::CUBE;
            SimplePrimitivePool::sharedValue_t skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);
            ShaderParams shaderParams("Cubemap Shader", mData.m_vsShaderPath, mData.m_fsShaderPath);
            ShaderPool::sharedValue_t shader = ShaderPool::GetInstance()->template GetOrAllocateResource<CubemapShader>(shaderParams);

            CubemapRenderData renderData(skin, shader, mData.m_textureObtainer);

            resultComponent = std::make_shared<CubemapComponent>(std::move(mData.m_translation), std::move(mData.m_eulerRotationDegrees), std::move(mData.m_scale), renderData);
         }

         return resultComponent;
      }
   };
}