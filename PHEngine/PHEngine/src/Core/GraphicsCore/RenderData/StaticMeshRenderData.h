#pragma once

#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"

using namespace Graphics::Mesh;
using namespace Graphics::OpenGL;
using namespace Graphics::Texture;

namespace Graphics
{
   namespace Data
   {

      struct StaticMeshRenderData
      {
         std::shared_ptr<Skin> m_skin;
         std::shared_ptr<IShader> m_materialShader;
         std::shared_ptr<IShader> m_planarReflectionShader;
         std::shared_ptr<MaterialProxy> mMaterialProxy;

         StaticMeshRenderData(
            std::shared_ptr<Skin> staticMesh,
            std::shared_ptr<IShader> materialShader,
            std::shared_ptr<IShader> planarReflectionShader,
            std::shared_ptr<MaterialProxy> materialProxy)
            : m_skin(staticMesh)
            , m_materialShader(materialShader)
            , m_planarReflectionShader(planarReflectionShader)
            , mMaterialProxy(materialProxy)
         {
         }

         ~StaticMeshRenderData() { }
      };
   }
}
