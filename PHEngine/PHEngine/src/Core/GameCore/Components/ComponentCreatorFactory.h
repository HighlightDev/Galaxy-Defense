#pragma once

#include <memory>
#include <type_traits>

#include "Component.h"
#include "ComponentType.h"

#include "Core/GameCore/ACamera.h"
#include "DirectionalLightComponent.h"
#include "PlanarReflectionComponent.h"
#include "PointLightComponent.h"
#include "SpotlightComponent.h"

#include "PrimitiveComponents/BillboardComponent.h"
#include "PrimitiveComponents/CubemapComponent.h"
#include "PrimitiveComponents/SkeletalMeshComponent.h"
#include "PrimitiveComponents/SkyboxComponent.h"
#include "PrimitiveComponents/StaticMeshComponent.h"
#include "PrimitiveComponents/WaterPlaneComponent.h"

#include "ComponentData/BillboardComponentData.h"
#include "ComponentData/ComponentData.h"
#include "ComponentData/CubemapComponentData.h"
#include "ComponentData/DirectionalLightComponentData.h"
#include "ComponentData/MeshComponentData.h"
#include "ComponentData/MovementComponentData.h"
#include "ComponentData/PhysicsComponentData.h"
#include "ComponentData/PlanarReflectionComponentData.h"
#include "ComponentData/PointLightComponentData.h"
#include "ComponentData/SkyboxComponentData.h"
#include "ComponentData/SpotlightComponentData.h"
#include "ComponentData/WaterPlaneComponentData.h"

#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

#include "Core/GraphicsCore/OpenGL/Shader/CompositeShaderParams.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"

#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GameCore/ShaderImplementation/CubemapShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"

#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkeletalMeshVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkyboxVertexFactory.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"

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
    typename CompositeShaderPool::sharedValue_t
    CreateMaterialShader(const std::string &compositeShaderName,
                         const ShaderParams &shaderParams,
                         std::shared_ptr<MaterialProxy> materialProxy)
    {
        TemplatedCompositeMaterialShaderParams<
            VertexFactoryMaterialCompositeShader<VertexFactoryType, BaseShaderType>>
            compositeParams(compositeShaderName, shaderParams, materialProxy);
        return CompositeShaderPool::GetInstance()
            ->template GetOrAllocateResource<VertexFactoryMaterialCompositeShader<
                VertexFactoryType, BaseShaderType>>(compositeParams);
    }

    std::shared_ptr<MaterialProxy> RegisterMaterialOnScene(class Scene *const scene,
                                                           IMaterial *material);

    struct IComponentCreator
    {
        virtual std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const = 0;
    };

    template <typename ComponentType>
    struct StaticMeshComponentCreator : public IComponentCreator
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const override
        {
            std::shared_ptr<Skin> skin = nullptr;

            const MeshComponentData &mData =
                static_cast<const MeshComponentData &>(data);
            if (mData.IsSimpleMesh())
            {
                const SimpleMeshComponentData &simpleMeshData =
                    static_cast<const SimpleMeshComponentData &>(mData);
                if ("PLANE" == simpleMeshData.mSimpleMeshType)
                {
                    skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(
                        (int32_t)SimplePrimitiveType::PLANE_WITH_ATTRIBUTES);
                }
                else
                    assert(false); // Not implemented
            }
            else
            {
                skin =
                    MeshPool::GetInstance()->GetOrAllocateResource(mData.m_pathToMesh);
            }

            const auto &materialProxy =
                RegisterMaterialOnScene(scene, mData.m_material);

            const ShaderParams shaderParams(
                "DeferredNonSkeletalBase Shader",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "simpleVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "deferredFS.glsl");

            typename CompositeShaderPool::sharedValue_t staticMeshShader =
                CreateMaterialShader<StaticMeshVertexFactory, SimpleShader>(
                    "StaticMeshVertexFactory_SimpleShader_" +
                        materialProxy->MaterialName,
                    shaderParams, materialProxy);

            const ShaderParams planarReflectionParams(
                "PlanarReflectionShader",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "planarReflectionVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "forwardFS.glsl");

            typename CompositeShaderPool::sharedValue_t planarReflectionShader =
                CreateMaterialShader<StaticMeshVertexFactory,
                                     CapturePlanarReflectionShader>(
                    "StaticMeshVertexFactory_CapturePlanarReflectionShader_" +
                        materialProxy->MaterialName,
                    planarReflectionParams, materialProxy);

            StaticMeshRenderData renderData(
                skin, staticMeshShader, planarReflectionShader, materialProxy);

            return std::make_shared<ComponentType>(
                mData.GameObjectName, mData.m_translation,
                mData.m_eulerRotationDegrees, mData.m_scale, renderData);
        }
    };

    template <typename ComponentType>
    struct SkeletalMeshComponentCreator : public IComponentCreator
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const override
        {
            const MeshComponentData &mData =
                static_cast<const MeshComponentData &>(data);

            typename MeshPool::sharedValue_t skin =
                MeshPool::GetInstance()->GetOrAllocateResource(mData.m_pathToMesh);

            const auto &materialProxy =
                RegisterMaterialOnScene(scene, mData.m_material);

            const ShaderParams shaderParams(
                "DeferredNonSkeletalBase Shader",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "simpleVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "deferredFS.glsl");

            typename CompositeShaderPool::sharedValue_t skeletalMeshShader =
                CreateMaterialShader<SkeletalMeshVertexFactory<4>, SimpleShader>(
                    "SkeletalMeshVertexFactory<4>_SimpleShader_" + materialProxy->MaterialName,
                    shaderParams, materialProxy);

            const ShaderParams planarReflectionParams(
                "PlanarReflectionShader",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "planarReflectionVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "forwardFS.glsl");

            typename CompositeShaderPool::sharedValue_t planarReflectionShader =
                CreateMaterialShader<SkeletalMeshVertexFactory<4>,
                                     CapturePlanarReflectionShader>(
                    "SkeletalMeshVertexFactory<4>_CapturePlanarReflectionShader" + materialProxy->MaterialName,
                    planarReflectionParams, materialProxy);

            SkeletalMeshRenderData renderData(skin, skeletalMeshShader,
                                              planarReflectionShader, materialProxy);

            return std::make_shared<ComponentType>(
                mData.GameObjectName, mData.m_translation,
                mData.m_eulerRotationDegrees, mData.m_scale, mData.m_luaScriptPath,
                renderData);
        }
    };

    template <typename ComponentType>
    struct DirectionalLightComponentCreator : public IComponentCreator
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const override
        {
            const DirectionalLightComponentData &mData =
                static_cast<const DirectionalLightComponentData &>(data);
            DirectionalLightRenderData renderData(mData.Direction, mData.Ambient,
                                                  mData.Diffuse, mData.Specular,
                                                  mData.ShadowInfo);
            return std::make_shared<ComponentType>(mData.GameObjectName,
                                                   mData.Rotation, renderData);
        }
    };

    template <typename ComponentType>
    struct PointLightComponentCreator : public IComponentCreator
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const override
        {
            const PointLightComponentData &mData =
                static_cast<const PointLightComponentData &>(data);
            PointLightRenderData renderData(mData.Attenuation, mData.RadianceRadius,
                                            mData.Ambient, mData.Diffuse,
                                            mData.Specular, mData.ShadowInfo);
            return std::make_shared<ComponentType>(mData.GameObjectName,
                                                   mData.Translation, renderData);
        }
    };

    template <typename ComponentType>
    struct SpotlightComponentCreator : public IComponentCreator
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const override
        {
            const SpotlightComponentData &mData =
                static_cast<const SpotlightComponentData &>(data);
            SpotlightRenderData renderData(mData.Attenuation, mData.RadianceRadius,
                                           mData.Cutoff, mData.Ambient, mData.Diffuse,
                                           mData.Specular, mData.ShadowInfo);
            return std::make_shared<ComponentType>(
                mData.GameObjectName, mData.Translation, mData.Rotation, renderData);
        }
    };

    template <typename ComponentType>
    struct SkyboxComponentCreator : public IComponentCreator
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const override
        {
            const SkyboxComponentData &mData =
                static_cast<const SkyboxComponentData &>(data);

            int32_t primitive =
                (int32_t)SimplePrimitiveType::INVERTED_VERTICES_DIRECTION_CUBE;
            auto skin =
                SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);

            const auto &materialProxy =
                RegisterMaterialOnScene(scene, mData.m_material);

            const ShaderParams shaderParams(
                "SkyboxForwardShader",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "simpleVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "forwardFS.glsl");

            typename CompositeShaderPool::sharedValue_t skyboxMeshShader =
                CreateMaterialShader<SkyboxVertexFactory, SimpleShader>(
                    "SkyboxVertexFactory_SimpleShader_" + materialProxy->MaterialName,
                    shaderParams, materialProxy);

            const ShaderParams planarReflectionParams(
                "PlanarReflectionShader",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "planarReflectionVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "forwardFS.glsl");

            typename CompositeShaderPool::sharedValue_t planarReflectionShader =
                CreateMaterialShader<SkyboxVertexFactory, CapturePlanarReflectionShader>("SkyboxVertexFactory_CapturePlanarReflectionShader_" + materialProxy->MaterialName,
                                                                                         planarReflectionParams, materialProxy);

            return std::make_shared<ComponentType>(
                mData.GameObjectName, mData.m_scale,
                SkyboxRenderData(skin, skyboxMeshShader, planarReflectionShader,
                                 materialProxy));
        }
    };

    template <typename ComponentType>
    struct WaterPlaneComponentCreator : public IComponentCreator
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const override
        {
            const WaterPlaneComponentData &mData =
                static_cast<const WaterPlaneComponentData &>(data);

            const int32_t primitive =
                (int32_t)SimplePrimitiveType::PLANE_WITH_ATTRIBUTES;
            auto skin =
                SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);

            const auto &materialProxy =
                RegisterMaterialOnScene(scene, mData.m_material);

            const ShaderParams shaderParams(
                "ForwardWaterPlane Shader",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "simpleVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "forwardFS.glsl");

            typename CompositeShaderPool::sharedValue_t waterPlaneShader =
                CreateMaterialShader<StaticMeshVertexFactory, SimpleShader>(
                    "StaticMeshVertexFactory_SimpleShader_" + materialProxy->MaterialName,
                    shaderParams, materialProxy);

            return std::make_shared<ComponentType>(
                mData.GameObjectName, mData.m_translation,
                mData.m_eulerRotationDegrees, mData.m_scale,
                WaterPlaneRenderData(skin, waterPlaneShader, materialProxy));
        }
    };

    template <typename ComponentType>
    struct CubemapComponentCreator : public IComponentCreator
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const override
        {
            const CubemapComponentData &mData =
                static_cast<const CubemapComponentData &>(data);

            const int32_t primitive = (int32_t)SimplePrimitiveType::CUBE;
            SimplePrimitivePool::sharedValue_t skin =
                SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);
            ShaderParams shaderParams("Cubemap Shader", mData.m_vsShaderPath,
                                      mData.m_fsShaderPath);
            ShaderPool::sharedValue_t shader =
                ShaderPool::GetInstance()
                    ->template GetOrAllocateResource<CubemapShader>(shaderParams);

            CubemapRenderData renderData(skin, shader, mData.m_textureObtainer);

            return std::make_shared<ComponentType>(
                mData.GameObjectName, mData.m_translation,
                mData.m_eulerRotationDegrees, mData.m_scale, renderData);
        }
    };

    template <typename ComponentType>
    struct BillboardComponentCreator : public IComponentCreator
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const override
        {
            const BillboardComponentData &mData =
                static_cast<const BillboardComponentData &>(data);

            int32_t primitive = (int32_t)SimplePrimitiveType::POINT;
            SimplePrimitivePool::sharedValue_t skin =
                SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);
            typename TexturePool::sharedValue_t texture =
                TexturePool::GetInstance()->GetOrAllocateResource(
                    mData.m_pathToTexture);
            ShaderParams shaderParams("Billboard Shader", mData.m_vsShaderPath,
                                      mData.m_fsShaderPath, mData.m_gsShaderPath);
            ShaderPool::sharedValue_t shader =
                ShaderPool::GetInstance()
                    ->template GetOrAllocateResource<BillboardShader>(shaderParams);

            BillboardRenderData renderData(skin, shader, texture);

            return std::make_shared<ComponentType>(
                mData.GameObjectName, mData.m_translation,
                mData.m_eulerRotationDegrees, mData.m_scale, renderData);
        }
    };

    template <typename ComponentType>
    struct InputComponentCreator : public IComponentCreator
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const override
        {
            return std::make_shared<ComponentType>(data.GameObjectName);
        }
    };

    template <typename ComponentType>
    struct CharacterMovementComponentCreator : public IComponentCreator
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const override
        {
            const CharacterMovementComponentData &mData =
                static_cast<const CharacterMovementComponentData &>(data);
            return std::make_shared<ComponentType>(
                mData.GameObjectName, mData.m_launchDirection, mData.mCameraName);
        }
    };

    template <typename ComponentType>
    struct MovementComponentCreator : public IComponentCreator
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const override
        {
            const MovementComponentData &mData =
                static_cast<const MovementComponentData &>(data);
            return std::make_shared<ComponentType>(mData.GameObjectName,
                                                   mData.mScriptName);
        }
    };

    template <typename ComponentType>
    struct PhysicsComponentCreator : public IComponentCreator
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const override
        {
            const PhysicsComponentData &mData =
                static_cast<const PhysicsComponentData &>(data);
            return std::make_shared<ComponentType>(mData.GameObjectName,
                                                   mData.mPhysicsDescriptor);
        }
    };

    template <typename ComponentType>
    struct PlanarReflectionComponentCreator : public IComponentCreator
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data, class Scene *const scene) const override
        {
            const PlanarReflectionComponentData &mData =
                static_cast<const PlanarReflectionComponentData &>(data);
            assert(mData.m_ownerCamera);
            const auto &component = std::make_shared<ComponentType>(
                mData.GameObjectName, mData.m_translation,
                mData.m_eulerRotationDegrees, mData.m_scale, mData.m_ownerCamera,
                mData.m_fboViewPortInfo);

            mData.m_ownerCamera->SetPlanarReflectionComponent(component);
            return component;
        }
    };

    template <typename ComponentType, ComponentMetaType componentMetaType>
    typename std::enable_if<componentMetaType == ComponentMetaType::DirectionalLight, std::unique_ptr<IComponentCreator>>::type CreateComponentCreatorInstance()
    {
        return std::make_unique<DirectionalLightComponentCreator<ComponentType>>();
    }

    template <typename ComponentType, ComponentMetaType componentMetaType>
    typename std::enable_if<componentMetaType == ComponentMetaType::PointLight, std::unique_ptr<IComponentCreator>>::type CreateComponentCreatorInstance()
    {
        return std::make_unique<PointLightComponentCreator<ComponentType>>();
    }

    template <typename ComponentType, ComponentMetaType componentMetaType>
    typename std::enable_if<componentMetaType == ComponentMetaType::Spotlight, std::unique_ptr<IComponentCreator>>::type CreateComponentCreatorInstance()
    {
        return std::make_unique<SpotlightComponentCreator<ComponentType>>();
    }

    template <typename ComponentType, ComponentMetaType componentMetaType>
    typename std::enable_if<componentMetaType == ComponentMetaType::Skybox, std::unique_ptr<IComponentCreator>>::type CreateComponentCreatorInstance()
    {
        return std::make_unique<SkyboxComponentCreator<ComponentType>>();
    }

    template <typename ComponentType, ComponentMetaType componentMetaType>
    typename std::enable_if<componentMetaType == ComponentMetaType::StaticMesh, std::unique_ptr<IComponentCreator>>::type CreateComponentCreatorInstance()
    {
        return std::make_unique<StaticMeshComponentCreator<ComponentType>>();
    }

    template <typename ComponentType, ComponentMetaType componentMetaType>
    typename std::enable_if<componentMetaType == ComponentMetaType::WaterPlane, std::unique_ptr<IComponentCreator>>::type CreateComponentCreatorInstance()
    {
        return std::make_unique<WaterPlaneComponentCreator<ComponentType>>();
    }

    template <typename ComponentType, ComponentMetaType componentMetaType>
    typename std::enable_if<componentMetaType == ComponentMetaType::SkeletalMesh, std::unique_ptr<IComponentCreator>>::type CreateComponentCreatorInstance()
    {
        return std::make_unique<SkeletalMeshComponentCreator<ComponentType>>();
    }

    template <typename ComponentType, ComponentMetaType componentMetaType>
    typename std::enable_if<componentMetaType == ComponentMetaType::Cubemap, std::unique_ptr<IComponentCreator>>::type CreateComponentCreatorInstance()
    {
        return std::make_unique<CubemapComponentCreator<ComponentType>>();
    }

    template <typename ComponentType, ComponentMetaType componentMetaType>
    typename std::enable_if<componentMetaType == ComponentMetaType::Billboard, std::unique_ptr<IComponentCreator>>::type CreateComponentCreatorInstance()
    {
        return std::make_unique<BillboardComponentCreator<ComponentType>>();
    }

    template <typename ComponentType, ComponentMetaType componentMetaType>
    typename std::enable_if<componentMetaType == ComponentMetaType::Input, std::unique_ptr<IComponentCreator>>::type CreateComponentCreatorInstance()
    {
        return std::make_unique<InputComponentCreator<ComponentType>>();
    }

    template <typename ComponentType, ComponentMetaType componentMetaType>
    typename std::enable_if<componentMetaType == ComponentMetaType::CharacterMovement, std::unique_ptr<IComponentCreator>>::type CreateComponentCreatorInstance()
    {
        return std::make_unique<CharacterMovementComponentCreator<ComponentType>>();
    }

    template <typename ComponentType, ComponentMetaType componentMetaType>
    typename std::enable_if<componentMetaType == ComponentMetaType::Movement, std::unique_ptr<IComponentCreator>>::type CreateComponentCreatorInstance()
    {
        return std::make_unique<MovementComponentCreator<ComponentType>>();
    }

    template <typename ComponentType, ComponentMetaType componentMetaType>
    typename std::enable_if<componentMetaType == ComponentMetaType::Physics, std::unique_ptr<IComponentCreator>>::type CreateComponentCreatorInstance()
    {
        return std::make_unique<PhysicsComponentCreator<ComponentType>>();
    }

    template <typename ComponentType, ComponentMetaType componentMetaType>
    typename std::enable_if<componentMetaType == ComponentMetaType::PlanarReflection, std::unique_ptr<IComponentCreator>>::type CreateComponentCreatorInstance()
    {
        return std::make_unique<PlanarReflectionComponentCreator<ComponentType>>();
    }

    template <typename ComponentType, ComponentMetaType componentMetaType>
    struct ComponentCreatorFactory
    {
        std::shared_ptr<Component> CreateComponent(const ComponentData &data,
                                                   class Scene *const scene)
        {
            const auto &componentCreator = CreateComponentCreatorInstance<ComponentType, componentMetaType>();
            return componentCreator->CreateComponent(data, scene);
        }
    };
} // namespace Game