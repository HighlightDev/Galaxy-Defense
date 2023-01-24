#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

#include <string>
#include <glm/vec3.hpp>

namespace EngineCore
{
   enum class eMeshComponentDataType
   {
      STATIC_OR_SKELETAL_MESH,
      RUNTIME_GENERATED_MESH
   };

   struct MeshComponentData : public ComponentData
   {
      MeshComponentData(const std::string &gameObjectName,
                        const std::string &pathToMesh,
                        const glm::vec3 &translation,
                        const glm::vec3 &rotation,
                        const glm::vec3 &scale,
                        const std::string &mLuaScriptRelPath,
                        Graphics::IMaterial *material)
          : ComponentData(gameObjectName),
            m_pathToMesh(pathToMesh),
            m_translation(translation),
            m_eulerRotationDegrees(rotation),
            m_scale(scale),
            m_luaScriptPath(mLuaScriptRelPath),
            m_material(material)
      {
      }

      std::string m_pathToMesh;
      glm::vec3 m_translation;
      glm::vec3 m_eulerRotationDegrees;
      glm::vec3 m_scale;
      std::string m_luaScriptPath;

      Graphics::IMaterial *m_material;

      virtual ~MeshComponentData() {}

      virtual eMeshComponentDataType GetMeshComponentDataType() const { return eMeshComponentDataType::STATIC_OR_SKELETAL_MESH; }
   };

   struct RuntimeGeneratedMeshComponentData
       : public MeshComponentData
   {
      RuntimeGeneratedMeshComponentData(const std::string &gameObjectName,
                                        const size_t maxVerticesCount,
                                        const glm::vec3 &translation,
                                        const glm::vec3 &rotation,
                                        const glm::vec3 &scale,
                                        const std::string &mLuaScriptRelPath,
                                        Graphics::IMaterial *material)
          : MeshComponentData(gameObjectName,
                              "",
                              translation,
                              rotation,
                              scale,
                              mLuaScriptRelPath,
                              material),
            mMaxVerticesCount(maxVerticesCount)
      {
      }

      size_t mMaxVerticesCount;

      virtual ~RuntimeGeneratedMeshComponentData() {}

      eMeshComponentDataType GetMeshComponentDataType() const override { return eMeshComponentDataType::RUNTIME_GENERATED_MESH; }
   };
}