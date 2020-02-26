#pragma once

#include <memory>

#include "Component.h"
#include "SkyboxComponent.h"
#include "StaticMeshComponent.h"
#include "DirectionalLightComponent.h"
#include "PointLightComponent.h"
#include "BillboardComponent.h"
#include "CubemapComponent.h"
#include "ComponentType.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/AnimationPool.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentData/SkyboxComponentData.h"
#include "Core/GameCore/Components/ComponentData/StaticMeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/PointLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/SkeletalMeshComponentData.h"
#include "Core/GameCore/Components/ComponentData/CubemapComponentData.h"
#include "Core/GameCore/Components/WaterPlaneComponent.h"
#include "Core/GameCore/Components/ComponentData/WaterPlaneComponentData.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/SkeletalMeshComponent.h"
#include "Core/GameCore/ShaderImplementation/SkeletalMeshShader.h"
#include "Core/GameCore/ShaderImplementation/CubemapShader.h"
#include "Core/GameCore/ShaderImplementation/DeferredCollectShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkeletalMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShaderParams.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"

using namespace Resources;
using namespace Common;

namespace Game
{
   template <typename ComponentType>
   struct ComponentCreatorFactory;

   // Skybox component
   template <>
   struct ComponentCreatorFactory<SkyboxComponent>
   {
      static std::shared_ptr<Component> CreateComponent(ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;
         if (SKYBOX_COMPONENT == data.GetType())
         {
            SkyboxComponentData& mData = static_cast<SkyboxComponentData&>(data);

            int32_t primitive = (int32_t)SimplePrimitiveType::CUBE;
            auto skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);

            typename TexturePool::sharedValue_t dTex = TexturePool::GetInstance()->GetOrAllocateResource(mData.m_commaSeparatedPathToSixTexturesDay);
            typename TexturePool::sharedValue_t nTex;
            if (mData.m_commaSeparatedPathToSixTexturesNight != "")
               nTex = TexturePool::GetInstance()->GetOrAllocateResource(mData.m_commaSeparatedPathToSixTexturesNight);

            ShaderParams shaderParams("Skybox Shader", mData.m_vsShaderPath, mData.m_fsShaderPath, "", "", "", "");
            ShaderPool::sharedValue_t skyboxShader = ShaderPool::GetInstance()->template GetOrAllocateResource<SkyboxShader>(shaderParams);

            SkyboxRenderData renderData(skin, skyboxShader, dTex, nTex);

            resultComponent = std::make_shared<SkyboxComponent>(mData.m_scale, mData.m_rotateSpeed, renderData);
         }

         return resultComponent;
      }
   };

   //Static Mesh component
   template <>
   struct ComponentCreatorFactory<StaticMeshComponent>
   {
      static std::shared_ptr<Component> CreateComponent(ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;
         if (STATIC_MESH_COMPONENT == data.GetType())
         {
            StaticMeshComponentData& mData = static_cast<StaticMeshComponentData&>(data);

            typename MeshPool::sharedValue_t skin = MeshPool::GetInstance()->GetOrAllocateResource(mData.m_pathToMesh);

            const ShaderParams shaderParams("DeferredNonSkeletalBase Shader", FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl", FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "deferredCollectFS.glsl", "", "", "", "");
            TemplatedCompositeShaderParams<CompositeShader<StaticMeshVertexFactory, DeferredCollectShader>> compositeParams(COMPOSITE_SHADER_TO_STR(StaticMeshVertexFactory, DeferredCollectShader, mData.m_material->MaterialName), shaderParams, mData.m_material);
            CompositeShaderPool::sharedValue_t staticMeshShader = CompositeShaderPool::GetInstance()->template GetOrAllocateResource<CompositeShader<StaticMeshVertexFactory, DeferredCollectShader>>(compositeParams);

            StaticMeshRenderData renderData(skin, staticMeshShader);

            resultComponent = std::make_shared<StaticMeshComponent>(std::move(mData.m_translation), std::move(mData.m_rotation), std::move(mData.m_scale), renderData);
         }

         return resultComponent;
      }
   };

   // Directional light component
   template <>
   struct ComponentCreatorFactory<DirectionalLightComponent>
   {
      static std::shared_ptr<Component> CreateComponent(ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;
         if (DIR_LIGHT_COMPONENT == data.GetType())
         {
            DirectionalLightComponentData& mData = static_cast<DirectionalLightComponentData&>(data);
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
      static std::shared_ptr<Component> CreateComponent(ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;
         if (POINT_LIGHT_COMPONENT == data.GetType())
         {
            PointLightComponentData& mData = static_cast<PointLightComponentData&>(data);
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
      static std::shared_ptr<Component> CreateComponent(ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;

         if (WATER_PLANE_COMPONENT == data.GetType())
         {
            WaterPlaneComponentData& mData = static_cast<WaterPlaneComponentData&>(data);

            const int32_t primitive = (int32_t)SimplePrimitiveType::PLANE_WITH_ATTRIBUTES;
            auto skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);

            const ShaderParams shaderParams("ForwardWaterPlane Shader", FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl", FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "forwardFS.glsl", "", "", "", "");
            TemplatedCompositeShaderParams<CompositeShader<StaticMeshVertexFactory, DeferredCollectShader>> compositeParams(COMPOSITE_SHADER_TO_STR(StaticMeshVertexFactory, DeferredCollectShader, mData.m_materialInstance->MaterialName), shaderParams, mData.m_materialInstance);
            CompositeShaderPool::sharedValue_t waterPlaneShader = CompositeShaderPool::GetInstance()->template GetOrAllocateResource<CompositeShader<StaticMeshVertexFactory, DeferredCollectShader>>(compositeParams);

            resultComponent = std::make_shared<WaterPlaneComponent>(mData.m_translation, mData.m_rotation, mData.m_scale, WaterPlaneRenderData(skin, waterPlaneShader));
         }

         return resultComponent;
      }
   };

   // Input component
   template <>
   struct ComponentCreatorFactory<InputComponent>
   {
      static std::shared_ptr<Component> CreateComponent(ComponentData& data)
      {
         return std::make_shared<InputComponent>();
      }
   };

   // Movement component 
   template <>
   struct ComponentCreatorFactory<MovementComponent>
   {
      static std::shared_ptr<Component> CreateComponent(ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;
         if (MOVEMENT_COMPONENT == data.GetType())
         {
            MovementComponentData& mData = static_cast<MovementComponentData&>(data);
            resultComponent = std::make_shared<MovementComponent>(mData.m_camera, mData.m_launchVelocity);
         }

         return resultComponent;
      }
   };

   // Skeletal mesh component
   template <>
   struct ComponentCreatorFactory<SkeletalMeshComponent>
   {

      static std::shared_ptr<Component> CreateComponent(ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;

         if (SKELETAL_MESH_COMPONENT == data.GetType())
         {
            SkeletalMeshComponentData& mData = static_cast<SkeletalMeshComponentData&>(data);

            typename MeshPool::sharedValue_t skin = MeshPool::GetInstance()->GetOrAllocateResource(mData.m_pathToMesh);

            typename AnimationPool::sharedValue_t animations = AnimationPool::GetInstance()->GetOrAllocateResource(mData.m_pathToMesh);

            const ShaderParams shaderParams("DeferredNonSkeletalBase Shader", FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl", FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "deferredCollectFS.glsl", "", "", "", "");
           
            TemplatedCompositeShaderParams<CompositeShader<SkeletalMeshVertexFactory<3>, DeferredCollectShader>> compositeParams(COMPOSITE_SHADER_TO_STR(SkeletalMeshVertexFactory<3>, DeferredCollectShader, mData.m_material->MaterialName), shaderParams, mData.m_material);

            CompositeShaderPool::sharedValue_t skeletalMeshShader = CompositeShaderPool::GetInstance()->template GetOrAllocateResource<CompositeShader<SkeletalMeshVertexFactory<3>, DeferredCollectShader>>(compositeParams);

            SkeletalMeshRenderData renderData(skin, animations, skeletalMeshShader);
             
            resultComponent = std::make_shared<SkeletalMeshComponent>(std::move(mData.m_translation), std::move(mData.m_rotation), std::move(mData.m_scale), renderData);
         }

         return resultComponent;
      }

   };

   // Billboard component
   template<>
   struct ComponentCreatorFactory<BillboardComponent>
   {
      static std::shared_ptr<Component> CreateComponent(ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;

         {
            BillboardComponentData& mData = static_cast<BillboardComponentData&>(data);

            int32_t primitive = (int32_t)SimplePrimitiveType::POINT;
            SimplePrimitivePool::sharedValue_t skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);
            typename TexturePool::sharedValue_t texture = TexturePool::GetInstance()->GetOrAllocateResource(mData.m_pathToTexture);
            ShaderParams shaderParams("Billboard Shader", mData.m_vsShaderPath, mData.m_fsShaderPath, mData.m_gsShaderPath, "", "", "");
            ShaderPool::sharedValue_t shader = ShaderPool::GetInstance()->template GetOrAllocateResource<BillboardShader>(shaderParams);

            BillboardRenderData renderData(skin, shader, texture);

            resultComponent = std::make_shared<BillboardComponent>(std::move(mData.m_translation), std::move(mData.m_rotation), std::move(mData.m_scale), renderData);
         }

         return resultComponent;
      }
   };

   // Cubemap component
   template<>
   struct ComponentCreatorFactory<CubemapComponent>
   {
      static std::shared_ptr<Component> CreateComponent(ComponentData& data)
      {
         std::shared_ptr<Component> resultComponent;

         {
            CubemapComponentData& mData = static_cast<CubemapComponentData&>(data);

            int32_t primitive = (int32_t)SimplePrimitiveType::CUBE;
            SimplePrimitivePool::sharedValue_t skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);
            ShaderParams shaderParams("Cubemap Shader", mData.m_vsShaderPath, mData.m_fsShaderPath, "", "", "", "");
            ShaderPool::sharedValue_t shader = ShaderPool::GetInstance()->template GetOrAllocateResource<CubemapShader>(shaderParams);

            CubemapRenderData renderData(skin, shader, mData.m_textureObtainer);

            resultComponent = std::make_shared<CubemapComponent>(std::move(mData.m_translation), std::move(mData.m_rotation), std::move(mData.m_scale), renderData);
         }

         return resultComponent;
      }
   };

}