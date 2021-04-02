#pragma once

#include <memory>

#include "Component.h"
#include "ComponentType.h"

#include "DirectionalLightComponent.h"
#include "PointLightComponent.h"
#include "SpotlightComponent.h"
#include "PlanarReflectionComponent.h"
#include "Core/GameCore/ACamera.h"

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
#include "ComponentData/SpotlightComponentData.h"
#include "ComponentData/CubemapComponentData.h"
#include "ComponentData/WaterPlaneComponentData.h"
#include "ComponentData/MovementComponentData.h"
#include "ComponentData/BillboardComponentData.h"
#include "ComponentData/PhysicsComponentData.h"
#include "ComponentData/PlanarReflectionComponentData.h"

#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"

#include "Core/GraphicsCore/OpenGL/Shader/CompositeShaderParams.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"

#include "Core/GameCore/ShaderImplementation/CubemapShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"

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
      Spotlight,
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
      PlanarReflection
   };

   template <typename VertexFactoryType, typename BaseShaderType>
   typename CompositeShaderPool::sharedValue_t CreateMaterialShader(const std::string& compositeShaderName, const ShaderParams& shaderParams, std::shared_ptr<MaterialProxy> materialProxy)
   {
      TemplatedCompositeShaderParams<CompositeShader<VertexFactoryType, BaseShaderType>> compositeParams(compositeShaderName, shaderParams, materialProxy);
      return CompositeShaderPool::GetInstance()->template GetOrAllocateResource<CompositeShader<VertexFactoryType, BaseShaderType>>(compositeParams);
   }

   std::shared_ptr<MaterialProxy> RegisterMaterialOnScene(class Scene* scene, IMaterial* material);

   template <ComponentMetaType metaType, typename ComponentType>
   struct ComponentCreatorFactory
   {
   private:

      template <ComponentMetaType metaType> struct CreatorFromMetaType { };

      template <>
      struct CreatorFromMetaType<ComponentMetaType::Skybox>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
         {
            const SkyboxComponentData& mData = static_cast<const SkyboxComponentData&>(data);

            int32_t primitive = (int32_t)SimplePrimitiveType::CUBE;
            auto skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);

            const auto& materialProxy = RegisterMaterialOnScene(scene, mData.m_material);

            const ShaderParams shaderParams("SkyboxForwardShader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "forwardFS.glsl");

            typename CompositeShaderPool::sharedValue_t skyboxMeshShader =
               CreateMaterialShader<SkyboxVertexFactory, SimpleShader>(COMPOSITE_SHADER_TO_STR(SkyboxVertexFactory, SimpleShader, materialProxy->MaterialName), shaderParams, materialProxy);

            const ShaderParams planarReflectionParams("PlanarReflectionShader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "planarReflectionVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "forwardFS.glsl");

            typename CompositeShaderPool::sharedValue_t planarReflectionShader =
               CreateMaterialShader<SkyboxVertexFactory, CapturePlanarReflectionShader>(COMPOSITE_SHADER_TO_STR(SkyboxVertexFactory, CapturePlanarReflectionShader, materialProxy->MaterialName), planarReflectionParams, materialProxy);

            return std::make_shared<ComponentType>(mData.GameObjectName, mData.m_scale, SkyboxRenderData(skin, skyboxMeshShader, planarReflectionShader, materialProxy));
         }
      };

      template <>
      struct CreatorFromMetaType<ComponentMetaType::StaticMesh>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
         {
            std::shared_ptr<Skin> skin = nullptr;

            const MeshComponentData& mData = static_cast<const MeshComponentData&>(data);
            if (mData.IsSimpleMesh())
            {
               const SimpleMeshComponentData& simpleMeshData = static_cast<const SimpleMeshComponentData&>(mData);
               if ("PLANE" == simpleMeshData.mSimpleMeshType) {
                  skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource((int32_t)SimplePrimitiveType::PLANE_WITH_ATTRIBUTES);
               }
               else assert(false); // Not implemented
            }
            else
            {
               skin = MeshPool::GetInstance()->GetOrAllocateResource(mData.m_pathToMesh);
            }

            const auto& materialProxy = RegisterMaterialOnScene(scene, mData.m_material);

            const ShaderParams shaderParams("DeferredNonSkeletalBase Shader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "deferredFS.glsl");

            typename CompositeShaderPool::sharedValue_t staticMeshShader =
               CreateMaterialShader<StaticMeshVertexFactory, SimpleShader>(COMPOSITE_SHADER_TO_STR(StaticMeshVertexFactory, SimpleShader, materialProxy->MaterialName), shaderParams, materialProxy);

            const ShaderParams planarReflectionParams("PlanarReflectionShader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "planarReflectionVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "forwardFS.glsl");

            typename CompositeShaderPool::sharedValue_t planarReflectionShader =
               CreateMaterialShader<StaticMeshVertexFactory, CapturePlanarReflectionShader>(COMPOSITE_SHADER_TO_STR(StaticMeshVertexFactory, CapturePlanarReflectionShader, materialProxy->MaterialName), planarReflectionParams, materialProxy);

            StaticMeshRenderData renderData(skin, staticMeshShader, planarReflectionShader, materialProxy);

            return std::make_shared<ComponentType>(mData.GameObjectName, mData.m_translation, mData.m_eulerRotationDegrees, mData.m_scale, renderData);
         }
      };

      template <>
      struct CreatorFromMetaType<ComponentMetaType::SkeletalMesh>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
         {
            const MeshComponentData& mData = static_cast<const MeshComponentData&>(data);

            typename MeshPool::sharedValue_t skin = MeshPool::GetInstance()->GetOrAllocateResource(mData.m_pathToMesh);

            const auto& materialProxy = RegisterMaterialOnScene(scene, mData.m_material);

            const ShaderParams shaderParams("DeferredNonSkeletalBase Shader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "deferredFS.glsl");

            typename CompositeShaderPool::sharedValue_t skeletalMeshShader =
               CreateMaterialShader<SkeletalMeshVertexFactory<4>, SimpleShader>(COMPOSITE_SHADER_TO_STR(SkeletalMeshVertexFactory<4>, SimpleShader, materialProxy->MaterialName), shaderParams, materialProxy);

            const ShaderParams planarReflectionParams("PlanarReflectionShader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "planarReflectionVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "forwardFS.glsl");

            typename CompositeShaderPool::sharedValue_t planarReflectionShader =
               CreateMaterialShader<SkeletalMeshVertexFactory<4>, CapturePlanarReflectionShader>(COMPOSITE_SHADER_TO_STR(SkeletalMeshVertexFactory<4>, CapturePlanarReflectionShader, materialProxy->MaterialName), planarReflectionParams, materialProxy);

            SkeletalMeshRenderData renderData(skin, skeletalMeshShader, planarReflectionShader, materialProxy);

            return std::make_shared<ComponentType>(mData.GameObjectName, mData.m_translation, mData.m_eulerRotationDegrees, mData.m_scale,
               mData.m_luaScriptPath, renderData);
         }
      };

      template <>
      struct CreatorFromMetaType<ComponentMetaType::WaterPlane>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
         {

            const WaterPlaneComponentData& mData = static_cast<const WaterPlaneComponentData&>(data);

            const int32_t primitive = (int32_t)SimplePrimitiveType::PLANE_WITH_ATTRIBUTES;
            auto skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);

            const auto& materialProxy = RegisterMaterialOnScene(scene, mData.m_material);

            const ShaderParams shaderParams("ForwardWaterPlane Shader",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "simpleVS.glsl",
               FolderManager::GetInstance()->GetShadersPath() + "composite_shaders\\" + "forwardFS.glsl");

            typename CompositeShaderPool::sharedValue_t waterPlaneShader =
               CreateMaterialShader<StaticMeshVertexFactory, SimpleShader>(COMPOSITE_SHADER_TO_STR(StaticMeshVertexFactory, SimpleShader, materialProxy->MaterialName), shaderParams, materialProxy);

            return std::make_shared<ComponentType>(mData.GameObjectName, mData.m_translation,
               mData.m_eulerRotationDegrees, mData.m_scale, WaterPlaneRenderData(skin, waterPlaneShader, materialProxy));
         }
      };

      template <>
      struct CreatorFromMetaType<ComponentMetaType::Billboard>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
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

      template <>
      struct CreatorFromMetaType<ComponentMetaType::Cubemap>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
         {

            const CubemapComponentData& mData = static_cast<const CubemapComponentData&>(data);

            const int32_t primitive = (int32_t)SimplePrimitiveType::CUBE;
            SimplePrimitivePool::sharedValue_t skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);
            ShaderParams shaderParams("Cubemap Shader", mData.m_vsShaderPath, mData.m_fsShaderPath);
            ShaderPool::sharedValue_t shader = ShaderPool::GetInstance()->template GetOrAllocateResource<CubemapShader>(shaderParams);

            CubemapRenderData renderData(skin, shader, mData.m_textureObtainer);

            return std::make_shared<ComponentType>(mData.GameObjectName, mData.m_translation, mData.m_eulerRotationDegrees, mData.m_scale, renderData);
         }
      };

      template <>
      struct CreatorFromMetaType<ComponentMetaType::DirectionalLight>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
         {
            const DirectionalLightComponentData& mData = static_cast<const DirectionalLightComponentData&>(data);
            DirectionalLightRenderData renderData(mData.Direction, mData.Ambient, mData.Diffuse, mData.Specular, mData.ShadowInfo);
            return std::make_shared<ComponentType>(mData.GameObjectName, mData.Rotation, renderData);
         }
      };

      template <>
      struct CreatorFromMetaType<ComponentMetaType::PointLight>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
         {
            const PointLightComponentData& mData = static_cast<const PointLightComponentData&>(data);
            PointLightRenderData renderData(mData.Attenuation, mData.RadianceRadius, mData.Ambient, mData.Diffuse, mData.Specular, mData.ShadowInfo);
            return std::make_shared<ComponentType>(mData.GameObjectName, mData.Translation, renderData);
         }
      };

      template <>
      struct CreatorFromMetaType<ComponentMetaType::Spotlight>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
         {
            const SpotlightComponentData& mData = static_cast<const SpotlightComponentData&>(data);
            SpotlightRenderData renderData(mData.Attenuation, mData.RadianceRadius, mData.Cutoff, mData.Ambient, mData.Diffuse, mData.Specular, mData.ShadowInfo);
            return std::make_shared<ComponentType>(mData.GameObjectName, mData.Translation, mData.Rotation, renderData);
         }
      };

      template <>
      struct CreatorFromMetaType<ComponentMetaType::CharacterMovement>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
         {
            const CharacterMovementComponentData& mData = static_cast<const CharacterMovementComponentData&>(data);
            return std::make_shared<ComponentType>(mData.GameObjectName, mData.m_launchDirection, mData.mCameraName);
         }
      };

      template <>
      struct CreatorFromMetaType<ComponentMetaType::Movement>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
         {
            const MovementComponentData& mData = static_cast<const MovementComponentData&>(data);
            return std::make_shared<ComponentType>(mData.GameObjectName, mData.mScriptName);
         }
      };

      template <>
      struct CreatorFromMetaType<ComponentMetaType::Input>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
         {
            return std::make_shared<ComponentType>(data.GameObjectName);
         }
      };

      template <>
      struct CreatorFromMetaType<ComponentMetaType::Physics>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
         {
            const PhysicsComponentData& mData = static_cast<const PhysicsComponentData&>(data);
            return std::make_shared<ComponentType>(mData.GameObjectName, mData.mPhysicsDescriptor);
         }
      };

      template <>
      struct CreatorFromMetaType<ComponentMetaType::PlanarReflection>
      {
         std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
         {
            const PlanarReflectionComponentData& mData = static_cast<const PlanarReflectionComponentData&>(data);
            assert(mData.m_ownerCamera);
            const auto& component = std::make_shared<ComponentType>(mData.GameObjectName, mData.m_translation,
               mData.m_eulerRotationDegrees, mData.m_scale, mData.m_ownerCamera, mData.m_fboViewPortInfo);

            mData.m_ownerCamera->SetPlanarReflectionComponent(component);
            return component;
         }
      };

   public:

      static std::shared_ptr<Component> CreateComponent(const ComponentData& data, class Scene* scene)
      {
         CreatorFromMetaType<metaType> creator;
         return creator.CreateComponent(data, scene);
      }
   };
}