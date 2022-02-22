#pragma once

#include <memory>
#include <string>

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhysicsShapeBase.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

using namespace Graphics;
using namespace EnginePhysics;

namespace EngineCore
{
   class SceneComponent;

   class EngineObjectCreator
   {
   public:
      static std::shared_ptr<Actor> CreateActorByString(const std::string &gameObjectName, std::shared_ptr<SceneComponent> rootComponent);

      static std::shared_ptr<ACamera> CreateThirdPersonCamera(const std::string &cameraName, std::shared_ptr<Scene> scene,
                                                              const ViewPortInfo &viewPort,
                                                              const float initPitchDeg, const float initYawDeg,
                                                              const float camDistanceToThirdPersonTarget, const glm::vec3 &thirdPersonTargetOffset, const bool bIsMainSceneCamera);
      static std::shared_ptr<ACamera> CreateFirstPersonCamera(const std::string &cameraName, std::shared_ptr<Scene> scene,
                                                              const ViewPortInfo &viewPort,
                                                              const float initPitchDeg, const float initYawDeg, const glm::vec3 &cameraPosition, const bool bIsMainSceneCamera);

      static std::shared_ptr<Component> CreateComponentByString(const std::string &componentType, ComponentData *data,
                                                                std::shared_ptr<Scene> scene);

      static ProjectedShadowInfo *CreateProjectedShadowInfo(const std::string &lightType, const glm::ivec2 &shadowAtlasSize);

      static ComponentData *CreateSpotlightComponentData(const std::string &gameObjectName, const glm::vec3 &translation,
                                                         const glm::vec3 &rotation,
                                                         const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular,
                                                         const glm::vec3 &attenutation, float radianceRadius, float cutoff, ProjectedShadowInfo *shadowInfo);
      static ComponentData *CreatePointLightComponentData(const std::string &gameObjectName,
                                                          const glm::vec3 &translation, const glm::vec3 &ambient,
                                                          const glm::vec3 &diffuse, const glm::vec3 &specular, const glm::vec3 &attenutation,
                                                          float radianceRadius, ProjectedShadowInfo *shadowInfo);
      static ComponentData *CreateDirLightComponentData(const std::string &gameObjectName,
                                                        const glm::vec3 &rotation,
                                                        const glm::vec3 &direction,
                                                        const glm::vec3 &ambient,
                                                        const glm::vec3 &diffuse, const glm::vec3 &specular, ProjectedShadowInfo *shadowInfo);
      static ComponentData *CreateMeshComponentData(const std::string &gameObjectName, const std::string &relativePathToMesh,
                                                    const glm::vec3 &translation,
                                                    const glm::vec3 &rotation, const glm::vec3 &scale, const std::string &luaPathToFile, IMaterial *material);
      static ComponentData *CreateSimpleMeshComponentData(const std::string &gameObjectName, const std::string &simpleMeshType,
                                                          const glm::vec3 &translation,
                                                          const glm::vec3 &rotation, const glm::vec3 &scale, const std::string &luaPathToFile, IMaterial *material);
      static ComponentData *CreatePhysicsComponentData(const std::string &gameObjectName, PhysicsDescriptor *physDescriptor);
      static ComponentData *CreateCharacterMovementComponentData(const std::string &gameObjectName, const glm::vec3 &launchDirection,
                                                                 const std::string &cameraName);
      static ComponentData *CreatePlatformTraverseComponentData(const std::string &gameObjectName, const std::string &scriptName);
      static ComponentData *CreateInputComponentData(const std::string &gameObjectName);
      static ComponentData *CreateSkyboxComponentData(const std::string &gameObjectName, const glm::vec3 &scale, IMaterial *material);
      static ComponentData *CreateWaterPlaneComponentData(const std::string &gameObjectName, const glm::vec3 &translation,
                                                          const glm::vec3 &rotation, const glm::vec3 &scale, IMaterial *materialInstance);

      static ComponentData *CreatePlanarReflectionComponentData(const std::string &gameObjectName, const glm::vec3 &translation,
                                                                const glm::vec3 &rotation, const glm::vec3 &scale, ACamera *ownerCamera,
                                                                const ViewPortInfo &fboViewPortInfo);

      static PhysicsShapeBase *CreatePhysicsBoxShape(const glm::vec3 &halfExtent);
      static PhysicsShapeBase *CreatePhysicsCapsuleShape(const double radius, const double height);
      static PhysicsShapeBase *CreatePhysicsPlaneShape(const glm::vec3 &normal, const double d);
      static PhysicsShapeBase *CreatePhysicsSphereShape(const double radius);
      static PhysicsShapeBase *CreatePhysicsCompoundShape();
      static void AddChildShapeToCompoundShape(PhysicsShapeBase *compoundShape, PhysicsShapeBase *childShape, const glm::vec3 &translation,
                                               const glm::vec3 &rotation);

      static PhysicsDescriptor *CreateRigidBodyController(PhysicsWorld *physWorld, PhysicsShapeBase *phyShape, const std::string &bodyType,
                                                          const float mass);
      static PhysicsDescriptor *CreateRigidBodyController(PhysicsWorld *physWorld, PhysicsShapeBase *phyShape, const PhysicsBodyType &bodyType,
                                                          const float mass);
      static PhysicsDescriptor *CreateDynamicCharacterController(PhysicsWorld *physWorld, float capsuleRadius, float capsuleHeight,
                                                                 float mass, float stepHeight);
   };

}
