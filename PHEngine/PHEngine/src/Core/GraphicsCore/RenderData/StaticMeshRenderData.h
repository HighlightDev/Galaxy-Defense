#pragma once

#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
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
         std::shared_ptr<ICompositeShader> m_materialShader;
         std::shared_ptr<IMaterial> mMaterialInstance;

         StaticMeshRenderData(
            std::shared_ptr<Skin> staticMesh,
            std::shared_ptr<IShader> materialShader,
            std::shared_ptr<IMaterial> materialInstance)
            : m_skin(staticMesh)
            , m_materialShader(std::dynamic_pointer_cast<ICompositeShader>(materialShader))
            , mMaterialInstance(materialInstance)
         {
         }

         ~StaticMeshRenderData() { }
      };
   }
}
